#include "stm32_eval_sdio_sd.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_sdio.h"
#include "stdbool.h"
#include <rtthread.h>
#define SDIO_CMD0TIMEOUT                ((uint32_t)0x00002710)
#define SDIO_INIT_CLK_DIV                  ((uint8_t)0xB2)
#define SDIO_FIFO_ADDRESS                ((uint32_t)0x40018080)
#define SD_DATATIMEOUT                  ((uint32_t)0xFFFFFFFF)
SDIO_InitTypeDef SDIO_InitStructure;
SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;
#define SDIO_STATIC_FLAGS               ((uint32_t)0x000005FF)
#define SD_STD_CAPACITY                 ((uint32_t)0x00000000)
mmc_card_t card;
rt_uint32_t CmdResp4Error(uint8_t cmd);
void acquire_io()
{
	 rt_sem_take(&(card->sem_lock), RT_WAITING_FOREVER);
}
void release_io()
{
	 rt_sem_release(&(card->sem_lock));
}

static SD_Error CmdError(void)
{
	 SD_Error errorstatus = SD_OK;
	 uint32_t timeout;

	 timeout = SDIO_CMD0TIMEOUT; /* 10000 */

	 while ((timeout > 0) && (SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) == RESET))
	 {
		  timeout--;
	 }

	 if (timeout == 0)
	 {
		  errorstatus = SD_CMD_RSP_TIMEOUT;
		  return(errorstatus);
	 }

	 /* Clear all the static flags */
	 SDIO_ClearFlag(SDIO_STATIC_FLAGS);

	 return(errorstatus);
}

static int rw_ioreg(ioreg_t * ioreg)
{
	 int ret;
	 rt_uint16_t argh = 0, argl = 0,response;

	 /* Nicely fill up argh and argl send the command down
	  * Read the response from MMC_R1
	  */

	 acquire_io();

	 /* SDIO Spec: CMD52 is 48 bit long. 
		-----------------------------------------------------------
		S|D|CMDIND|R|FUN|F|S|REGISTER  ADDRESS|S|WRITEBIT|CRC    7|
		-----------------------------------------------------------
		The Command and the Command index will be filled by the SDIO Controller
		(48 - 16)
		So fill up argh (16 bits) argl (16 bits) with 
		R/W flag (1)
		FUNC NUMBER (3)
		RAW FLAG (1)
		STUFF (1)
		REG ADDR (17)
		and the Write data value or a Null if read
		*/
	 argh =
		  (ioreg->read_or_write ? (1 << 15) : 0) |
		  (ioreg->function_num << 12) |
		  (ioreg->read_or_write == 2 ? (1 << 11) : 0) |
		  ((ioreg->reg_addr & 0x0001ff80) >> 7);

	 argl =
		  ((ioreg->reg_addr & 0x0000007f) << 9) |
		  (ioreg->read_or_write ? ioreg->dat : 0);

	 //MMC_CMD = CMD(52);
	 //MMC_ARGH = argh;
	 //MMC_ARGL = argl;
	 //wmb();
	 SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SDIO_RW_DIRECT;
	 SDIO_CmdInitStructure.SDIO_Argument = (argh<<16)|argl;

#define CARD_RESET 8
#define HOST_INTSTATUS_REG 0x05
	 /* Disabling controller to check for SDIO interrupt from the card
		solves the extra interrupt issue. The next CMD52 write will re-enable it. */
	 if ((ioreg->reg_addr == HOST_INTSTATUS_REG) &&
			   !ioreg->read_or_write && (ioreg->function_num == FN1))
	 {
		  //MMC_CMDAT = MMC_CMDAT_R1 & (~MMC_CMDAT_SDIO_INT_EN);
		  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		  SDIO_CmdInitStructure.SDIO_Response=SDIO_Response_Short;
	 }
	 else if ((ioreg->reg_addr == IO_ABORT_REG) && ioreg->read_or_write &&
			   (ioreg->function_num == FN0) &&
			   ((ioreg->dat & CARD_RESET) == CARD_RESET))
	 {
		  //MMC_CMDAT = MMC_CMDAT_SDIO_INT_EN;
		  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_IT;
		  SDIO_CmdInitStructure.SDIO_Response=SDIO_Response_No;
	 }
	 else
	 {
		  //MMC_CMDAT = MMC_CMDAT_R1;       /* R5 */
		  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		  SDIO_CmdInitStructure.SDIO_Response=SDIO_Response_Short;

	 }

	 //ret = send_command(ctrller, MMC_R5, 0);
	 SDIO_SendCommand(&SDIO_CmdInitStructure);

	 response=CmdResp4Error(SD_CMD_SDIO_RW_DIRECT);
	 //   rt_kprintf("CMD52 respond %x\r\n",response);

	 ioreg->dat = response;

	 rt_kprintf("ioreg->dat = %x\n", ioreg->dat);

	 release_io();

	 return 0;

exit:
	 release_io();
	 return -1;
}

static int check_for_valid_ioreg(ioreg_t * ioreg)
{

	 if (!ioreg) {
		  rt_kprintf("Bad ioreg\n");
		  return -1;
	 }

	 if (ioreg->function_num > 7 || ioreg->function_num >
			   card->info.num_of_io_funcs) {

		  return -1;
	 }

	 /* Check register range */
	 if ((ioreg->reg_addr & 0xfffe0000) != 0x0) {
		  return -1;
	 }

	 return 0;
}

int sdio_read_ioreg(rt_uint8_t func, rt_uint32_t reg, rt_uint8_t * dat)
{
	 int ret = -1;
	 ioreg_t ioreg;

	 /* SDIO Spec: Command 52 needs 
	  * R/W Flag if 0 this will be read data if 1 write
	  * Function number: Number of the function within I/O 
	  * Register Address: This is the address of the  register
	  * write data: This bit is set to 0 for read
	  * CRC7: 7 bits CRC
	  */

	 ioreg.read_or_write = SDIO_READ;
	 ioreg.function_num = func;
	 ioreg.reg_addr = reg;
	 ioreg.dat = 0x00;     /** Will be filled by the card */

	 ret = check_for_valid_ioreg( &ioreg);

	 if (ret < 0) {
		  rt_kprintf("Wrong parameters for rw_ioreg\n");
		  goto exit;
	 }

	 ret = rw_ioreg(&ioreg);

	 if (ret < 0) {
		  rt_kprintf("rw_ioreg failed\n");
	 } else
		  *dat = ioreg.dat;

exit:
	 return ret;
}

int sdio_write_ioreg(rt_uint8_t func, rt_uint32_t reg, rt_uint8_t dat)
{
	 int ret;
	 ioreg_t ioreg;

	 /* SDIO Spec: Command 52 needs 
	  * R/W Flag if 0 this will be read data if 1 write
	  * Function number: Number of the function within I/O 
	  * Register Address: This is the address of the byte of data inside of 
	  * the write data: This bit is set to 0 for read
	  * CRC7: 7 bits CRC
	  */

	 ioreg.read_or_write = SDIO_WRITE;
	 ioreg.function_num = func;
	 ioreg.reg_addr = reg;
	 ioreg.dat = dat;     /** Will be filled by the card */

	 ret = check_for_valid_ioreg(&ioreg);

	 if (ret < 0) {
		  rt_kprintf("Wrong parameters for rw_ioreg\n");
		  goto exit;
	 }

	 ret = rw_ioreg(&ioreg);

	 if (ret < 0) {
		  rt_kprintf("rw_ioreg failed\n");
	 }

exit:
	 return ret;
}

static int get_ioblk_size()
{
	 int ret = -1;
	 int fn;
	 rt_uint8_t dat;

	 for (fn = 0; fn <= card->info.num_of_io_funcs; fn++) {
		  /* Low byte */
		  ret = sdio_read_ioreg(FN0,FN_BLOCK_SIZE_0_REG(fn), &dat);
		  rt_kprintf("low byte of ioblk %d\r\n",dat);
		  /* High byte */
		  ret = sdio_read_ioreg(FN0,FN_BLOCK_SIZE_1_REG(fn), &dat);
		  rt_kprintf("High byte of ioblk %d\r\n",dat);
	 }

	 return ret;

exit:
	 rt_kprintf("Recived a Null controller returning back\n");
	 return -1;
}

static int get_cisptr_address()
{
	 int ret, fn;
	 rt_uint8_t dat;

	 for (fn = 0; fn <= card->info.num_of_io_funcs; fn++) {

		  ret = sdio_read_ioreg(FN0,FN_CIS_POINTER_0_REG(fn), &dat);

		  card->info.cisptr[fn] = (((rt_uint32_t) dat) << 0);

		  rt_kprintf("dat = %x\n", dat);

		  ret = sdio_read_ioreg(FN0,FN_CIS_POINTER_1_REG(fn), &dat);

		  card->info.cisptr[fn] |= (((rt_uint32_t) dat) << 8);

		  rt_kprintf("dat = %x\n", dat);

		  ret = sdio_read_ioreg(FN0,FN_CIS_POINTER_2_REG(fn), &dat);

		  card->info.cisptr[fn] |= (((rt_uint32_t) dat) << 16);

		  rt_kprintf("dat = %x\n", dat);

		  rt_kprintf("Card CIS Addr = %x fn = %d\n",
					card->info.cisptr[fn], fn);

	 }

	 return 0;
}

static int read_manfid(int func)
{

	 int offset = 0;
	 rt_uint32_t manfid, card_id, ret = 0;
	 rt_uint8_t tuple, link, datah, datal;

	 do {
		  ret = sdio_read_ioreg(func,
					card->info.cisptr[func]
					+ offset, &tuple);

		  if (ret < 0)
			   return ret;

		  if (tuple == CISTPL_MANFID) {
			   offset += 2;

			   ret = sdio_read_ioreg(func,
						 card->info.cisptr[func]
						 + offset, &datal);
			   if (ret < 0)
					return ret;

			   offset++;

			   ret = sdio_read_ioreg(func,
						 card->info.cisptr[func]
						 + offset, &datah);
			   if (ret < 0)
					return ret;

			   manfid = datal | datah << 8;
			   card->manf_id = manfid;

			   offset++;

			   ret = sdio_read_ioreg(func,
						 card->info.cisptr[func]
						 + offset, &datal);
			   if (ret < 0)
					return ret;

			   offset++;

			   ret = sdio_read_ioreg(func,
						 card->info.cisptr[func]
						 + offset, &datah);
			   if (ret < 0)
					return ret;

			   card_id = datal | datah << 8;

			   card->dev_id = card_id;

			   rt_kprintf("Card id = 0%2x manfid = %x\n", card_id, manfid);
			   return manfid;
		  }

		  ret = sdio_read_ioreg(func,
					card->info.cisptr[func] +
					offset + 1, &link);
		  if (ret < 0)
			   return ret;

		  offset += link + 2;

	 } while (tuple != CISTPL_END);

	 return -1;
}
int sbi_probe_card()
{

	 int ret = 0;
	 rt_uint8_t bic;

	 /* Check for MANFID */
	 ret = read_manfid(FN0);

#define MARVELL_VENDOR_ID 0x02df
	 if (ret == MARVELL_VENDOR_ID) {
		  rt_kprintf("Marvell SDIO card detected!\n");
	 } else {
		  rt_kprintf("Ignoring a non-Marvell SDIO card...\n");
		  ret = -1;
		  goto done;
	 }

	 /* read Revision Register to get the hw revision number */
	 if (sdio_read_ioreg(FN1, CARD_REVISION_REG, &card->chiprev) < 0) {
		  rt_kprintf("cannot read CARD_REVISION_REG\n");
	 } else {
		  rt_kprintf("revsion=0x%x\n", card->chiprev);
		  switch (card->chiprev) {
			   default:
					card->block_size_512 = RT_TRUE;
					card->async_int_mode = RT_TRUE;

					/* enable async interrupt mode */
					ret = sdio_read_ioreg(FN0, BUS_INTERFACE_CONTROL_REG, &bic);
					if (ret < 0) {
						 ret = -1;
						 goto done;
					}
					bic |= ASYNC_INT_MODE;
					ret = sdio_write_ioreg(FN0, BUS_INTERFACE_CONTROL_REG, bic);
					if (ret < 0) {
						 ret = -1;
						 goto done;
					}
					break;
		  }
	 }

	 ret = 0;
done:

	 return ret;
}
static int disable_host_int_mask(rt_uint8_t mask)
{
	 int ret = 0;
	 rt_uint8_t host_int_mask;

	 /* Read back the host_int_mask register */
	 ret = sdio_read_ioreg(FN1, HOST_INT_MASK_REG,
			   &host_int_mask);
	 if (ret < 0) {
		  ret = -1;
		  goto done;
	 }

	 /* Update with the mask and write back to the register */
	 host_int_mask &= ~mask;
	 ret = sdio_write_ioreg(FN1, HOST_INT_MASK_REG,
			   host_int_mask);
	 if (ret < 0) {
		  rt_kprintf("Unable to diable the host interrupt!\n");
		  ret = -1;
		  goto done;
	 }

done:
	 return ret;
}
SD_Error SD_ProcessIRQSrc(void)
{
	rt_kprintf("in SD_ProcessIRQSrc %x\r\n",SDIO_GetITStatus(SDIO_IT_SDIOIT));
	if(SDIO_GetITStatus(SDIO_IT_SDIOIT)==SET)
	SDIO_ClearITPendingBit(SDIO_IT_SDIOIT);
}
void SDIO_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    /* Process All SDIO Interrupt Sources */
    SD_ProcessIRQSrc();

    /* leave interrupt */
    rt_interrupt_leave();
}

int sdio_request_irq()
{
	 int ret, func;
	 rt_uint8_t io_enable_reg, int_enable_reg;
	 //sdio_ctrller *ctrller;

	 //_ENTER();

	 //net_devid = dev_id;

	 //sdioint_handler = handler;

	 //_DBGMSG("address of dev_id = %p sdioint_handler =%p\n",
	//		   net_devid, sdioint_handler);
	 if (!card->card_int_ready)
		  card->card_int_ready = YES;

	 io_enable_reg = 0x0;

	 int_enable_reg = IENM;

	 for (func = 1; func <= 7 && func <=
			   card->info.num_of_io_funcs; func++) {

		  io_enable_reg |= IOE(func);
		  int_enable_reg |= IEN(func);
	 }

	 /** Enable function IOs on card */
	 sdio_write_ioreg(FN0, IO_ENABLE_REG, io_enable_reg);
	 /** Enable function interrupts on card */
	 sdio_write_ioreg(FN0, INT_ENABLE_REG, int_enable_reg);

	 //MMC_I_MASK &= ~MMC_I_MASK_SDIO_INT;
	 //wmb();
	 //_PRINTK("sdio_request_irq: MMC_I_MASK = 0x%x\n", MMC_I_MASK);

//#ifdef DEBUG_SDIO_LEVEL1
	 //print_addresses(ctrller);
//#endif

	 ret = 0;
//	 _LEAVE();

done:
	 return ret;
}

int sbi_register_dev()
{
	 int ret = 0;
	 rt_uint8_t reg;
	 rt_uint32_t ioport;
	 //mmc_card_t card = (mmc_card_t) priv->wlan_dev.card;

	 //ENTER();

	 /* Initialize the private structure */
	 //strncpy(priv->wlan_dev.name, "sdio0", sizeof(priv->wlan_dev.name));
	 //priv->wlan_dev.ioport = 0;
	 //priv->wlan_dev.upld_rcv = 0;
	 //priv->wlan_dev.upld_typ = 0;
	 //priv->wlan_dev.upld_len = 0;
	 ioport=0;
	 /* Read the IO port */
	 ret = sdio_read_ioreg(FN1, IO_PORT_0_REG, &reg);
	 if (ret < 0)
		  goto failed;
	 else
		  ioport |= reg;

	 ret = sdio_read_ioreg(FN1, IO_PORT_1_REG, &reg);
	 if (ret < 0)
		  goto failed;
	 else
		  ioport |= (reg << 8);

	 ret = sdio_read_ioreg(FN1, IO_PORT_2_REG, &reg);
	 if (ret < 0)
		  goto failed;
	 else
		  ioport |= (reg << 16);

	 rt_kprintf("SDIO FUNC1 IO port: 0x%x\n", ioport);

	 /* Disable host interrupt first. */
	 if ((ret = disable_host_int_mask(0xff)) < 0) {
		  rt_kprintf("Warning: unable to disable host interrupt!\n");
	 }

	 /* Request the SDIO IRQ */
	// rt_kprintf("Before request_irq Address is if==>%p\n", isr_function);
	 ret = sdio_request_irq();

	 if (ret < 0) {
		  rt_kprintf("Failed to request IRQ on SDIO bus (%d)\n", ret);
		  goto failed;
	 }

	 //rt_kprintf(INFO, "IrqLine: %d\n", card->ctrlr->tmpl->irq_line);
	 //priv->wlan_dev.netdev->irq = card->ctrlr->tmpl->irq_line;
	 //priv->adapter->irq = priv->wlan_dev.netdev->irq;
	 //priv->adapter->chip_rev = card->chiprev;
	 //priv->adapter->sdiomode =
	//	  ((mmc_card_t) ((priv->wlan_dev).card))->ctrlr->bus_width;
//#if defined(_MAINSTONE) || defined(SYSKT)
//	 if (request_gpio_irq_callback
//			   ((void (*)(void *)) gpio_irq_callback, (void *) priv))
//		  rt_kprintf(FATAL, "Failed to request GPIO IRQ callback\n");
//#endif /* _MAINSTONE || SYSKT */

	 return 0;

failed:
	 //priv->wlan_dev.card = NULL;

	 return -1;
}
int sbi_get_cis_info()
{
    //mmc_card_t card = (mmc_card_t) priv->wlan_dev.card;
    //wlan_adapter *Adapter = priv->adapter;
    rt_uint8_t tupledata[255];
    int i;
    rt_uint32_t ret = 0;

    //ENTER();

    /* Read the Tuple Data */
    for (i = 0; i < sizeof(tupledata); i++) {
        ret = sdio_read_ioreg(FN0, card->info.cisptr[FN0] + i,
                              &tupledata[i]);
        if (ret) {
            rt_kprintf(WARN, "get_cis_info error:%d\n", ret);
            return WLAN_STATUS_FAILURE;
        }
    }

    /* Copy the CIS Table to Adapter */
    memset(card->CisInfoBuf, 0x0, sizeof(card->CisInfoBuf));
    memcpy(card->CisInfoBuf, tupledata, sizeof(tupledata));
    card->CisInfoLen = sizeof(tupledata);

   // LEAVE();
    return 0;
}
int sbi_disable_host_int()
{
    return disable_host_int_mask(HIM_DISABLE);
}
static int mv_sdio_read_scratch(rt_uint16_t * dat)
{
    int ret = 0;
    rt_uint8_t scr0;
    rt_uint8_t scr1;
    ret = sdio_read_ioreg(FN1, CARD_OCR_0_REG, &scr0);
    if (ret < 0)
        return -1;

    ret = sdio_read_ioreg(FN1, CARD_OCR_1_REG, &scr1);
    rt_kprintf("CARD_OCR_0_REG = 0x%x, CARD_OCR_1_REG = 0x%x\n", scr0,
           scr1);
    if (ret < 0)
        return -1;

    *dat = (((u16) scr1) << 8) | scr0;

    return 0;
}
static int mv_sdio_poll_card_status(rt_uint8_t bits)
{
    int tries;
    int rval;
    rt_uint8_t cs;

    for (tries = 0; tries < MAX_POLL_TRIES; tries++) {
        rval = sdio_read_ioreg(FN1,
                               CARD_STATUS_REG, &cs);
        if (rval == 0 && (cs & bits) == bits) {
            return 0;
        }

        rt_thread_delay(10);
    }

    rt_kprintf("mv_sdio_poll_card_status: FAILED!:%d\n", rval);
    return -1;
}

static int sbi_prog_firmware_image(const rt_uint8_t * firmware, int firmwarelen)
{
    int ret = 0;
    rt_uint16_t firmwarestat;
    rt_uint8_t *fwbuf = card->TmpTxBuf;
    int fwblknow;
    rt_uint32_t tx_len;

    //ENTER();

    /* Check if the firmware is already downloaded */
    if ((ret = mv_sdio_read_scratch(&firmwarestat)) < 0) {
        rt_kprintf("read scratch returned <0\n");
        goto done;
    }

    if (firmwarestat == FIRMWARE_READY) {
        rt_kprintf("FW already downloaded!\n");
        ret = 0;
        goto done;
    }

    rt_kprintf("Downloading helper image (%d bytes), block size %d bytes\n",
           firmwarelen, SD_BLOCK_SIZE_FW_DL);

    /* Perform firmware data transfer */
    tx_len =
        (FIRMWARE_TRANSFER_NBLOCK * SD_BLOCK_SIZE_FW_DL) - SDIO_HEADER_LEN;
    for (fwblknow = 0; fwblknow < firmwarelen; fwblknow += tx_len) {

        /* The host polls for the DN_LD_CARD_RDY and CARD_IO_READY bits */
        ret = mv_sdio_poll_card_status(CARD_IO_READY | DN_LD_CARD_RDY);
        if (ret < 0) {
            rt_kprintf("FW download died @ %d\n", fwblknow);
            goto done;
        }

        /* Set blocksize to transfer - checking for last block */
        if (firmwarelen - fwblknow < tx_len)
            tx_len = firmwarelen - fwblknow;

        fwbuf[0] = ((tx_len & 0x000000ff) >> 0);        /* Little-endian */
        fwbuf[1] = ((tx_len & 0x0000ff00) >> 8);
        fwbuf[2] = ((tx_len & 0x00ff0000) >> 16);
        fwbuf[3] = ((tx_len & 0xff000000) >> 24);

        /* Copy payload to buffer */
        memcpy(&fwbuf[SDIO_HEADER_LEN], &firmware[fwblknow], tx_len);

        rt_kprintf(".");

        /* Send data */
        ret = sdio_write_iomem(priv->wlan_dev.card, FN1,
                               priv->wlan_dev.ioport, BLOCK_MODE,
                               FIXED_ADDRESS, FIRMWARE_TRANSFER_NBLOCK,
                               SD_BLOCK_SIZE_FW_DL, fwbuf);

        if (ret < 0) {
            rt_kprintf("IO error: transferring block @ %d\n", fwblknow);
            goto done;
        }
    }

#ifdef FW_DOWNLOAD_SPEED
    tv2 = get_utimeofday();
    rt_kprintf("helper: %ld.%03ld.%03ld ", tv1 / 1000000,
           (tv1 % 1000000) / 1000, tv1 % 1000);
    rt_kprintf(" -> %ld.%03ld.%03ld ", tv2 / 1000000,
           (tv2 % 1000000) / 1000, tv2 % 1000);
    tv2 -= tv1;
    rt_kprintf(" == %ld.%03ld.%03ld\n", tv2 / 1000000,
           (tv2 % 1000000) / 1000, tv2 % 1000);
#endif

    /* Write last EOF data */
    rt_kprintf("\nTransferring EOF block\n");
    memset(fwbuf, 0x0, SD_BLOCK_SIZE_FW_DL);
    ret = sdio_write_iomem(priv->wlan_dev.card, FN1,
                           priv->wlan_dev.ioport, BLOCK_MODE,
                           FIXED_ADDRESS, 1, SD_BLOCK_SIZE_FW_DL, fwbuf);

    if (ret < 0) {
        rt_kprintf("IO error in writing EOF FW block\n");
        goto done;
    }

    ret = 0;

  done:
    return ret;
}

int wlan_setup_station_hw()
{
sbi_disable_host_int();

}
rt_uint32_t CmdResp4Error(uint8_t cmd)
{
	 SD_Error errorstatus = SD_OK;
	 uint32_t status;
	 uint32_t response_r1;
	 uint32_t response_r2;

	 status = SDIO->STA;
	 //rt_kprintf("1 status=%x\r\n",status);
	 if(SDIO_CmdInitStructure.SDIO_Wait==SDIO_Wait_IT)
	 {
		  while (!(status & (SDIO_IT_SDIOIT)))
		  {    
			   status = SDIO->STA;
		  }
	 }
	 else
	 {
		  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
		  {    
			   status = SDIO->STA;
		  }
	 }
	 //  rt_kprintf("2 status=%x\r\n",status);
	 if (status & SDIO_FLAG_CTIMEOUT)
	 {
		  errorstatus = SD_CMD_RSP_TIMEOUT;
		  SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);	   
		  return(errorstatus);
	 }
	 else if (status & SDIO_FLAG_CCRCFAIL)
	 {
		  errorstatus = SD_CMD_CRC_FAIL;
		  SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
		  return(errorstatus);
	 }

	 /* Check response received is of desired command */
	 if ((SDIO_GetCommandResponse() != cmd)&&(SDIO_GetCommandResponse()!=0x3f))
	 {
		  rt_kprintf("illegal cmd %x\r\n",SDIO_GetCommandResponse());
		  errorstatus = SD_ILLEGAL_CMD;
		  return(errorstatus);
	 }
	 /* Clear all the static flags */
	 SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	 /* We have received response, retrieve it for analysis  */
	 response_r1 = SDIO_GetResponse(SDIO_RESP1);		
	 return response_r1;
}
void SD_LowLevel_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;

	 /*!< GPIOC and GPIOD Periph clock enable */
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

	 /*!< Configure PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK pin */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	 GPIO_Init(GPIOC, &GPIO_InitStructure);

	 /*!< Configure PD.02 CMD line */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	 GPIO_Init(GPIOD, &GPIO_InitStructure);

	 /*!< Enable the SDIO AHB Clock */
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO, ENABLE);

	 /*!< Enable the DMA2 Clock */
	 //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
}
void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
{

	 DMA_InitTypeDef DMA_InitStructure;

	 DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);

	 /*!< DMA2 Channel4 disable */
	 DMA_Cmd(DMA2_Channel4, DISABLE);

	 /*!< DMA2 Channel4 Config */
	 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
	 DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)BufferSRC;
	 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	 DMA_InitStructure.DMA_BufferSize = BufferSize / 4;
	 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	 DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	 DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	 DMA_Init(DMA2_Channel4, &DMA_InitStructure);

	 /*!< DMA2 Channel4 enable */
	 DMA_Cmd(DMA2_Channel4, ENABLE);  
}
void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize)
{
	 DMA_InitTypeDef DMA_InitStructure;

	 DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);

	 /*!< DMA2 Channel4 disable */
	 DMA_Cmd(DMA2_Channel4, DISABLE);

	 /*!< DMA2 Channel4 Config */
	 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
	 DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)BufferDST;
	 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	 DMA_InitStructure.DMA_BufferSize = BufferSize / 4;
	 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	 DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	 DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	 DMA_Init(DMA2_Channel4, &DMA_InitStructure);

	 /*!< DMA2 Channel4 enable */
	 DMA_Cmd(DMA2_Channel4, ENABLE); 
}
SD_Error SD_PowerON(void)
{
	 int i;

	 SD_Error errorstatus = SD_OK;
	 uint32_t response = 0, count = 0, validvoltage = 0;
	 uint32_t SDType = SD_STD_CAPACITY;
	 card=(mmc_card_t)rt_malloc(sizeof(mmc_card_rec));
	 rt_sem_init(&(card->sem_lock), "wifi_lock", 1, RT_IPC_FLAG_FIFO);
	 SD_LowLevel_Init();
	 	    NVIC_InitTypeDef NVIC_InitStructure;

	    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	    NVIC_Init(&NVIC_InitStructure);

	 /*!< Power ON Sequence -----------------------------------------------------*/
	 /*!< Configure the SDIO peripheral */
	 /*!< SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_INIT_CLK_DIV) */
	 /*!< on STM32F2xx devices, SDIOCLK is fixed to 48MHz */
	 /*!< SDIO_CK for initialization should not exceed 400 KHz */  
	 SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;
	 SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
	 SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
	 SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
	 SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
	 SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
	 SDIO_Init(&SDIO_InitStructure);

	 /*!< Set Power State to ON */
	 SDIO_SetPowerState(SDIO_PowerState_ON);

	 /*!< Enable SDIO Clock */
	 SDIO_ClockCmd(ENABLE);
	 SDIO_ITConfig(SDIO_IT_SDIOIT/*|SDIO_IT_CMDREND*/,ENABLE);
	 /*!< CMD0: GO_IDLE_STATE ---------------------------------------------------*/
	 /*!< No CMD response required */
	 SDIO_CmdInitStructure.SDIO_Argument = 0x0;
	 SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_GO_IDLE_STATE;
	 SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;
	 SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	 SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;

	 for(i=0;i<80;i++)
	 {
		  SDIO_SendCommand(&SDIO_CmdInitStructure);

		  errorstatus = CmdError();

		  if (errorstatus != SD_OK)
		  {
			   /*!< CMD Response TimeOut (wait for CMDSENT flag) */
			   rt_kprintf("CMD Go Idle timeout\r\n");
			   return(errorstatus);
		  }
	 }

	 /* CMD5:SD_CMD_SDIO_SEN_OP_COND */
	 SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SDIO_SEN_OP_COND;
	 SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;	 

	 SDIO_SendCommand(&SDIO_CmdInitStructure);
	 response=CmdResp4Error(SD_CMD_SDIO_SEN_OP_COND);
	 rt_kprintf("respond %x\r\n",response);
	 if(response==0)
		  response=0x00ffff00;
	 SDIO_CmdInitStructure.SDIO_Argument = response;

	 SDIO_SendCommand(&SDIO_CmdInitStructure);
	 response=CmdResp4Error(SD_CMD_SDIO_SEN_OP_COND);
	 rt_kprintf("2 respond %x\r\n",response);

	 card->info.num_of_io_funcs=(response&0x70000000)>>27;
	 card->info.ocr=response&0xffffff;
	 card->info.memory_yes=(response&0x08000000)?1:0;
	 /* CMD3:SD_CMD_SET_REL_ADDR */
	 SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;
	 SDIO_CmdInitStructure.SDIO_Argument = 0;
	 SDIO_SendCommand(&SDIO_CmdInitStructure);
	 response=CmdResp4Error(SD_CMD_SET_REL_ADDR);
	 rt_kprintf("3 respond %x\r\n",response);
	 if((response&0xe000)!=0)
		  rt_kprintf("get rca failed\r\n");
	 else
		  card->info.rca=(response&0xffff0000)>>16;
	 rt_kprintf("rca %d,num_fn %d\r\n",card->info.rca,card->info.num_of_io_funcs);
	 /* CMD7:SD_CMD_SEL_DESEL_CARD */
	 SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEL_DESEL_CARD;
	 SDIO_CmdInitStructure.SDIO_Argument =card->info.rca<<16;
	 SDIO_SendCommand(&SDIO_CmdInitStructure);
	 response=CmdResp4Error(SD_CMD_SEL_DESEL_CARD);
	 rt_kprintf("4 respond %x\r\n",response);
	 SDIO_SendCommand(&SDIO_CmdInitStructure);
	 response=CmdResp4Error(SD_CMD_SEL_DESEL_CARD);
	 rt_kprintf("5 respond %x\r\n",response);

	 get_ioblk_size();
	 get_cisptr_address();
	 response=read_manfid(FN0);
	 if (response == MRVL_MANFID)
		  rt_kprintf("\n Found Marvell SDIO Card (0x%04x) "
					"Initializing the driver\n", response);
	 SDIO_DeInit();
	 SDIO_InitStructure.SDIO_ClockDiv = 1;
	 SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
	 SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
	 SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
	 SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_4b;
	 SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
	 SDIO_Init(&SDIO_InitStructure);
	 SDIO_SetPowerState(SDIO_PowerState_ON);

	 /*!< Enable SDIO Clock */
	 SDIO_ClockCmd(ENABLE);
	 SDIO_ITConfig(SDIO_IT_SDIOIT/*|SDIO_IT_CMDREND*/,ENABLE);
	 sbi_probe_card();
	sbi_register_dev();
	sbi_get_cis_info();
	 return SD_OK;
}
