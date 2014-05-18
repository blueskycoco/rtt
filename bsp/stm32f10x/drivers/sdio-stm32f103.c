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
rt_uint32_t CmdResp4Error(uint8_t cmd)
{
    SD_Error errorstatus = SD_OK;
    uint32_t status;
    uint32_t response_r1;
    uint32_t response_r2;

    status = SDIO->STA;
    //rt_kprintf("1 status=%x\r\n",status);
    while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
    {    
        status = SDIO->STA;
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
	 rt_uint16_t rca,num_fn;
	 SD_Error errorstatus = SD_OK;
	 uint32_t response = 0, count = 0, validvoltage = 0;
	 uint32_t SDType = SD_STD_CAPACITY;
	
	 SD_LowLevel_Init();
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
	num_fn=(response&0x70000000)>>27;
	/* CMD3:SD_CMD_SET_REL_ADDR */
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;
	SDIO_CmdInitStructure.SDIO_Argument = 0;
	SDIO_SendCommand(&SDIO_CmdInitStructure);
	 response=CmdResp4Error(SD_CMD_SET_REL_ADDR);
	 rt_kprintf("3 respond %x\r\n",response);
	 if((response&0xe000)!=0)
	 	rt_kprintf("get rca failed\r\n");
	 else
	 	rca=(response&0xffff0000)>>16;
	 rt_kprintf("rca %d,num_fn %d\r\n",rca,num_fn);
	 SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEL_DESEL_CARD;
	SDIO_CmdInitStructure.SDIO_Argument = rca<<16;
	SDIO_SendCommand(&SDIO_CmdInitStructure);
	 response=CmdResp4Error(SD_CMD_SEL_DESEL_CARD);
	 rt_kprintf("4 respond %x\r\n",response);
	SDIO_SendCommand(&SDIO_CmdInitStructure);
	 response=CmdResp4Error(SD_CMD_SEL_DESEL_CARD);
	 rt_kprintf("5 respond %x\r\n",response);

	 return SD_OK;
}
