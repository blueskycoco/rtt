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
#include "board.h"
#ifdef RT_USING_MTD_NAND

/* configurations */
#define PAGE_DATA_SIZE                  512
static struct rt_mutex nand;
/*	add for nor flash 2012-12-11	*/
#define NOR_SPARE_BLOCK 124*32*512//1990656

static rt_err_t nand_mtd_check_block(
		struct rt_mtd_nand_device* device,
		rt_uint32_t block)
{
	rt_err_t result=RT_EOK;	
	rt_uint16_t bad;
	rt_uint32_t spare_offs = NOR_SPARE_BLOCK+block*32;
	rt_mutex_take(&nand, RT_WAITING_FOREVER);		
	Mem_Rd(&bad,spare_offs);
	rt_mutex_release(&nand);
	return ((bad&0xff)==0xff) ? RT_EOK:RT_ERROR;
}

static rt_err_t nand_mtd_mark_bad_block(
		struct rt_mtd_nand_device* device,
		rt_uint32_t block)
{
	rt_err_t result=RT_EOK;	
	rt_uint16_t data=0xff00;
	rt_uint32_t spare_offs = NOR_SPARE_BLOCK+block*32;
	rt_mutex_take(&nand, RT_WAITING_FOREVER);		
	//write spare_offs to 0xff00
	Mem_Wr(data,spare_offs);
	rt_mutex_release(&nand);
	return result;
}

static rt_err_t nand_mtd_erase_block(
		struct rt_mtd_nand_device* device,
		rt_uint32_t block)
{
	rt_err_t result=RT_EOK;	
	int i;
	rt_uint16_t data=0xffff;
	rt_uint32_t block_offs=block*32*512;
	rt_uint32_t spare_offs = NOR_SPARE_BLOCK+block*32;
	rt_mutex_take(&nand, RT_WAITING_FOREVER);		
	//erase block offs , len=one block size /2 , 16bit wr
	for(i=0;i<(32*512)/2;i++)
		Mem_Wr(data,block_offs+i);
	//erase spare offs, len=one block spare size /2 , 16bit wr
	for(i=0;i<(32*16)/2;i++)
		Mem_Wr(data,spare_offs+i);	
	rt_mutex_release(&nand);
	return result;
}

static rt_err_t nand_mtd_read(
		struct rt_mtd_nand_device * dev,
		rt_off_t page,
		rt_uint8_t * data, rt_uint32_t data_len, //may not always be 2048
		rt_uint8_t * spare, rt_uint32_t spare_len)
{
	rt_err_t result=RT_EOK;
	int i;
	rt_mutex_take(&nand, RT_WAITING_FOREVER);
	if (data != RT_NULL && data_len != 0)
	{	
	    // get offset of sst39vf's read position
	    	
		rt_uint32_t page_offs = page*512;
		if(data_len>512 && data_len<=528)
		{
            	//read back both main data and spare data 
			rt_uint32_t spare_offs = page*16 + NOR_SPARE_BLOCK;
			for(i=0;i<256;i++)
				Mem_Rd((rt_uint16_t *)data+i,page_offs+i);
			for(i=0;i<(data_len-512)/2;i++)
				Mem_Rd((rt_uint16_t *)data+256+i,spare_offs+i);
		}
		else
		{
			for(i=0;i<data_len/2;i++)
				Mem_Rd((rt_uint16_t *)data+i,page_offs+i);
		}
	}

	
	if (spare != RT_NULL && spare_len != 0)
	{
		rt_uint32_t spare_offs = page*16 + NOR_SPARE_BLOCK;
		for(i=0;i<spare_len/2;i++)
			Mem_Rd((rt_uint16_t *)spare+i,spare_offs+i);
	}	
	rt_mutex_release(&nand);

	return result;
}

static rt_err_t nand_mtd_write (
		struct rt_mtd_nand_device * dev,
		rt_off_t page,
		const rt_uint8_t * data, rt_uint32_t data_len,//will be 2048 always!
		const rt_uint8_t * spare, rt_uint32_t spare_len)
{	
	rt_err_t result=RT_EOK;
	int i;
	rt_mutex_take(&nand, RT_WAITING_FOREVER);
	if (data != RT_NULL && data_len != 0)
	{	
	    // get offset of sst39vf's write position
		rt_uint32_t page_offs = page*512;
		for(i=0;i<data_len/2;i++)
			Mem_Wr(*(rt_uint16_t *)((rt_uint16_t *)data+i),page_offs+i);
	}

	
	if (spare != RT_NULL && spare_len != 0)
	{
	    // get spare offset of sst39vf's write position
		rt_uint32_t spare_offs = page*16 + NOR_SPARE_BLOCK;
	    	for(i=0;i<spare_len/2;i++)
		    Mem_Wr(*(rt_uint16_t *)((rt_uint16_t *)spare+i),spare_offs+i);

	}
	
	rt_mutex_release(&nand);

	return result;
}

static rt_err_t nand_read_id(
		struct rt_mtd_nand_device * dev)
{
	rt_kprintf("psram read id ok\r\n");
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
static struct rt_mtd_nand_device nand_part[2];

void nand_mtd_init()
{

    /* initialize mutex */
	if (rt_mutex_init(&nand, "nand", RT_IPC_FLAG_FIFO) != RT_EOK)
	{
		rt_kprintf("init nand lock mutex failed\n");
	}


	nand_part[0].page_size = PAGE_DATA_SIZE;
	nand_part[0].pages_per_block = 32;//don't caculate oob size
	nand_part[0].block_start = 0;
	nand_part[0].block_end = 123;
	nand_part[0].oob_size = 16;
	nand_part[0].ops = &nand_mtd_ops;
	rt_mtd_nand_register_device("psram0", &nand_part[0]);

	nand_read_id(RT_NULL);

}
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

