/**
 ******************************************************************************
 * @file    SDIO/sdcard.c
 * @author  MCD Application Team
 * @version V3.1.2
 * @date    09/28/2009
 * @brief   This file provides all the SD Card driver firmware functions.
 ******************************************************************************
 * @copy
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
 */

/* Includes ------------------------------------------------------------------*/
#include "sdcard.h"
//#include "stm32f10x_dma.h"
//#include "stm32f10x_sdio.h"
#include "stdbool.h"
#include <rtthread.h>
#include <dfs_fs.h>

/* set sector size to 512 */
#define SECTOR_SIZE		512

static struct rt_device sdcard_device;
static SD_CardInfo SDCardInfo;
static struct dfs_partition part;
static struct rt_semaphore sd_lock;
static rt_uint8_t _sdcard_buffer[SECTOR_SIZE];
extern rt_uint8_t  SD_Type;
extern rt_uint16_t SD_Size;
/* RT-Thread Device Driver Interface */
static rt_err_t rt_sdcard_init(rt_device_t dev)
{
	  return RT_EOK;
}

static rt_err_t rt_sdcard_open(rt_device_t dev, rt_uint16_t oflag)
{
	  return RT_EOK;
}

static rt_err_t rt_sdcard_close(rt_device_t dev)
{
	  return RT_EOK;
}

static rt_size_t rt_sdcard_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	  SD_Error status;
	  rt_uint32_t retry;

	  retry = 3;
	  while(retry)
	  {
			/* read all sectors */
			if (((rt_uint32_t)buffer % 4 != 0) ||
						((rt_uint32_t)buffer > 0x20080000))
			{
				  rt_uint32_t index;

				  /* which is not alignment with 4 or chip SRAM */
				  for (index = 0; index < size; index ++)
				  {
						SD_ReadDisk((rt_uint8_t *)_sdcard_buffer,(part.offset + index + pos),1);
						if (status != 0) break;

						/* copy to the buffer */
						rt_memcpy(((rt_uint8_t*)buffer + index * SECTOR_SIZE), _sdcard_buffer, SECTOR_SIZE);
				  }
			}
			else
			{
				  if (size == 1)
				  {
						status= SD_ReadDisk((rt_uint8_t *)buffer,(part.offset + pos),1);
				  }
				  else
				  {
						status= SD_ReadDisk((rt_uint8_t *)buffer,(part.offset + pos),size);
				  }
			}

			if (status == 0) break;

			retry --;
	  }

	  if (status == 0) return size;
	  rt_kprintf("read failed: %d, buffer 0x%08x\n", status, buffer);
	  return 0;
}

static rt_size_t rt_sdcard_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
	  return RT_EOK;
}

static rt_err_t rt_sdcard_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
	  RT_ASSERT(dev != RT_NULL);

	  if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
	  {
			struct rt_device_blk_geometry *geometry;

			geometry = (struct rt_device_blk_geometry *)args;
			if (geometry == RT_NULL) return -RT_ERROR;

			geometry->bytes_per_sector = 512;
			geometry->block_size = SD_GetSectorCount();//SDCardInfo.CardBlockSize;
			SDCardInfo.CardBlockSize=geometry->block_size;
			SDCardInfo.SD_csd.DeviceSize=SD_Size;
			if (SD_Type == SD_TYPE_V2HC/*SDIO_HIGH_CAPACITY_SD_CARD*/)
				  geometry->sector_count = (SDCardInfo.SD_csd.DeviceSize + 1)  * 1024;
			else
				  geometry->sector_count = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
	  }

	  return RT_EOK;
}

int rt_hw_sdcard_init(void)
{
	  /* SDIO POWER */
	  GPIO_InitTypeDef GPIO_InitStructure;
	  if(SD_Initialize()!=0)
	  {
			rt_kprintf("sd init error ,please check\r\n");
			return 0;
	  }
	  rt_uint8_t *sector;
	  //u8 *buf;
	  rt_uint16_t i;
	  sector=(rt_uint8_t *)rt_malloc(512);				//申请内存
	  if(SD_ReadDisk(sector,0,1)==0)	//读取0扇区的内容
	  {	
			/* get the first partition */
			if (dfs_filesystem_get_partition(&part, sector, 0) != 0)
			{
				  /* there is no partition */
				  part.offset = 0;
				  part.size   = 0;
				  rt_kprintf("sd init failed 2\r\n");
				  GPIO_SetBits(GPIOC,GPIO_Pin_6);
				  return RT_ERROR;
			}else
			{
				  rt_kprintf("part info %x_%x_%x\r\n",part.size,part.type,part.offset);
			}
	  }
	  else
	  {
			/* there is no partition table */
			part.offset = 0;
			part.size   = 0;
			rt_kprintf("sd init failed 1\r\n");
			GPIO_SetBits(GPIOC,GPIO_Pin_6);
			return RT_ERROR;
	  }

	  /* release sector buffer */
	  rt_free(sector);
	  /* register sdcard device */
	  sdcard_device.type  = RT_Device_Class_Block;
	  sdcard_device.init 	= rt_sdcard_init;
	  sdcard_device.open 	= rt_sdcard_open;
	  sdcard_device.close = rt_sdcard_close;
	  sdcard_device.read 	= rt_sdcard_read;
	  sdcard_device.write = rt_sdcard_write;
	  sdcard_device.control = rt_sdcard_control;

	  /* no private */
	  sdcard_device.user_data = &SDCardInfo;

	  rt_device_register(&sdcard_device, "sd0",
				  RT_DEVICE_FLAG_RDONLY | RT_DEVICE_FLAG_REMOVABLE | RT_DEVICE_FLAG_STANDALONE);

	  return 0;
}
INIT_DEVICE_EXPORT(rt_hw_sdcard_init);
