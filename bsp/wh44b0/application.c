/*
 * File      : app.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-10-08     Bernard      the first version
 */

/**
 * @addtogroup wh44b0
 */
/** @{ */
#include <rtthread.h>
#ifdef RT_USING_LWIP
#include <netif/ethernetif.h>
#include "rtl8019.h"
#endif
#include "s1.h"
#include <dfs_posix.h>
void rt_led_thread_entry(void *parameter)
{
		/* LwIP Initialization */
#ifdef RT_USING_LWIP
		{
			extern void lwip_sys_init(void);
			eth_system_device_init();
	
			/* register ethernetif device */
			rt_hw_rtl8019_init();
	
			/* re-init device driver */
			rt_device_init_all();
			/* init lwip system */
			lwip_sys_init();
			rt_kprintf("TCP/IP initialized!\n");
			netio_init();
		}
#endif
	nfs_init();
	nfs("192.168.0.100:/test");
	ftpd_start();
	chargen();
	while (1)
	{
		/* light on leds for one second */
		//rt_kprintf("Led on rom\n");
		rt_hw_led_set(0xff);
		rt_thread_delay(100);
		//rt_kprintf("Led off rom\n");
		/* light off leds for one second */
		rt_hw_led_set(0x00);
		rt_thread_delay(100);
	}
}

/* application start function */
int rt_application_init()
{
	rt_thread_t led_thread;
	led_thread = rt_thread_create("led",
								rt_led_thread_entry, RT_NULL,
								512, 20, 20);
	if (led_thread != RT_NULL)
		rt_thread_startup(led_thread);
	return 0;	/* empty */
}
#include "finsh.h"
void nfs(const char *folder)
{
	#if defined(RT_USING_DFS) && defined(RT_USING_LWIP) && defined(RT_USING_DFS_NFS)
			/* NFSv3 Initialization */
			//rt_uint8_t path[256];
			//strcpy(path,"192.168.1.102:/");
			//strcat(path,folder);
			rt_kprintf("to mount %s\n",folder);
			if (dfs_mount(RT_NULL, "/nfs", "nfs", 0, folder) == 0)
				rt_kprintf("NFSv3 File System initialized!\n");
			else
			{
				if(mkdir("/nfs",0777)==RT_EOK)
				{
					if (dfs_mount(RT_NULL, "/nfs", "nfs", 0, folder) == 0)
					{
						rt_kprintf("nfs mount on /nfs ok\n");
					}
					else
						rt_kprintf("nfs mount on %s failed!\n",folder);
				}
				else
					rt_kprintf("nfs mount on %s failed!\n",folder);
			}

#endif
}
FINSH_FUNCTION_EXPORT(nfs, mount nfs);
void b(void)
{
	pe p;
	int i;
	int fd,length;
	memset(&p,0xff,sizeof(pe));
	for(i=0;i<32;i++)
	{
		p.user_zone[0][i]=i;
	}
	p.ar[0][0]=0x17;//normal auth,encrypted
	p.ar[0][1]=0x61;//use g1,pw1
	for(i=0;i<7;i++)
	{
		p.ci[1][i]=i;
		if(i!=3)
			p.pw[1][i]=i;
	}
	for(i=0;i<8;i++)
	{
		p.g[1][i]=i;
	}
	p.fuse=RT_FALSE;
	fd = open("/nor/burn.txt", O_WRONLY | O_CREAT | O_TRUNC, 0);
	if (fd < 0)
	{
		rt_kprintf("open file for write failed\n");
		return;
	}
	length = write(fd, &p, sizeof(pe));
	if (length != sizeof(pe))
	{
		rt_kprintf("write data failed\n");
		close(fd);
		return;
	}
	close(fd);
	burn(p);
}
FINSH_FUNCTION_EXPORT(b, test at88sc burn);
void bf(void)
{
	pe p;
	int fd,length;
	fd = open("/nor/burn.txt", O_RDONLY, 0);
	if (fd < 0)
	{
		rt_kprintf("check: open file for read failed\n");
		return;
	}
	length = read(fd, &p, sizeof(pe));
	if (length != sizeof(pe))
	{
		rt_kprintf("check: read file failed\n");
		close(fd);
		return;
	}
	close(fd);
	burn(p);
}
FINSH_FUNCTION_EXPORT(bf, test at88sc burn from file);
void a(void)
{
	ge p;
	int i,fd,length;
	memset(&p,0xff,sizeof(ge));
	for(i=0;i<3;i++)
	{
			p.pw[i]=i;
	}
	for(i=0;i<8;i++)
	{
		p.g[i]=i;
	}
	p.use_g=1;
	p.use_pw=1;
	p.zone_index=0;
	fd = open("/nor/auth.txt", O_WRONLY | O_CREAT | O_TRUNC, 0);
	if (fd < 0)
	{
		rt_kprintf("open file for write failed\n");
		return;
	}
	length = write(fd, &p, sizeof(ge));
	if (length != sizeof(ge))
	{
		rt_kprintf("write data failed\n");
		close(fd);
		return;
	}
	close(fd);
	auth(&p);
}
FINSH_FUNCTION_EXPORT(a, test at88sc auth);
void af(void)
{
	ge p;
	int fd,length;
	fd = open("/nor/auth.txt", O_RDONLY, 0);
	if (fd < 0)
	{
		rt_kprintf("check: open file for read failed\n");
		return;
	}
	length = read(fd, &p, sizeof(ge));
	if (length != sizeof(ge))
	{
		rt_kprintf("check: read file failed\n");
		close(fd);
		return;
	}
	close(fd);
	auth(&p);
}
FINSH_FUNCTION_EXPORT(af, test at88sc auth from file);
/** @} */
