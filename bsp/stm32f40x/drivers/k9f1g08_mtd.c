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
 * 2012-12-11	 bbstr	  add sst39vf10601 1.2Mbyte managed by uffs together
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
typedef struct 
{
  u16 Zone;
  u16 Block;
  u16 Page;
} NAND_ADDRESS;

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

// Nand Flash HY27UF081G2A 等等
#define NAND_PAGE_SIZE             ((uint16_t)0x0800) /* 2048 bytes per page w/o Spare Area */
#define NAND_BLOCK_SIZE            ((uint16_t)0x0040) /* 64 pages per block */
#define NAND_ZONE_SIZE             ((uint16_t)0x0400) /* 1024 Block per zone */
#define NAND_SPARE_AREA_SIZE       ((uint16_t)0x0040) /* last 64 bytes as spare area */
#define NAND_MAX_ZONE              ((uint16_t)0x0001) /* 1 zones of 1024 block */

#define ADDR_1st_CYCLE(ADDR)       (uint8_t)((ADDR)& 0xFF)               /* 1st addressing cycle */
#define ADDR_2nd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF00) >> 8)      /* 2nd addressing cycle */
#define ADDR_3rd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF0000) >> 16)   /* 3rd addressing cycle */
#define ADDR_4th_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF000000) >> 24) /* 4th addressing cycle */  

#define ROW_ADDRESS (Address.Page + (Address.Block + (Address.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE)
void NAND_Init(void);
uint32_t NAND_Reset(void);
void NAND_ReadID(NAND_IDTypeDef* NAND_ID);
void Fill_Buffer(uint8_t *pBuffer, u16 BufferLenght, uint32_t Offset);
uint32_t FSMC_NAND_EraseBlock(NAND_ADDRESS Address);
uint32_t FSMC_NAND_GetStatus(void);
uint32_t  FSMC_NAND_ReadStatus(void);
uint32_t FSMC_NAND_WriteSmallPage(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumPageToWrite);
uint32_t FSMC_NAND_AddressIncrement(NAND_ADDRESS* Address);
uint32_t FSMC_NAND_ReadSmallPage(uint8_t *pBuffer, NAND_ADDRESS Address, uint8_t NumPageToRead);



/*******管脚配置  FSMC配置****************************************************/
void NAND_Init()
{
 GPIO_InitTypeDef GPIO_InitStructure; 
  FSMC_NAND_PCCARDTimingInitTypeDef  p;
  FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
  
  /*FSMC总线使用的GPIO组时钟使能*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOE |
                         RCC_AHB1Periph_GPIOF, ENABLE);

  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 
  
 /*FSMC CLE, ALE, D0->D3, NOE, NWE and NCE2初始化，推挽复用输出*/
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
//  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5  | 
                                GPIO_Pin_7  | GPIO_Pin_11 |
                                GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* GPIOE configuration */

  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 |GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 ;

  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* GPIOG configuration */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6   ;      

  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /*--------------FSMC 总线 存储器参数配置------------------------------*/
  p.FSMC_SetupTime = 0x0;         //建立时间
  p.FSMC_WaitSetupTime = 0x4;     //等待时间
  p.FSMC_HoldSetupTime = 0x5;     //保持时间
  p.FSMC_HiZSetupTime = 0x0;      //高阻建立时间

  FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank2_NAND; //使用FSMC BANK2
  FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Enable; //使能FSMC的等待功能
  FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b; //NAND Flash的数据宽度为8位
  FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Enable;                  //使能ECC特性
  FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_2048Bytes; //ECC页大小2048
  FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0x00;             
  FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x00;
  FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;
  FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;

  FSMC_NANDInit(&FSMC_NANDInitStructure);

  /*!使能FSMC BANK2 */
  FSMC_NANDCmd(FSMC_Bank2_NAND, ENABLE);
}
/*******************复位**********************************************/
uint32_t NAND_Reset(void)
{
  *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_RESET;

  return (NAND_READY);
}
/******************读取ID****************************************/
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
/**********************装载数据***********************************/
void Fill_Buffer(uint8_t *pBuffer, u16 BufferLenght,uint32_t Offset)
{
  u16 IndexTmp = 0;

  /* Put in global buffer same values */
  for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
  {
    pBuffer[IndexTmp] = IndexTmp + Offset;
  }
}
/**********************块擦出*****************************/
uint32_t FSMC_NAND_EraseBlock(NAND_ADDRESS Address)
{
  *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_ERASE0;
  
  *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);  
  *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);  
  *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);
  		
  *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_ERASE1; 

    while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );
  
    return (FSMC_NAND_GetStatus());
}
uint32_t FSMC_NAND_GetStatus(void)
{
  uint32_t timeout = 0x1000000, status = NAND_READY;

  status = FSMC_NAND_ReadStatus(); 

  /* Wait for a NAND operation to complete or a TIMEOUT to occur */
  while ((status != NAND_READY) &&( timeout != 0x00))
  {
     status = FSMC_NAND_ReadStatus();
     timeout --;      
  }

  if(timeout == 0x00)
  {          
    status =  NAND_TIMEOUT_ERROR;      
  } 

  /* Return the operation status */
  return (status);      
}
uint32_t  FSMC_NAND_ReadStatus(void)
{
  uint32_t data = 0x00, status = NAND_BUSY;

  /* Read status operation ------------------------------------ */
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
uint32_t FSMC_NAND_WriteSmallPage(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumPageToWrite)
{
   uint32_t index = 0x00, numpagewritten = 0x00, addressstatus = NAND_VALID_ADDRESS;
   uint32_t status = NAND_READY, size = 0x00;

  while((NumPageToWrite != 0x00) && (addressstatus == NAND_VALID_ADDRESS) && (status == NAND_READY))
  {
    /* Page write command and address */
    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM;

    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;  
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0X00; 
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);  
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS); 
  	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS); 

    /* Calculate the size */
    size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpagewritten);

    /* Write data */
    for(; index < size; index++)
    {
      *(vu8 *)(Bank_NAND_ADDR | DATA_AREA) = pBuffer[index];
    }
    
    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM_TRUE;
    //
    while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );
    
    /* Check status for successful operation */
    status = FSMC_NAND_GetStatus();
     
    if(status == NAND_READY)
    {
      numpagewritten++;
      NumPageToWrite--;
      /* Calculate Next small page Address */
      addressstatus = FSMC_NAND_AddressIncrement(&Address);    
    }    
  }
  
  return (status | addressstatus);
}
uint32_t FSMC_NAND_ReadSmallPage(uint8_t *pBuffer1, NAND_ADDRESS Address,uint8_t NumPageToRead)
{
  uint32_t index = 0x0000, addressstatus = NAND_VALID_ADDRESS;
  uint32_t status = NAND_READY;
	uint32_t size = 0;
	
	uint8_t numpageread = 0x00;

// while((NumPageToRead != 0x0) && (addressstatus == NAND_VALID_ADDRESS))
  {	   
 //  /* Page Read command and page address 
    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_READ_1; 
   
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; 
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = 0X00; 
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);  
    *(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);  
	*(vu8 *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);     

    *(vu8 *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_READ_TRUE; 

   while( GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0 );    
  //  /* Calculate the size 
    size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpageread);

 //   /* Get Data into Buffer    
    for(; index < size; index++)
    {
      pBuffer1[index]= *(vu8 *)(Bank_NAND_ADDR);
    }
    
    numpageread++;
    
    NumPageToRead--;

  //  /* Calculate page address            			 
    addressstatus = FSMC_NAND_AddressIncrement(&Address);
 }

 status = FSMC_NAND_GetStatus();
 	 
  return (status | addressstatus);
}
uint32_t FSMC_NAND_AddressIncrement(NAND_ADDRESS* Address)
{
 uint32_t status = NAND_VALID_ADDRESS;
 
  Address->Page++;

  if(Address->Page == NAND_BLOCK_SIZE)
  {
    Address->Page = 0;
    Address->Block++;
    
    if(Address->Block == NAND_ZONE_SIZE)
    {
      Address->Block = 0;
      Address->Zone++;

      if(Address->Zone == NAND_MAX_ZONE)
      {
        status = NAND_INVALID_ADDRESS;
      }
    }
  } 
  
  return (status);
}

#define NAND_END_BLOCK 1025

/* configurations */
#define PAGE_DATA_SIZE                  512
#define BLOCK_MARK_SPARE_OFFSET         4
static struct rt_mutex nand;
#if 0
/* nand flash commands. This appears to be generic across all NAND flash chips */
#define CMD_READ			0x00	//  Read
#define CMD_READ1			0x01	//  Read1
#define CMD_READ2			0x50	//  Read2
#define CMD_READID			0x90	//  ReadID
#define CMD_WRITE1			0x80	//  Write phase 1
#define CMD_WRITE2			0x10	//  Write phase 1
#define CMD_ERASE1			0x60	//  Erase phase 1
#define CMD_ERASE2			0xd0	//  Erase phase 1
#define CMD_STATUS			0x70	//  Status read
#define CMD_RESET			0xff	//  Reset

#define NF_CMD(cmd)			{*(volatile rt_uint8_t *)0x02000002 = (cmd); }
#define NF_ADDR(addr)		{*(volatile rt_uint8_t *)0x02000004 = (addr); }	
#define NF_CE_L()			{PDATC = PDATC & ~(1<<9) ; }
#define NF_CE_H()			{PDATC = PDATC | (1<<9) ; }
#define NF_RDDATA()			(*(volatile rt_uint8_t *)0x02000000)
#define NF_RDDATA8()		(*(volatile rt_uint8_t *)0x02000000)
#define NF_WRDATA(data)		{*(volatile rt_uint8_t *)0x02000000 = (data); }
#define NF_WRDATA8(data)	{*(volatile rt_uint8_t *)0x02000000 = (data); } 
#define NF_WAITRB()			{while(!(PDATC&(1<<8)));} 
#define STATUS_READY        0x40    // ready
#define STATUS_ERROR        0x01    // error
#define	STATUS_ILLACC       0x08    // illegal access
/*	add for nor flash 2012-12-11	*/

#ifndef RT_USING_MTD_NOR
#define NOR_START_BLOCK 10
#define NOR_SPARE_BLOCK 1982464//1990656
#define	CHECK_DELAY	150000
#define ROM_BASE 0x00000000
#define toogle_addr(r)	((r))
rt_base_t	baset;

#define inportw(r) 		(*(volatile rt_uint16_t *)(r))
#define outportw(r, d) 	(*(volatile rt_uint16_t *)(d) = r)

static void SWPIDExit(void)
{
    outportw(0x00aa, ROM_BASE+0xaaaa);
    outportw(0x0055, ROM_BASE+0x5554);
    outportw(0x00f0, ROM_BASE+0xaaaa);
}
static void SWPIDEntry(void)
{
    outportw(0x00aa, ROM_BASE+0xaaaa);
    outportw(0x0055, ROM_BASE+0x5554);
    outportw(0x0090, ROM_BASE+0xaaaa);
}
/* RT-Thread MTD device interface */
static rt_uint8_t check_toggle_ready(rt_uint32_t dst)
{
	rt_int16_t PreData,CurrData;
	rt_uint32_t TimeOut=0;

	PreData = inportw(dst);
	PreData = PreData & 0x0040;
	while(TimeOut < CHECK_DELAY)
	{
		CurrData = inportw(dst);
		CurrData = CurrData & 0x0040;
		if(CurrData == PreData)
		{
			rt_hw_interrupt_enable(baset);
			return RT_EOK;
		}
		else
		{
			PreData = CurrData;
			TimeOut++;
		}
	}
	rt_hw_interrupt_enable(baset);
	return RT_ERROR;
}
static rt_uint8_t SectorErase(rt_uint32_t sector)
{
	baset=rt_hw_interrupt_disable();
	outportw(0xaaaa, ROM_BASE+0xaaaa);
	outportw(0x5555, ROM_BASE+0x5554);
	outportw(0x8080, ROM_BASE+0xaaaa);
	outportw(0xaaaa, ROM_BASE+0xaaaa);
	outportw(0x5555, ROM_BASE+0x5554);
	outportw(0x3030, ROM_BASE+sector);	
	return check_toggle_ready(ROM_BASE+sector);
}

static int FlashProg(rt_uint32_t ProgStart, rt_uint16_t *DataPtr, rt_uint32_t WordCnt)
{	

	for( ; WordCnt; ProgStart+=2, DataPtr++, WordCnt--) {
		baset=rt_hw_interrupt_disable();
		outportw(0xaaaa, ROM_BASE+0xaaaa);
		outportw(0x5555, ROM_BASE+0x5554);
		outportw(0xa0a0, ROM_BASE+0xaaaa);
		outportw(*DataPtr, ROM_BASE+ProgStart);

		if(check_toggle_ready(ROM_BASE+ProgStart)!=RT_EOK)
		{
			return RT_ERROR;
		}

	}
	return RT_EOK;
}

static rt_err_t sst39vf_mtd_check_block(
		struct rt_mtd_nand_device* device,
		rt_uint32_t block)
{
	//for nor flash ,there is no bad block	
	rt_uint32_t spare_offs = ((block - NAND_END_BLOCK)/8)*4*1024 + NOR_SPARE_BLOCK;
    rt_uint32_t index = ((block-NAND_END_BLOCK)%8)*512;
	return (*(volatile rt_uint8_t *)(spare_offs+index+4)==0xff) ? RT_EOK:RT_ERROR;
}

static rt_err_t sst39vf_mtd_mark_bad_block(
		struct rt_mtd_nand_device* device,
		rt_uint32_t block)
{
	rt_uint32_t spare_offs = ((block - NAND_END_BLOCK)/8)*4*1024 + NOR_SPARE_BLOCK;
    rt_uint32_t index = ((block-NAND_END_BLOCK)%8)*512;
	rt_uint16_t mark = 0xff00;
    if(FlashProg(spare_offs+index+4,&mark,1)!=RT_EOK)
	{
	    rt_kprintf("Prog spare %x failed,offs %d to %d\n",spare_offs,index+512,4096);
	    return RT_ERROR;
	}
    
    return RT_EOK;
}

static rt_err_t sst39vf_mtd_erase_block(
		struct rt_mtd_nand_device* device,
		rt_uint32_t block)
{
	//step1 get offset of sst39vf's sector position 1 block need 4 real nor sectors to store
	rt_uint32_t block_offs=(block-NAND_END_BLOCK)*4*4*1024+NOR_START_BLOCK*64*1024;//offs of sst39vf1601, form block 10 ,then to sector address
	rt_uint32_t i;
	//step2 erase 4 sector(one sector is 4k byte, nand's 1 block = 32 page = 32*512 byte = 4 sector, so we need erase 4 sector at a time )
	for(i=0;i<4;i++)
	{
		if(SectorErase(block_offs & ~0xfff)!=RT_EOK) 
		{	//4K Bytes boudary
			rt_kprintf("erase nor block %d failed\n",block_offs/(32*512));
			return RT_ERROR;
		}
		block_offs=block_offs+4096;
	}
	//step3 update spare area at the last 10 sectors 16 bytes to 0xff
	/*read back spare data, 1 sector is enough*/
	rt_uint32_t spare_offs = ((block - NAND_END_BLOCK)/8)*4*1024 + NOR_SPARE_BLOCK;
	rt_uint8_t *spare_buf=(rt_uint8_t *)rt_malloc(4096);
	rt_memcpy(spare_buf,(rt_uint8_t *)spare_offs,4096);
	/*erase this sector*/
	if(SectorErase(spare_offs&~0xfff)==RT_EOK)
	{		
        rt_uint32_t index = ((block-NAND_END_BLOCK)%8)*512;
		rt_kprintf("Erase spare %x ,offs %x\n",spare_offs,index);
        if(index==0)
        {
	        if(FlashProg(spare_offs+512,(rt_uint16_t *)(spare_buf+512),1792)!=RT_EOK)
	        {
		        rt_kprintf("Prog spare %x failed,offs %d to %d\n",spare_offs,index+512,4096);
		        rt_free(spare_buf);
		        return RT_ERROR;
	        }
            
        }
        else if(index==3584)
        {
	        if(FlashProg(spare_offs,(rt_uint16_t *)spare_buf,1792)!=RT_EOK)
	        {
		        rt_kprintf("Prog spare %x failed,offs %d to %d\n",spare_offs,0,3584);
		        rt_free(spare_buf);
		        return RT_ERROR;
	        }

        }
        else
        {
	        if(FlashProg(spare_offs,(rt_uint16_t *)spare_buf,index/2)!=RT_EOK)
	        {
		        rt_kprintf("Prog spare %x failed,offs %d to %d\n",spare_offs,0,index);
		        rt_free(spare_buf);
		        return RT_ERROR;
	        }
            
	        if(FlashProg(spare_offs+index+512,(rt_uint16_t *)(spare_buf+index+512),(3584-index)/2)!=RT_EOK)
	        {
		        rt_kprintf("Prog spare %x failed,offs %d to %d\n",spare_offs,0,3584);
		        rt_free(spare_buf);
		        return RT_ERROR;
	        }

        }

	}
    else
	{
		rt_kprintf("erase nor block %d failed,secotr %d\n",spare_offs/(64*1024),(block-NAND_END_BLOCK)/8);
		rt_free(spare_buf);
		return RT_ERROR;
	}
	//rt_kprintf("Erase block %x,spare %x,offs %d\r\n",block_offs/65536,(block - NAND_END_BLOCK)/8,((block-NAND_END_BLOCK)%8)*512);
	rt_free(spare_buf);
	return RT_EOK;
}

static rt_err_t sst39vf_mtd_read(
		struct rt_mtd_nand_device * dev,
		rt_off_t page,
		rt_uint8_t * data, rt_uint32_t data_len, 
		rt_uint8_t * spare, rt_uint32_t spare_len)
{
	if (data != RT_NULL && data_len != 0)
	{	
	    // get offset of sst39vf's read position
		rt_uint32_t page_offs = (page-NAND_END_BLOCK*32)*512 + NOR_START_BLOCK*64*1024;
		if(data_len>512 && data_len<=528)
		{
            //read back both main data and spare data 
			rt_uint32_t spare_offs = (page-NAND_END_BLOCK*32)*16 + NOR_SPARE_BLOCK;
			rt_memcpy(data,(rt_uint8_t *)page_offs,512);
			rt_memcpy((rt_uint8_t *)(data+512),(rt_uint8_t *)spare_offs,data_len-512);
		}
		else
			rt_memcpy(data,(rt_uint8_t *)page_offs,data_len);
	}

	
	if (spare != RT_NULL && spare_len != 0)
	{
		rt_uint32_t spare_offs = (page-NAND_END_BLOCK*32)*16 + NOR_SPARE_BLOCK;
		rt_memcpy(spare,(rt_uint8_t *)spare_offs,spare_len);
	}	
	//rt_kprintf("Read block %x,sector %x ,spare %x\r\n",((page-NAND_END_BLOCK*32)*512 + NOR_START_BLOCK*64*1024)/65536,(page-NAND_END_BLOCK*32)%128,(page-NAND_END_BLOCK*32)*16 + NOR_SPARE_BLOCK);
	return RT_EOK;
}

static rt_err_t sst39vf_mtd_write (
		struct rt_mtd_nand_device * dev,
		rt_off_t page,
		const rt_uint8_t * data, rt_uint32_t data_len,
		const rt_uint8_t * spare, rt_uint32_t spare_len)
{
	if (data != RT_NULL && data_len != 0)
	{	
	    // get offset of sst39vf's write position
		rt_uint32_t page_offs = (page-NAND_END_BLOCK*32)*512 + NOR_START_BLOCK*64*1024;
		if(FlashProg(page_offs,(rt_uint16_t *)data,data_len/2)!=RT_EOK)
		{
			rt_kprintf("prog nor block %d ,page %x failed\n",page_offs/(32*512),(page-NAND_END_BLOCK*32)%32);
			return RT_ERROR;
		}
	}

	
	if (spare != RT_NULL && spare_len != 0)
	{
	    // get spare offset of sst39vf's write position
		rt_uint32_t spare_offs = (page-NAND_END_BLOCK*32)*16 + NOR_SPARE_BLOCK;
		if(FlashProg(spare_offs,(rt_uint16_t *)spare,spare_len/2)!=RT_EOK)
		{
			rt_kprintf("prog nor block spare %d ,page spare %x failed\n",spare_offs/(32*512),(page-NAND_END_BLOCK*32)%16);
			return RT_ERROR;
		}

	}
	
//	rt_kprintf("Write block %x,sector %x ,spare %x\r\n",((page-NAND_END_BLOCK*32)*512 + NOR_START_BLOCK*64*1024)/65536,(page-NAND_END_BLOCK*32)%128,(page-NAND_END_BLOCK*32)*16 + NOR_SPARE_BLOCK);
	return RT_EOK;
}

static rt_err_t sst39vf_read_id(
		struct rt_mtd_nand_device * dev)
{
    rt_uint32_t i;

    SWPIDEntry();
    i  = inportw(ROM_BASE);
    i |= inportw(ROM_BASE+2)<<16;
    SWPIDExit();
	rt_kprintf("sst39vf id %x\n",i);
    return RT_EOK;	
}
#endif
/*	add for nor flash 2012-12-11	*/
rt_uint32_t NF_DETECT_RB(void)
{
	rt_uint8_t stat;
	
	NF_CMD(CMD_STATUS);
	do {
		stat = NF_RDDATA();
		//printf("%x\n", stat);
	}while(!(stat&0x40));
	NF_CMD(CMD_READ);
	return stat&1;
}
/*
 * In a page, data's ecc code is stored in spare area, from BYTE 0 to BYTEE 3.
 * Block's status byte which indicate a block is bad or not is BYTE4.
 */
static void nand_hw_init(void)
{

	/* reset nand flash */
	NF_CE_L();
	NF_CMD(CMD_RESET);
	NF_DETECT_RB();
	NF_CE_H();
}

/*
 *check the first byte in spare of the block's first page
 *return
 * good block,  RT_EOK
 * bad  blcok, return -RT_ERROR
 */
static rt_err_t k9f2808_mtd_check_block(
		struct rt_mtd_nand_device* device,
		rt_uint32_t block)
{
	rt_uint8_t status;
	block =  block << 5;

	NF_CE_L();
	NF_CMD(CMD_READ2);
	NF_ADDR(BLOCK_MARK_SPARE_OFFSET);
	NF_ADDR(block & 0xff);
	NF_ADDR((block >> 8) & 0xff);
	NF_ADDR((block >> 16) & 0xff);

	NF_DETECT_RB();	 /* wait for ready bit */

	status = NF_RDDATA8();
	NF_CMD(CMD_READ);
	NF_CE_H();
	//rt_kprintf("k9f2808_mtd_check_block %d %x\n",block,status);
	/* TODO: more check about status */
	return status == 0xFF ? RT_EOK : -RT_ERROR;

}

static rt_err_t k9f2808_mtd_mark_bad_block(
		struct rt_mtd_nand_device* device,
		rt_uint32_t block)
{
	/* get address of the fisrt page in the block */
	rt_err_t result = RT_EOK;
	block =  block << 5;

	NF_CE_L();
	NF_CMD(CMD_READ2);
	NF_CMD(CMD_WRITE1);

	NF_ADDR(BLOCK_MARK_SPARE_OFFSET);
	NF_ADDR(block & 0xff);
	NF_ADDR((block >> 8) & 0xff);

	/* write bad block mark in spare*/
	NF_WRDATA8(0);

	NF_CMD(CMD_WRITE2);
 	NF_DETECT_RB();	     /* wait for ready bit */
	NF_CMD(CMD_STATUS);	/* get the status */

	if (NF_RDDATA() &  STATUS_ERROR)
		result = -RT_ERROR;
	NF_CMD(CMD_READ);
	NF_CE_H(); /* disable chip select */
	//rt_kprintf("k9f2808_mtd_mark_bad_block %d\n",block);
    return result;
}

static rt_err_t k9f2808_mtd_erase_block(
		struct rt_mtd_nand_device* device,
		rt_uint32_t block)
{
	/* 1 block = 64 page= 2^6*/
    	rt_err_t result = RT_EOK;
	block <<= 5; /* get the first page's address in this block*/

	NF_CE_L();  /* enable chip */
	
	NF_CMD(CMD_ERASE1);	/* erase one block 1st command */
	NF_ADDR(block & 0xff);
	NF_ADDR((block >> 8) & 0xff);
	NF_CMD(CMD_ERASE2);	

	NF_DETECT_RB(); /* wait for ready bit */

	NF_CMD(CMD_STATUS);	/* check status	*/

	if (NF_RDDATA() & STATUS_ERROR) {
		result = -RT_ERROR;
	}

	NF_CE_H();
	//rt_kprintf("k9f2808_mtd_erase_block %d\n",block);
	return result;

}

/* return 0, ecc ok, 1, can be fixed , -1 can not be fixed */
static rt_err_t k9f2808_mtd_read(
		struct rt_mtd_nand_device * dev,
		rt_off_t page,
		rt_uint8_t * data, rt_uint32_t data_len, //may not always be 2048
		rt_uint8_t * spare, rt_uint32_t spare_len)
{
	rt_uint32_t i;
 	rt_uint32_t mecc;
	rt_uint32_t status;
	rt_err_t result = RT_EOK;
	
	NF_CE_L();
	if (data != RT_NULL && data_len != 0)
	{
		/* read page data area */
		NF_CE_L();

		NF_CMD(CMD_READ);
		NF_ADDR(0);
		NF_ADDR((page) & 0xff);
		NF_ADDR((page >> 8) & 0xff);

		NF_DETECT_RB();/* wait for ready bit */

		/*TODO: use a more quick method */
		for (i = 0; i < data_len; i++)
			data[i] = NF_RDDATA8();

	}

	if (spare != RT_NULL && spare_len != 0)
	{
		/* read page spare area */

		NF_CMD(CMD_READ2);
		NF_ADDR(0);
		NF_ADDR((page) & 0xff);
		NF_ADDR((page >> 8) & 0xff);
		//NF_ADDR((page >> 16) & 0xff);

		NF_DETECT_RB();/* wait for ready bit */
		/*TODO: use a more quick method */
		for (i = 0; i < spare_len; i++)
			spare[i] = NF_RDDATA8();

		//NF_CMD(CMD_READ);
		result = RT_EOK;
	}
	NF_CE_H();
	//for(i=0;i<spare_len;i++)
	//rt_kprintf("k9f2808_mtd_read %d\n",spare[i]);
	/* TODO: more check about status */
	return result;
}

static rt_err_t k9f2808_mtd_write (
		struct rt_mtd_nand_device * dev,
		rt_off_t page,
		const rt_uint8_t * data, rt_uint32_t data_len,//will be 2048 always!
		const rt_uint8_t * spare, rt_uint32_t spare_len)
{
	rt_uint32_t i;
	rt_uint32_t mecc0;
	rt_err_t result = RT_EOK;

	NF_CE_L();       /* enable chip */
	if (data != RT_NULL && data_len != 0)
	{
		RT_ASSERT(data_len == PAGE_DATA_SIZE);
		NF_CMD(CMD_WRITE1);

		NF_ADDR(0);
		NF_ADDR( (page) & 0xff);
		NF_ADDR((page >> 8) & 0xff);

		for(i=0; i<PAGE_DATA_SIZE; i++)
			NF_WRDATA8(data[i]);


		NF_CMD(CMD_WRITE2);
		NF_DETECT_RB();	 	/* wait for ready bit */
	}

	if (spare != RT_NULL && spare_len != 0)
	{
		NF_CMD(CMD_READ2);
		NF_CMD(CMD_WRITE1);

		NF_ADDR(0);
		NF_ADDR( (page )& 0xff);
		NF_ADDR((page >> 8) & 0xff);

		for(i=0; i<spare_len; i++)
			NF_WRDATA8(spare[i]);

		NF_CMD(CMD_WRITE2);
		//NF_CMD(CMD_READ);
		NF_DETECT_RB();
	}

__ret:
	NF_CE_H(); /* disable chip */
	//for(i=0;i<spare_len;i++)
	//rt_kprintf("k9f2808_mtd_write %d\n",spare[i]);
	return result;
}

static rt_err_t k9f2808_read_id(
		struct rt_mtd_nand_device * dev)
{
	rt_uint32_t id;

	NF_CE_L();
	NF_CMD(CMD_READID);
	NF_ADDR(0);
	NF_WAITRB();
	id  = NF_RDDATA8()<<8;
	id |= NF_RDDATA8();
	NF_CE_H();
	rt_kprintf("K9F2808 ID %x\n",id);
	return RT_EOK;
}
#endif
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

	if(/*(NAND_READY==FSMC_NAND_GetStatus()) && (*/status == 0xff)
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
	uint32_t size = 0;
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

/* interface of nand and rt-thread device */
static struct rt_mtd_nand_device nand_part;

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
	rt_uint8_t spare[64],spare1[64],*data,*data1;
	int i;
	NAND_Init();

	NAND_ReadID(&NAND_ID);
	rt_kprintf("\nNand Flash ID:0x%02x 0x%02x 0x%02x 0x%02x \n",NAND_ID.Maker_ID,NAND_ID.Device_ID, NAND_ID.Third_ID,NAND_ID.Fourth_ID);

    /* initialize mutex */
	if (rt_mutex_init(&nand, "nand", RT_IPC_FLAG_FIFO) != RT_EOK)
	{
		rt_kprintf("init nand lock mutex failed\n");
	}

	k9f2808_mtd_init();

	#if 0
	data=(rt_uint8_t *)rt_malloc(2048);
	data1=(rt_uint8_t *)rt_malloc(2048);
	rt_memset(spare1,0,64);
	if(data1!=RT_NULL)
	rt_memset(data1,0,2048);
	rt_kprintf("==Data>\n");
	for(i=0;i<2048;i++)
	{
		data[i]=255-i%255;
		rt_kprintf("%02x ",data[i]);
	}	
	rt_kprintf("\n==Spare>\n");
	for(i=0;i<64;i++)
	{
		spare[i]=i;
		rt_kprintf("%d ",spare[i]);
	}
	//NAND_Reset();
	nand_mtd_erase_block(RT_NULL,0);
	nand_mtd_write(RT_NULL,0,data,2048,spare,64);
	nand_mtd_read(RT_NULL,0,data1,2048,spare1,64);
	rt_kprintf("\n<Data==\n");
	for(i=0;i<2048;i++)
	{
		rt_kprintf("%02x ",data1[i]);
	}
	rt_kprintf("\n<Spare==\n");
	for(i=0;i<64;i++)
	{
		rt_kprintf("%d ",spare1[i]);
	}
	#endif
}
#if 0
#include "finsh.h"
static char buf[PAGE_DATA_SIZE+16];
static char buf1[PAGE_DATA_SIZE+16];
static char spare[16];
static char spare1[16];
static int flag;
void nand_erase(int start, int end)
{
	int page;
	flag=1;
	rt_memset(buf, 0, PAGE_DATA_SIZE);
	rt_memset(spare, 0, 16);
	for(; start <= end; start ++)
	{
		page = start * 32;
		nand_mtd_erase_block(RT_NULL, start);
		nand_mtd_read(RT_NULL, page, buf, PAGE_DATA_SIZE, spare, 16);
		if (spare[0] != 0xFF)
		{
			rt_kprintf("block %d is bad, mark it bad\n", start);
			if (spare[4] == 0xFF)
			{
				spare[4] = 0x00;
				nand_mtd_write(RT_NULL, page, RT_NULL, 0, spare, 16);
			}
		}
	}
}

int nand_read(int start,int end)
{
	int i,page,j;
	int res;
	rt_memset(buf, 0, sizeof(buf));
	for(; start <= end; start ++)
	{
        for(j=0;j<32;j++)
        {
			page = start * 32+j;
			res = nand_mtd_read(RT_NULL, page, buf, PAGE_DATA_SIZE+16, RT_NULL, 0);
			for(i=0; i<PAGE_DATA_SIZE; i++)
			{
				if(flag==0)
				{
					if(buf[i]!=buf1[i])
					rt_kprintf("nand_read block %d ,page %d ,buf[%2d] %d != %d	\n",page/32,page%32,i,buf[i],buf1[i]);
				}else
				{
					if(buf[i]!=0xff)
					rt_kprintf("nand_read block %d ,page %d ,i %d is not correct\n",page/32,page%32,i);			
				}
			}

			for(i=0; i<16; i++)
			{
				if(flag==0)
				{
					if(buf[512+i]!=spare1[i])
					rt_kprintf("nand_read block %d ,page %d ,spare[%2d] %d!=%d n",page/32,page%32,i,buf[512+i],spare1[i]);
				}else
				{
					if(buf[512+i]!=0xff)
					rt_kprintf("nand_read block %d ,page %d ,spare %d is not correct\n",page/32,page%32,i);			
				}
			
		    }
        }
	}
	return res;
}
int nand_write(int start,int end)
{
	int i;
	rt_err_t result=RT_EOK;
	rt_memset(buf, 0, PAGE_DATA_SIZE);
	rt_memset(spare, 0, 16);
	for(i=0; i<PAGE_DATA_SIZE; i++)
	{
		buf[i] = (i % 2) + i / 2;
		buf1[i] = buf[i];
	}
	for(i=0;i<16;i++)
	{
		spare[i]=i;
		spare1[i]=i;
	}
	flag=0;
	for(;start<=end;start++)		
	  for(i=0;i<32;i++)
		{
			result = nand_mtd_write(RT_NULL, start*32+i, buf, PAGE_DATA_SIZE, spare, 16);
			if(result!=RT_EOK)
			{
				rt_kprintf("nand_mtd_write block %d,page %d filed\n",start,start*32+i);
			}
	  	}
	return	result;
}

int nand_read2(int start,int end)
{
	int i,page,j;
	int res;
	rt_memset(buf, 0, sizeof(buf));		
	rt_memset(spare, 0, 16);
	for(; start <= end; start ++)
	{
        for(j=0;j<32;j++)
        {
			page = start * 32+j;
			res = nand_mtd_read(RT_NULL, page, buf, PAGE_DATA_SIZE, RT_NULL, 0);
			for(i=0; i<PAGE_DATA_SIZE; i++)
			{
				if(flag==0)
				{
					if(buf[i]!=buf1[i])
					rt_kprintf("nand_read2 block %d ,page %d ,buf[%2d] %d != %d \n",page/32,page%32,i,buf[i],buf1[i]);
				}else
				{
					if(buf[i]!=0xff)
					rt_kprintf("nand_read2 block %d ,page %d ,i %d is not correct\n",page/32,page%32,i);			
				}
			}


			res = nand_mtd_read(RT_NULL, page, RT_NULL, 0, spare, 16);
			for(i=0; i<16; i++)
			{
				if(flag==0)
				{
					if(spare[i]!=spare1[i])
					rt_kprintf("nand_read2 block %d ,page %d ,spare[%2d] %d != %d \n",page/32,page%32,i,spare[i],spare1[i]);
				}else
				{
					if(spare[i]!=0xff)
					rt_kprintf("nand_read2 block %d ,page %d ,spare %d is not correct\n",page/32,page%32,i);			
				}
			}
        }
	}
	return res;
}
int nand_read3(int start, int end)
{
	int i,page,j;
	int res;
	rt_memset(buf, 0, sizeof(buf));
	rt_memset(spare, 0, 16);
	for(; start <= end; start ++)
	{
        for(j=0;j<32;j++)
        {
			page = start * 32+j;
			res = nand_mtd_read(RT_NULL, page, buf, PAGE_DATA_SIZE, spare, 16);
			for(i=0; i<PAGE_DATA_SIZE; i++)
			{
				if(flag==0)
				{
					if(buf[i]!=buf1[i])
					rt_kprintf("nand_read3 block %d ,page %d ,buf[%2d] %d != %d\n",page/32,page%32,i,buf[i],buf1[i]);
				}else
				{
					if(buf[i]!=0xff)
					rt_kprintf("nand_read3 block %d ,page %d ,i %d is not correct\n",page/32,page%32,i);			
				}
			}

			for(i=0; i<16; i++)
			{
				if(flag==0)
				{
					if(spare[i]!=spare1[i])
					rt_kprintf("nand_read3 block %d ,page %d ,spare[%2d] %d !=%d \n",page/32,page%32,i,spare[i],spare1[i]);
				}else
				{
					if(spare[i]!=0xff)
					rt_kprintf("nand_read3 block %d ,page %d ,spare %d is not correct\n",page/32,page%32,i);			
				}
			}
        }
	
	}
	return res;
}
void nand_read_total(int start, int end)
{

	nand_read(start,end);
	nand_read2(start,end);
	nand_read3(start,end);
}
int nand_check(int start, int end)
{
	for(; start <= end; start ++)
	{
		if ( nand_mtd_check_block(RT_NULL, start) != RT_EOK)
			rt_kprintf("block %d is bad\n", start);
	}
}

int nand_mark(int start,int end)
{
	rt_err_t result;
	for(; start <= end; start ++)
	{
		result = nand_mtd_mark_bad_block(RT_NULL, start);
		if(result !=RT_EOK)
			rt_kprintf("nand_mark %d block failed",start);
	}
}
void nand_id(void)
{
	nand_read_id(NULL);
}
void nand_test_erase(void)
{
	nand_erase(0,1105);
	rt_kprintf("total block are erased \n");
	nand_read_total(0,1105);
	rt_kprintf("total block are readed \n");
}
void nand_test_write(void)
{
	nand_erase(0,1105);
	rt_kprintf("total block are erased \n");
	nand_write(0,1105);
	rt_kprintf("total block are writed \n");
	nand_read_total(0,1105);
	rt_kprintf("total block are readed \n");
}
FINSH_FUNCTION_EXPORT(nand_id, nand_read_id);
FINSH_FUNCTION_EXPORT(nand_read, nand_read(0,1105).);
FINSH_FUNCTION_EXPORT(nand_read2, nand_read2(0,1105).);
FINSH_FUNCTION_EXPORT(nand_read3, nand_read3(0,1105).);
FINSH_FUNCTION_EXPORT(nand_read_total, nand_read_total(0,1105).);
FINSH_FUNCTION_EXPORT(nand_write, nand_write(0,1105).);
FINSH_FUNCTION_EXPORT(nand_check, nand_check(0,1105).);
FINSH_FUNCTION_EXPORT(nand_mark, nand_mark(0,1105).);
FINSH_FUNCTION_EXPORT(nand_test_erase, nand_test_erase().);
FINSH_FUNCTION_EXPORT(nand_test_write, nand_test_write().);
FINSH_FUNCTION_EXPORT(nand_erase, nand_erase(0, 1105). erase block in nand);
#endif
#endif
