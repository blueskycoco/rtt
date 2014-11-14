struct rt_semaphore rx_sem;
static rt_mutex_t mutex = RT_NULL;
rt_device_t uart_dev = RT_NULL;
void uart_thread_entry(void* parameter);
static struct rt_thread uart_thread;
ALIGN(RT_ALIGN_SIZE)
static char uart_thread_stack[2048];
/*send config data or socket data*/
int uart_rw_config()
{

}
/*get config data to global config zone, or get socket data to buffer*/
int uart_rw_socket()
{
	
}
rt_bool_t ind_low()
{
	return ((MAP_GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_0)&(0x1<<GPIO_PIN_7))==0)?RT_TRUE:RT_FALSE);
}

void uart_thread_entry(void* parameter)
{
	char ch,rw=0;/*0 is r , 1 is w*/
	   rt_uint16_t addr=0x0000;
	   rt_uint8_t send_reg;
	   STATE_OP state=SLEEP;
	   while (1)
	   {
		 /* wait receive */
		 if (rt_sem_take(&rx_sem, RT_WAITING_FOREVER) != RT_EOK) continue;
		if(ind_low())
		{/*socket data transfer,use dma*/
			while((rt_device_read(uart_dev, 0, &ch, 1) == 1))
			{
				uart_rw_socket(ch);
			}
		}
		else
		{
			/*config data parser*/
			while((rt_device_read(uart_dev, 0, &ch, 1) == 1))
			{
				uart_rw_config(ch);
			}
		}
	   }

}

static rt_err_t uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    /* release semaphore to let finsh thread rx data */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

/*init uart1,2,3,4 for 4 socket*/
int uart_init()
{
	/*init uart device*/
	rt_err_t result;
	uart_dev = rt_device_find("uart1");
	if (uart_dev == RT_NULL)
	{
		rt_kprintf("app_uart: can not find device: %s\n", "uart1");
		return 0;
	}
	if (rt_device_open(uart_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)
	{
		rt_device_set_rx_indicate(uart_dev, uart_rx_ind);
	}
	rt_sem_init(&rx_sem, "uart1_rx", 0, 0);
	result = rt_thread_init(&uart_thread,
					"uart_rx",
					uart_thread_entry, RT_NULL,
					&uart_thread_stack[0], sizeof(uart_thread_stack),
					20, 10);
	mutex = rt_mutex_create("mutex", RT_IPC_FLAG_FIFO);
	if (mutex == RT_NULL)
	{
		rt_kprintf("create mutex failed\n");
		return 0;
	}
	if (result == RT_EOK)
		rt_thread_startup(&uart_thread);
	return 1;

}
