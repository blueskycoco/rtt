#include "socket.h"

enum STATE_OP{
	GET_F5,
	GET_8A_8B,
	GET_DATA,
	GET_26,
	GET_FA
};
struct rt_semaphore rx_sem[4];
rt_mutex_t mutex = RT_NULL;
rt_device_t uart_dev[4] = {RT_NULL,RT_NULL,RT_NULL,RT_NULL};
void uart_thread_entry(void* parameter);
struct rt_thread uart_thread[4];
//ALIGN(RT_ALIGN_SIZE)
static char uart_thread_stack[4][2048];
int which_uart_dev(rt_device_t *dev,rt_device_t dev2)
{
	int i=0;
	for(i=0;i<4;i++)
		if(memcmp(dev[i],dev2,sizeof(dev2)==0)
		{
			rt_kprintf("Uart %d 's setting\r\n",i);
			break;
		}
		return i;
}

/*get config data to global config zone, or get socket data to buffer*/
int uart_rw_socket(rt_device_t dev,unsigned char ch)
{	
	if(which_uart_dev(uart_dev,dev)==0)
	{

	}
	else if(which_uart_dev(uart_dev,dev)==1)
	{

	}
	else if(which_uart_dev(uart_dev,dev)==2)
	{

	}
	else if(which_uart_dev(uart_dev,dev)==3)
	{

	}
	return 0;
}

void uart_rw_config(rt_device_t dev,unsigned char ch)
{
	static rt_uint8_t buf[6];
	rt_uint8_t i=0;
	static rt_uint8_t len=0,param;
	static STATE_OP state=GET_F5;
	switch(state)
	{
		case GET_F5:
		{
			if(ch==0xf5)
			{
				DBG("Dev %d , 0XF5 Got\r\n",which_uart_dev(uart_dev,dev));
				state=GET_8A_8B;
			}
		}
		break;
		case GET_8A_8B:
		{
			if(ch==0x8a)
			{
				DBG("Dev %d , 0X8A Got\r\n",which_uart_dev(uart_dev,dev));
				state=GET_DATA;
			}
			else if(ch==0x8b)
			{
				/*send config data out*/
				unsigned char i,*ptr=(unsigned char *)g_conf;
				int result=0;
				DBG("Dev %d , 0X8B Got\r\n",which_uart_dev(uart_dev,dev));
				get_config[0]=0xf5;
				get_config[1]=0x8C;
				result=0xf5+0x8c+0x27+0xfa;
				for(i=0;i<sizeof(g_conf);i++)
				{
					get_config[2+i]=ptr[i];
					result=result+get_config[2+i];
				}
				get_config[2+i+1]=0x27;
				get_config[2+i+2]=0xfa;
				get_config[2+i+3]=(result&0xff00)>>8;
				get_config[2+i+4]=result&0xff;
				rt_device_write(dev,0,get_config,sizeof(get_config));
			}
			state=GET_F5;
		}
		break;
		case GET_DATA:
		{
			param=ch;
			DBG("Dev %d , 0x%2x Got\r\n",which_uart_dev(uart_dev,dev),ch);
			if(ch==0||ch==2||ch==3||ch==5||ch==6||ch==7||ch==8)
				len=4;
			else if(ch==1||ch==9||ch==10||ch==11||ch==12)
				len=2;
			else if(ch==4)
				len=6;
			else if(ch==13||ch==14||ch==15)
				len=1;
			for(i=0;i<len;i++)
				while((rt_device_read(dev,0,&(buf[i]),1)==0));
			state=GET_26;
		}
		break;
		case GET_26:
		{
			if(ch==0x26)
			{
				DBG("Dev %d , 0X26 Got\r\n",which_uart_dev(uart_dev,dev));
				state=GET_FA;
			}
			else
				state=GET_F5;
		}
		break;
		case GET_FA:
		{
			if(ch==0xFA)
			{
				unsigned char crc1,crc2,*ptr;
				DBG("Dev %d , 0XFA Got\r\n",which_uart_dev(uart_dev,dev));
				int result=0xf5+0x8a+param+0x26+0xfa;
				while((rt_device_read(dev,0,&(crc1),1)==0));
				while((rt_device_read(dev,0,&(crc2),1)==0));
				for(i=0;i<len;i++)
					result=result+buf[i];
				if(result==(crc1<<8|crc2))
				{
					switch(param)
					{
						case 0://local ip		
							ptr=g_conf.local_ip;
						break;
						case 1://local port 	
							ptr=g_conf.local_port;
						break;
						case 2://sub msk		
							ptr=g_conf.sub_msk;
						break;
						case 3://gw ip		
							ptr=g_conf.gw;
						break;
						case 4://mac		
							ptr=g_conf.mac;
						break;
						case 5://socket 0 ip		
						{
							if(which_uart_dev(uart_dev,dev)==0)
								ptr=g_conf.remote_ip0;											
						}
						break;
						case 6://socket 1 ip											
						{
							if(which_uart_dev(uart_dev,dev)==1)
								ptr=g_conf.remote_ip1;
						}
						break;
						case 7://socket 2 ip											
						{
							if(which_uart_dev(uart_dev,dev)==2)
								ptr=g_conf.remote_ip2;
						}
						break;
						case 8://socket 3 ip													
						{
							if(which_uart_dev(uart_dev,dev)==3)
								ptr=g_conf.remote_ip3;
						}
						break;
						case 9://socket 0 port										
						{
							if(which_uart_dev(uart_dev,dev)==0)
								ptr=g_conf.remote_port0;
						}
						break;
						case 10://socket 1 port 										
						{
							if(which_uart_dev(uart_dev,dev)==1)
								ptr=g_conf.remote_port1;
						}
						break;
						case 11://socket 2 port 										
						{
							if(which_uart_dev(uart_dev,dev)==2)
								ptr=g_conf.remote_port2;
						}
						break;
						case 12://socket 3 port 											
						{
							if(which_uart_dev(uart_dev,dev)==3)
								ptr=g_conf.remote_port3;
						}
						break;									
						case 13://protol
							ptr=g_conf.protol+which_uart_dev(uart_dev,dev);
						break;
						case 14://server or client mode
							ptr=g_conf.server_mode+which_uart_dev(uart_dev,dev);
						break;									
						case 15://socket uart baud
						{

							struct serial_configure config;
							ptr=g_conf.uart_baud+which_uart_dev(uart_dev,dev);
							i=which_uart_dev(uart_dev,dev);
							if(buf[i]==0)
								config.baud_rate = 115200;
							else if(buf[i]==1)
								config.baud_rate = 406800;
							else if(buf[i]==2)
								config.baud_rate = 921600;
							else if(buf[i]==3)
								config.baud_rate = 2000000;
							else if(buf[i]==4)
								config.baud_rate = 4000000;
							else if(buf[i]==5)
								config.baud_rate = 6000000;

							config.bit_order = BIT_ORDER_LSB;
							config.data_bits = DATA_BITS_8;
							config.parity	 = PARITY_NONE;
							config.stop_bits = STOP_BITS_1;
							config.invert	 = NRZ_NORMAL;
							config.bufsz	 = RT_SERIAL_RB_BUFSZ;
							rt_device_control(dev,RT_DEVICE_CTRL_CONFIG,&config);

						}
						break;
						default:
							ptr=NULL;
					}
				if(ptr!=NULL)
					for(i=0;i<len;i++)
						ptr[i]=buf[i];
			}
				else
					DBG("Dev %d , crc fault %x!=%x\r\n",which_uart_dev(uart_dev,dev),result,(crc1<<8)|crc2);
		}
		state=GET_F5;
		}
		break;
		default:
			state=GET_F5;
		break;
	}

}
rt_bool_t ind_low(rt_device_t dev)
{
	if(which_uart_dev(uart_dev,dev)==0)
		return ((MAP_GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_0)&(0x1<<GPIO_PIN_7))==0)?RT_TRUE:RT_FALSE);
	else if(which_uart_dev(uart_dev,dev)==1)
		return ((MAP_GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_0)&(0x1<<GPIO_PIN_7))==0)?RT_TRUE:RT_FALSE);
	else if(which_uart_dev(uart_dev,dev)==2)
		return ((MAP_GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_0)&(0x1<<GPIO_PIN_7))==0)?RT_TRUE:RT_FALSE);
	else if(which_uart_dev(uart_dev,dev)==3)
		return ((MAP_GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_0)&(0x1<<GPIO_PIN_7))==0)?RT_TRUE:RT_FALSE);

	return RT_FALSE;
}
void uart_thread_entry(void* parameter)
{
	char ch,rw=0;/*0 is r , 1 is w*/
	
	rt_device_t dev=(rt_device_t)parameter;
	while (1)
	{
		/* wait receive */
		if (rt_sem_take(&rx_sem, RT_WAITING_FOREVER) != RT_EOK) continue;
		if(ind_low(dev))
		{/*socket data transfer,use dma*/
			while((rt_device_read(dev, 0, &ch, 1) == 1))
			{
				uart_rw_socket(dev,ch);
			}
		}
		else
		{
			/*config data parser*/
			while((rt_device_read(dev, 0, &ch, 1) == 1))
			{
				uart_rw_config(dev,ch);
			}
		}
	}
}

static rt_err_t uart_rx0_ind(rt_device_t dev, rt_size_t size)
{
    /* release semaphore to let finsh thread rx data */
    rt_sem_release(&rx_sem[0]);

    return RT_EOK;
}
static rt_err_t uart_rx1_ind(rt_device_t dev, rt_size_t size)
{
    /* release semaphore to let finsh thread rx data */
    rt_sem_release(&rx_sem[1]);

    return RT_EOK;
}
static rt_err_t uart_rx2_ind(rt_device_t dev, rt_size_t size)
{
    /* release semaphore to let finsh thread rx data */
    rt_sem_release(&rx_sem[2]);

    return RT_EOK;
}
static rt_err_t uart_rx3_ind(rt_device_t dev, rt_size_t size)
{
    /* release semaphore to let finsh thread rx data */
    rt_sem_release(&rx_sem[3]);

    return RT_EOK;
}

/*init uart1,2,3,4 for 4 socket*/
int uart_init()
{
	/*init uart device*/
	rt_err_t result;
	uart_dev[0] = rt_device_find("uart1");
	if (uart_dev[0] == RT_NULL)
	{
		rt_kprintf("app_uart: can not find device: %s\n", "uart1");
		return 0;
	}
	if (rt_device_open(uart_dev[0], RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)
	{
		rt_device_set_rx_indicate(uart_dev[0], uart_rx0_ind);
	}
	rt_sem_init(&rx_sem[0], "uart1_rx", 0, 0);
	result = rt_thread_init(&uart_thread[0],
					"uart_rx1",
					uart_thread_entry, (void *)uart_dev[0],
					&uart_thread_stack[0][0], sizeof(uart_thread_stack[0]),
					20, 10);
	
	uart_dev[1] = rt_device_find("uart2");
	if (uart_dev[1] == RT_NULL)
	{
		rt_kprintf("app_uart: can not find device: %s\n", "uart2");
		return 0;
	}
	if (rt_device_open(uart_dev[1], RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)
	{
		rt_device_set_rx_indicate(uart_dev[1], uart_rx1_ind);
	}
	rt_sem_init(&rx_sem[1], "uart2_rx", 0, 0);
	result = rt_thread_init(&uart_thread[1],
					"uart_rx2",
					uart_thread_entry, (void *)uart_dev[1],
					&uart_thread_stack[1][0], sizeof(uart_thread_stack[1]),
					20, 10);
	
	uart_dev[2] = rt_device_find("uart3");
	if (uart_dev[2] == RT_NULL)
	{
		rt_kprintf("app_uart: can not find device: %s\n", "uart2");
		return 0;
	}
	if (rt_device_open(uart_dev[2], RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)
	{
		rt_device_set_rx_indicate(uart_dev[2], uart_rx2_ind);
	}
	rt_sem_init(&rx_sem[0], "uart2_rx", 0, 0);
	result = rt_thread_init(&uart_thread[2],
					"uart_rx2",
					uart_thread_entry, (void *)uart_dev[2],
					&uart_thread_stack[2][0], sizeof(uart_thread_stack[2]),
					20, 10);
	uart_dev[3] = rt_device_find("uart4");
	if (uart_dev[3] == RT_NULL)
	{
		rt_kprintf("app_uart: can not find device: %s\n", "uart3");
		return 0;
	}
	if (rt_device_open(uart_dev[3], RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)
	{
		rt_device_set_rx_indicate(uart_dev[3], uart_rx3_ind);
	}
	rt_sem_init(&rx_sem[3], "uart3_rx", 0, 0);
	result = rt_thread_init(&uart_thread[3],
					"uart_rx3",
					uart_thread_entry, (void *)uart_dev[3],
					&uart_thread_stack[3][0], sizeof(uart_thread_stack[3]),
					20, 10);

	mutex = rt_mutex_create("mutex", RT_IPC_FLAG_FIFO);
	if (mutex == RT_NULL)
	{
		rt_kprintf("create mutex failed\n");
		return 0;
	}
	if (result == RT_EOK)
	{
		rt_thread_startup(&uart_thread[0]);
		rt_thread_startup(&uart_thread[1]);
		rt_thread_startup(&uart_thread[2]);
		rt_thread_startup(&uart_thread[3]);
	}
	return 1;

}
