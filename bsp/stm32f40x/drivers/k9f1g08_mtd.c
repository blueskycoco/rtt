/*
 * File      : rtthread.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2012, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-10-13     prife        the first version 
 * 2012-03-11     prife        use mtd device interface
 * 2012-12-11	  bbstr	  	   add sst39vf10601 1.2Mbyte managed by uffs together
*/
#include <rtdevice.h>
#include "stm32f4xx_conf.h"
#include "board.h"
#ifdef RT_USING_MTD_NAND
#define FSMC_Bank_NAND     FSMC_Bank2_NAND
#define Bank_NAND_ADDR     Bank2_NAND_ADDR 
#define Bank2_NAND_ADDR    ((uint32_t)0x70000000) 

typedef struct
{
  uint8_t Maker_ID;
  uint8_t Device_ID;
  uint8_t Third_ID;
  uint8_t Fourth_ID;
}NAND_IDTypeDef;

#define CMD_AREA                   (uint32_t)(1<<16)  /* A16 = CLE  high */
#define ADDR_AREA                  (uint32_t)(1<<17)  /* A17 = ALE high */

#define DATA_AREA                  ((uint32_t)0x00000000) 

#define NAND_CMD_READ_1            ((uint8_t)0x00)
#define NAND_CMD_AREA_B            ((uint8_t)0x01)
#define NAND_CMD_AREA_C            ((uint8_t)0x50)
#define NAND_CMD_READ_TRUE        ((uint8_t)0x30)

#define NAND_CMD_WRITE0            ((uint8_t)0x80)
#define NAND_CMD_WRITE_TRUE1       ((uint8_t)0x10)

#define NAND_CMD_ERASE0            ((uint8_t)0x60)
#define NAND_CMD_ERASE1            ((uint8_t)0xD0)

#define NAND_CMD_READID            ((uint8_t)0x90)
#define NAND_CMD_STATUS            ((uint8_t)0x70)
#define NAND_CMD_LOCK_STATUS       ((uint8_t)0x7A)
#define NAND_CMD_RESET             ((uint8_t)0xFF)

#define NAND_CMD_PAGEPROGRAM       ((uint8_t)0x80)
#define NAND_CMD_PAGEPROGRAM_TRUE  ((uint8_t)0x10)


#define NAND_VALID_ADDRESS         ((uint32_t)0x00000100)
#define NAND_INVALID_ADDRESS       ((uint32_t)0x00000200)
#define NAND_TIMEOUT_ERROR         ((uint32_t)0x00000400)
#define NAND_BUSY                  ((uint32_t)0x00000000)
#define NAND_ERROR                 ((uint32_t)0x00000001)
#define NAND_READY                 ((uint32_t)0x00000040)

// Nand Flash HY27UF081G2A µÈµÈ
#define NAND_PAGE_SIZE             ((uint16_t)0x0800) /* 2048 bytes per page w/o Spare Area */
#define NAND_BLOCK_SIZE            ((uint16_t)0x0040) /* 64 pages per block */
#define NAND_ZONE_SIZE             ((uint16_t)0x0400) /* 1024 Block per zone */
#define NAND_SPARE_AREA_SIZE       ((uint16_t)0x0040) /* last 64 bytes as spare area */
#define NAND_MAX_ZONE              ((uint16_t)0x0001) /* 1 zones of 1024 block */

#define ADDR_1st_CYCLE(ADDR)       (uint8_t)((ADDR)& 0xFF)               /* 1st addressing cycle */
#define ADDR_2nd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF00) >> 8)      /* 2nd addressing cycle */
#define ADDR_3rd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF0000) >> 16)   /* 3rd addressing cycle */
#define ADDR_4th_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF000000) >> 24) /* 4th addressing cycle */  

static struct rt_mutex nand;
static struct rt_mtd_nand_device nand_part;
void NAND_Init(void);
void NAND_ReadID(NAND_IDTypeDef* NAND_ID);
uint32_t FSMC_NAND_GetStatus(void);
uint32_t  FSMC_NAND_ReadStatus(void);

void NAND_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  FSMC_NAND_PCCARDTimingInitTypeDef  p;
  FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE , ENABLE);

  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 
  
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5  | 
                                GPIO_Pin_6 | GPIO_Pin_7  | GPIO_Pin_11 |
                                GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 |GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 ;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  p.FSMC_SetupTime 		= 0x0;
  p.FSMC_WaitSetupTime 	= 0x4;
  p.FSMC_HoldSetupTime 	= 0x5;
  p.FSMC_HiZSetupTime 	= 0x0;

  FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank2_NAND;
  FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Enable;
  FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Enable;
  FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_2048Bytes;
  FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0x00;
  FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x00;
  FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;
  FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;

  FSMC_NANDInit(&FSMC_NANDInitStructure);

  FSMC_NANDCmd(FSMC_Bank2_NAND, ENABLE);
}

void NAND_ReadID(NAND_IDTypeDef* NAND_ID)
{
  uint32_t data = 0;
  *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = 0x90;
	
  *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;

  data = *(__IO uint32_t *)(Bank_NAND_ADDR | DATA_AREA);

   NAND_ID->Maker_ID   = ADDR_1st_CYCLE (data);
   NAND_ID->Device_ID  = ADDR_2nd_CYCLE (data);
   NAND_ID->Third_ID   = ADDR_3rd_CYCLE (data);
   NAND_ID->Fourth_ID  = ADDR_4th_CYCLE (data);
}
uint32_t FSMC_NAND_GetStatus(void)
{
  uint32_t timeout = 0x1000000, status = NAND_READY;

  status = FSMC_NAND_ReadStatus(); 
  while ((status != NAND_READY) &&( timeout != 0x00))
  {
     status = FSMC_NAND_ReadStatus();
     timeout --;      
  }

  if(timeout == 0x00)
  {          
    status =  NAND_TIMEOUT_ERROR;      
  } 
  return (status);      
}
uint32_t  FSMC_NAND_ReadStatus(void)
{
  uint32_t data = 0x00, status = NAND_BUSY;
  *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_STATUS;
  data = *(vu8 *)(Bank_NAND_ADDR);

  if((data & NAND_ERROR) == NAND_ERROR)
  {
    status = NAND_ERROR;
  } 
  else if((data & NAND_READY) == NAND_READY)
  {
    status = NAND_READY;
  }
  else
  {
    status = NAND_BUSY; 
  }
  
  return (status);
}
static rt_err_t nand_mtd_check_block(
		struct rt_mtd_nand_device* device,
		rt_uint32_t block)
{
	rt_err_t result=RT_ERROR;
	rt_uint8_t status=0;
	rt_uint32_t addr=(block << 6);
	rt_mutex_take(&nand, RT_WAITING_FOREVER);
	*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_READ_1; 
	   
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; 
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x08; 
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(addr);  
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(addr);

	*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_READ_TRUE; 

	while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );

	status= *(vu8 *)(Bank_NAND_ADDR);

	if((NAND_READY==FSMC_NAND_GetStatus()) && (status == 0xff))
		result=RT_EOK;
	else
		rt_kprintf("check block %d ,%d\n",addr,status);
	rt_mutex_release(&nand);
	return result;
}

static rt_err_t nand_mtd_mark_bad_block(
		struct rt_mtd_nand_device* device,
		rt_uint32_t block)
{
	rt_err_t result=RT_ERROR;	
	rt_mutex_take(&nand, RT_WAITING_FOREVER);
	rt_uint32_t addr=(block << 6);
	/* Page write command and address */
    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM;

    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;  
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x08; 
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(addr);  
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(addr);

    /* Write data */
    *(vu8 *)(Bank_NAND_ADDR | DATA_AREA) = 0x00;
    
    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM_TRUE;
    while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );
    
    /* Check status for successful operation */
    if(NAND_READY==FSMC_NAND_GetStatus())
		result=RT_EOK;
	//rt_kprintf("Mark block %d Bad\n",block);
	rt_mutex_release(&nand);
	return result;
}

static rt_err_t nand_mtd_erase_block(
		struct rt_mtd_nand_device* device,
		rt_uint32_t block)
{
	rt_err_t result=RT_ERROR;	
	rt_uint32_t addr=(block << 6);
	rt_mutex_take(&nand, RT_WAITING_FOREVER);
	*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_ERASE0;

	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(addr);  
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(addr);
		
	*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_ERASE1; 

    while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );
	if(FSMC_NAND_GetStatus()==NAND_READY)
    	result=RT_EOK;
	//rt_kprintf("Erase block %d\n",addr);
	rt_mutex_release(&nand);
	return result;
}

static rt_err_t nand_mtd_read(
		struct rt_mtd_nand_device * dev,
		rt_off_t page,
		rt_uint8_t * data, rt_uint32_t data_len, //may not always be 2048
		rt_uint8_t * spare, rt_uint32_t spare_len)
{
	rt_err_t result=RT_ERROR;
	uint32_t index = 0x0000;
  	uint32_t status = NAND_READY;
	rt_mutex_take(&nand, RT_WAITING_FOREVER);
	if (data != RT_NULL && data_len != 0)
	{
		*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_READ_1; 
	   
	    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;
	    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;
	    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(page);  
	    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(page);

	    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_READ_TRUE; 

	   	while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );

	    for(index = 0x0000; index < data_len; index++)
	    {
	      data[index]= *(vu8 *)(Bank_NAND_ADDR);
	    }
		status = FSMC_NAND_GetStatus();
	}
	if (spare != RT_NULL && spare_len != 0)
	{
		*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_READ_1; 
	   
	    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;
	    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x08;
	    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(page);  
	    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(page);

	    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_READ_TRUE; 

	   	while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );

	    for(index = 0x0000; index < spare_len; index++)
	    {
	      spare[index]= *(vu8 *)(Bank_NAND_ADDR);
	    }
		status = FSMC_NAND_GetStatus();
	}
	rt_mutex_release(&nand);
	if(NAND_READY==status)
			result=RT_EOK;
	else
		rt_kprintf("nand_mtd_read page %d, %02x %d,%02x %d failed\n",page,data,data_len,spare,spare_len);

	return result;
}

static rt_err_t nand_mtd_write (
		struct rt_mtd_nand_device * dev,
		rt_off_t page,
		const rt_uint8_t * data, rt_uint32_t data_len,//will be 2048 always!
		const rt_uint8_t * spare, rt_uint32_t spare_len)
{	
	rt_err_t result=RT_ERROR;
	uint32_t index = 0x00;
   	uint32_t status = NAND_READY;
	rt_mutex_take(&nand, RT_WAITING_FOREVER);
	//rt_kprintf("nand write %02x %d,%02x %d\r\n",data,data_len,spare,spare_len);
	if(data != RT_NULL && data_len != 0 )
	{
		RT_ASSERT(data_len == NAND_PAGE_SIZE);
		/* Page write command and address */
	    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM;

	    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;  
	    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; 
	    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(page);  
	    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(page);

	    /* Write data */
	    for(index = 0x0000; index < data_len; index++)
	    {
	      *(vu8 *)(Bank_NAND_ADDR | DATA_AREA) = data[index];
	    }
	    if(spare==RT_NULL)
	    {
	    	*(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM_TRUE;
	    	//
	    	while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );
	    
	    	/* Check status for successful operation */
	    	status = FSMC_NAND_GetStatus();
	    }
	}    

	if(spare != RT_NULL && spare_len != 0)
	{
		if(data==RT_NULL)
		{
			/* Page write command and address */
		    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM;

		    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;  
		    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x08; 
		    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(page);  
		    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(page);
		}
	    /* Write data */
	    for(index = 0x0000; index < spare_len; index++)
	    {
	      *(vu8 *)(Bank_NAND_ADDR | DATA_AREA) = spare[index];
	    }
		
	    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM_TRUE;
	    //
	    while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );
	    
	    /* Check status for successful operation */
	    status = FSMC_NAND_GetStatus();
	}
	rt_mutex_release(&nand);
	if(NAND_READY==status)
		result=RT_EOK;
	else
		rt_kprintf("nand_mtd_write page %d, %02x %d,%02x %d failed\n",page,data,data_len,spare,spare_len);
	return result;
}

static rt_err_t nand_read_id(
		struct rt_mtd_nand_device * dev)
{
	NAND_IDTypeDef NAND_ID;
	NAND_ReadID(&NAND_ID);
    return RT_EOK;	
}

const static struct rt_mtd_nand_driver_ops nand_mtd_ops =
{
	nand_read_id,
	nand_mtd_read,
	nand_mtd_write,
	RT_NULL,
	nand_mtd_erase_block,
	nand_mtd_check_block,
	nand_mtd_mark_bad_block,
};

void k9f2808_mtd_init()
{
	/* the first partition of nand */
	nand_part.page_size = NAND_PAGE_SIZE;
	nand_part.pages_per_block = NAND_BLOCK_SIZE;//don't caculate oob size
	nand_part.block_start = 0;
	nand_part.block_end = NAND_ZONE_SIZE;
	nand_part.oob_size = 64;
	nand_part.ops = &nand_mtd_ops;
	rt_mtd_nand_register_device("nand0", &nand_part);
}
void nand_mtd_init()
{
	NAND_IDTypeDef NAND_ID;
	#define NAND_HY_MakerID    0XEC
	#define NAND_HY_DeviceID   0XF1
	NAND_Init();

	NAND_ReadID(&NAND_ID);
	rt_kprintf("\nNand Flash ID:0x%02x 0x%02x 0x%02x 0x%02x \n",NAND_ID.Maker_ID,NAND_ID.Device_ID, NAND_ID.Third_ID,NAND_ID.Fourth_ID);

    /* initialize mutex */
	if (rt_mutex_init(&nand, "nand", RT_IPC_FLAG_FIFO) != RT_EOK)
	{
		rt_kprintf("init nand lock mutex failed\n");
	}

	k9f2808_mtd_init();
}
#ifdef RT_USING_FINSH
#include <finsh.h>
void nand_id()
{
	NAND_IDTypeDef NAND_ID;
	
	NAND_ReadID(&NAND_ID);
	rt_kprintf("\nNand Flash ID:0x%02x 0x%02x 0x%02x 0x%02x \n",NAND_ID.Maker_ID,NAND_ID.Device_ID, NAND_ID.Third_ID,NAND_ID.Fourth_ID);
	return ;
}
FINSH_FUNCTION_EXPORT(nand_id, get nand id. e.g: nand_id())
#endif
#endif
