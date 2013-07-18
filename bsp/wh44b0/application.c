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
void nfs(const char *folder);
void rt_led_thread_entry(void *parameter)
{
	while (1)
	{
		/* light on leds for one second */
		//rt_kprintf("Led on rom\n");
		rt_hw_led_set(0xff);
		rt_thread_delay(200);
		//rt_kprintf("Led off rom\n");
		/* light off leds for one second */
		rt_hw_led_set(0x00);
		rt_thread_delay(200);
	}
}

void rt_net_thread_entry(void *parameter)
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
	ftpd_start();
	chargen();
	nfs_init();
	nfs("192.168.0.100:/test");
}
/* application start function */
int rt_application_init()
{
	rt_thread_t led_thread;
	rt_thread_t net_thread;
	led_thread = rt_thread_create("led",rt_led_thread_entry, RT_NULL,512,20,20);
	net_thread = rt_thread_create("net",rt_net_thread_entry, RT_NULL,1024,20,20);
	if(led_thread != RT_NULL)
		rt_thread_startup(led_thread);
	if(net_thread != RT_NULL)
		rt_thread_startup(net_thread);
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
void b(unsigned char zone)
{
	pe p;
	int i;
	int fd,length;
	memset(&p,0xff,sizeof(pe));
	for(i=0;i<32;i++)
	{
		p.user_zone[zone][i]=i;
	}
	p.ar[zone][0]=0x17;//normal auth,encrypted
	p.ar[zone][1]=(zone<<6)|(zone&0x3);//use g1,pw1
	for(i=0;i<7;i++)
	{
		p.ci[zone][i]=i;
		if(i!=3)
			p.pw[zone][i]=i;
	}
	for(i=0;i<8;i++)
	{
		p.g[zone][i]=i;
	}
	p.fuse=0x00;
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
	//burn(p);
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
void cb()
{
	rt_kprintf("Call back called\r\n");
}
void a(unsigned char zone)
{
	ge p;
	int i,fd,length;
	memset(&p,0xff,sizeof(ge));
	for(i=0;i<32;i++)
	{
		p.user_zone[i]=i;
	}
	for(i=0;i<3;i++)
	{
		p.pw[i]=i;
	}
	for(i=0;i<8;i++)
	{
		p.g[i]=i;
	}
	if(zone==0){
		p.use_g=1;
		p.use_pw=1;
	}else{
		p.use_g=zone;
		p.use_pw=zone;
	}
	p.zone_index=zone;
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
	auth(&p,(callback_t *)cb);
}
FINSH_FUNCTION_EXPORT(a, a(1).test at88sc auth);
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
	auth(&p,(callback_t)cb);
}
FINSH_FUNCTION_EXPORT(af, test at88sc auth from file);
/** @} */
