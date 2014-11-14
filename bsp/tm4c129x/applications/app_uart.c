#include "app_reg.h"
struct rt_semaphore rx_sem;
static rt_mutex_t mutex = RT_NULL;
rt_device_t uart_dev = RT_NULL;
void uart_thread_entry(void* parameter);
static struct rt_thread uart_thread;
ALIGN(RT_ALIGN_SIZE)
static char uart_thread_stack[2048];

static rt_err_t uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    /* release semaphore to let finsh thread rx data */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

int app_uart_init()
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
	g_lwip_app=(plwip_app)rt_malloc(sizeof(lwip_app));
	
	mutex = rt_mutex_create("mutex", RT_IPC_FLAG_FIFO);
	if (mutex == RT_NULL)
	{
	  rt_kprintf("create mutex failed\n");
	  return 0;
	}
	/*write g_lwip_app register default val*/
	set_default_reg();
	if (result == RT_EOK)
	  rt_thread_startup(&uart_thread);
    return 1;
}
rt_bool_t cs_low()
{
	return ((MAP_GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_0)&(0x1<<GPIO_PIN_7))==0)?RT_TRUE:RT_FALSE);
}
void set_default_reg()
{
	rt_err_t result;
	int i;
	result = rt_mutex_take(mutex, RT_WAITING_FOREVER);
	g_lwip_app->common->mr=		0x00;
	g_lwip_app->common->gar0=	0x00;
	g_lwip_app->common->gar1=	0x00;
	g_lwip_app->common->gar2=	0x00;
	g_lwip_app->common->gar3=	0x00;
	g_lwip_app->common->subr0=	0x00;
	g_lwip_app->common->subr1=	0x00;
	g_lwip_app->common->subr2=	0x00;
	g_lwip_app->common->subr3=	0x00;	
	g_lwip_app->common->shar0=	0x00;
	g_lwip_app->common->shar1=	0x00;
	g_lwip_app->common->shar2=	0x00;
	g_lwip_app->common->shar3=	0x00;
	g_lwip_app->common->shar4=	0x00;
	g_lwip_app->common->shar5=	0x00;	
	g_lwip_app->common->sipr0=	0x00;
	g_lwip_app->common->sipr1=	0x00;
	g_lwip_app->common->sipr2=	0x00;
	g_lwip_app->common->sipr3=	0x00;
	g_lwip_app->common->ir=		0x00;
	g_lwip_app->common->imr=	0x00;
	g_lwip_app->common->rtr0=	0x07;
	g_lwip_app->common->rtr1=	0xd0;
	g_lwip_app->common->rcr=	0x08;
	g_lwip_app->common->rmsr=	0x55;
	g_lwip_app->common->tmsr=	0x55;	
	g_lwip_app->common->uipr0=	0x00;
	g_lwip_app->common->uipr1=	0x00;
	g_lwip_app->common->uipr2=	0x00;
	g_lwip_app->common->uipr3=	0x00;
	g_lwip_app->common->uport0=	0x00;
	g_lwip_app->common->uport1=	0x00;
	for(i=0;i<4;i++)
	{
		g_lwip_app->socket[i]->mr=	0x00;
		g_lwip_app->socket[i]->cr=	0x00;
		g_lwip_app->socket[i]->ir=	0x00;
		g_lwip_app->socket[i]->sr=	0x00;		
		g_lwip_app->socket[i]->port0=	0x00;
		g_lwip_app->socket[i]->port1=	0x00;
		g_lwip_app->socket[i]->dhar0=	0x00;
		g_lwip_app->socket[i]->dhar1=	0x00;
		g_lwip_app->socket[i]->dhar2=	0x00;
		g_lwip_app->socket[i]->dhar3=	0x00;
		g_lwip_app->socket[i]->dhar4=	0x00;
		g_lwip_app->socket[i]->dhar5=	0x00;
		g_lwip_app->socket[i]->dipr0=	0x00;
		g_lwip_app->socket[i]->dipr1=	0x00;
		g_lwip_app->socket[i]->dipr2=	0x00;
		g_lwip_app->socket[i]->dipr3=	0x00;
		g_lwip_app->socket[i]->dport0=0x00;
		g_lwip_app->socket[i]->dport1=0x00;
		g_lwip_app->socket[i]->mssr0=	0xff;
		g_lwip_app->socket[i]->mssr1=	0xff;
		g_lwip_app->socket[i]->proto=	0x00;
		g_lwip_app->socket[i]->tos=	0x00;
		g_lwip_app->socket[i]->ttl=	0x80;
		g_lwip_app->socket[i]->txfsr0=0x08;
		g_lwip_app->socket[i]->txfsr1=0x00;
		g_lwip_app->socket[i]->txrd0=	0x00;
		g_lwip_app->socket[i]->txrd1=	0x00;
		g_lwip_app->socket[i]->txwr0=	0x00;
		g_lwip_app->socket[i]->txwr1=	0x00;
		g_lwip_app->socket[i]->rxrsr0=0x00;
		g_lwip_app->socket[i]->rxrsr1=0x00;
		g_lwip_app->socket[i]->rxrd0=	0x00;
		g_lwip_app->socket[i]->rxrd1=	0x00;
		g_lwip_app->socket[i]->rxwr0=	0x00;
		g_lwip_app->socket[i]->rxwr1=	0x00;
	}
	rt_mutex_release(mutex);
}
rt_uint8_t rw_reg(rt_uint16_t addr,rt_uint8_t data,rt_bool_t rw)
{
	rt_uint8_t ret=0x0;
	rt_err_t result;
	result = rt_mutex_take(mutex, RT_WAITING_FOREVER);
	if(addr>=COMMON_REG_MR && addr<=COMMON_REG_TMSR)
	{
		if(rw)/*write*/
		{
			if(addr!=COMMON_REG_IR)
				*(rt_uint8_t *)(g_lwip_app->common+addr)=data;
			else
			{
				//clear conflict , unreach
				*(rt_uint8_t *)(g_lwip_app->common+addr)&=~(data&0xc0);
			}
		}
		else
			ret=*(rt_uint8_t *)(g_lwip_app->common+addr);
	}
	else if(addr>=COMMON_REG_UIPR0 && addr<=COMMON_REG_UPORT1)
	{
		if(rw)/*write*/
		{
			if(addr!=COMMON_REG_UIPR0&&addr!=COMMON_REG_UIPR1&&addr!=COMMON_REG_UIPR2&&addr!=COMMON_REG_UIPR3&&addr!=COMMON_REG_UPORT0&&addr!=COMMON_REG_UPORT1)
				*(rt_uint8_t *)(g_lwip_app->common+addr-0x10)=data;
		}
		else
			ret=*(rt_uint8_t *)(g_lwip_app->common+addr-0x10);
	}
	else if(addr>=SOCKET_REG_S0_MR && addr<=SOCKET_REG_S0_TTL)
	{
		if(rw)/*write*/
		{
			if(addr!=SOCKET_REG_S0_IR&&addr!=SOCKET_REG_S0_SR)
				*(rt_uint8_t *)(g_lwip_app->socket[0]+addr)=data;
			else
			{
				if(addr==SOCKET_REG_S0_IR)
					*(rt_uint8_t *)(g_lwip_app->socket[0]+addr)&=~(data&0x1f);
			}
		}
		else
			ret=*(rt_uint8_t *)(g_lwip_app->socket[0]+addr);
	}
	else if(addr>=SOCKET_REG_S0_TX_FSR0 && addr<=SOCKET_REG_S0_RX_WR1)
	{
		if(rw)/*write*/
			if(addr!=SOCKET_REG_S0_TX_FSR0&&addr!=SOCKET_REG_S0_TX_FSR1&&addr!=SOCKET_REG_S0_TX_RD0&&addr!=SOCKET_REG_S0_TX_RD1)
				addr!=SOCKET_REG_S0_RX_RSR0&&addr!=SOCKET_REG_S0_RX_RSR1)
			*(rt_uint8_t *)(g_lwip_app->socket[0]+addr-0x40B)=data;
		else
			ret=*(rt_uint8_t *)(g_lwip_app->socket[0]+addr-0x40B);
	}
	else if(addr>=SOCKET_REG_S1_MR && addr<=SOCKET_REG_S1_TTL)
	{
		if(rw)/*write*/
		{
			if(addr!=SOCKET_REG_S1_IR&&addr!=SOCKET_REG_S1_SR)
				*(rt_uint8_t *)(g_lwip_app->socket[1]+addr)=data;
			else
			{
				if(addr==SOCKET_REG_S1_IR)
					*(rt_uint8_t *)(g_lwip_app->socket[1]+addr)&=~(data&0x1f);
			}
		}
		else
			ret=*(rt_uint8_t *)(g_lwip_app->socket[1]+addr);
	}
	else if(addr>=SOCKET_REG_S1_TX_FSR0 && addr<=SOCKET_REG_S1_RX_WR1)
	{
		if(rw)/*write*/
			if(addr!=SOCKET_REG_S1_TX_FSR0&&addr!=SOCKET_REG_S1_TX_FSR1&&addr!=SOCKET_REG_S1_TX_RD0&&addr!=SOCKET_REG_S1_TX_RD1)
				addr!=SOCKET_REG_S1_RX_RSR0&&addr!=SOCKET_REG_S1_RX_RSR1)
			*(rt_uint8_t *)(g_lwip_app->socket[1]+addr-0x50B)=data;
		else
			ret=*(rt_uint8_t *)(g_lwip_app->socket[1]+addr-0x50B);
	}
	else if(addr>=SOCKET_REG_S2_MR && addr<=SOCKET_REG_S2_TTL)
	{
		if(rw)/*write*/
		{
			if(addr!=SOCKET_REG_S2_IR&&addr!=SOCKET_REG_S2_SR)
				*(rt_uint8_t *)(g_lwip_app->socket[2]+addr)=data;
			else
			{
				if(addr==SOCKET_REG_S2_IR)
					*(rt_uint8_t *)(g_lwip_app->socket[2]+addr)&=~(data&0x1f);
			}
		}
		else
			ret=*(rt_uint8_t *)(g_lwip_app->socket[2]+addr);
	}
	else if(addr>=SOCKET_REG_S2_TX_FSR0 && addr<=SOCKET_REG_S2_RX_WR1)
	{
		if(rw)/*write*/
			if(addr!=SOCKET_REG_S2_TX_FSR0&&addr!=SOCKET_REG_S2_TX_FSR1&&addr!=SOCKET_REG_S2_TX_RD0&&addr!=SOCKET_REG_S2_TX_RD1)
				addr!=SOCKET_REG_S2_RX_RSR0&&addr!=SOCKET_REG_S2_RX_RSR1)
			*(rt_uint8_t *)(g_lwip_app->socket[2]+addr-0x60B)=data;
		else
			ret=*(rt_uint8_t *)(g_lwip_app->socket[2]+addr-0x60B);
	}
	else if(addr>=SOCKET_REG_S3_MR && addr<=SOCKET_REG_S3_TTL)
	{
		if(rw)/*write*/
		{
			if(addr!=SOCKET_REG_S3_IR&&addr!=SOCKET_REG_S3_SR)
				*(rt_uint8_t *)(g_lwip_app->socket[3]+addr)=data;
			else
			{
				if(addr==SOCKET_REG_S3_IR)
					*(rt_uint8_t *)(g_lwip_app->socket[3]+addr)&=~(data&0x1f);
			}
		}
		else
			ret=*(rt_uint8_t *)(g_lwip_app->socket[3]+addr);
	}
	else if(addr>=SOCKET_REG_S3_TX_FSR0 && addr<=SOCKET_REG_S3_RX_WR1)
	{
		if(rw)/*write*/
			if(addr!=SOCKET_REG_S3_TX_FSR0&&addr!=SOCKET_REG_S3_TX_FSR1&&addr!=SOCKET_REG_S3_TX_RD0&&addr!=SOCKET_REG_S3_TX_RD1)
				addr!=SOCKET_REG_S3_RX_RSR0&&addr!=SOCKET_REG_S3_RX_RSR1)
			*(rt_uint8_t *)(g_lwip_app->socket[3]+addr-0x70B)=data;
		else
			ret=*(rt_uint8_t *)(g_lwip_app->socket[3]+addr-0x70B);
	}
	else
		rt_kprintf("invalid addr in rw_reg\n");
	rt_mutex_release(mutex);
	return ret;
}
enum STATE_OP{
	GET_OPERATION,
	GET_ADDR_0,
	GET_ADDR_1,
	GET_DATA
};
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

        /* read one character from device */
        while ((rt_device_read(uart_dev, 0, &ch, 1) == 1) && cs_low())
        {

		rt_kprintf("==>%c",ch);
		switch (state)
			{
				case GET_OPERATION:/*Write or Read process*/					
					{
						if(ch==0x0f)
						{
							rw=0;
							state=GET_ADDR_0;
						}
						else if(ch==0xf0)
						{
							rw=1;
							state=GET_ADDR_0;
						}
						else
							rt_kprintf("not vaild op code %x\n",ch);
					}
					break;
				case GET_ADDR_0:
					{
						addr=ch;
						state=GET_ADDR_1;
					}
					break;
				case GET_ADDR_1:	
					{
						addr=(addr<<8)|ch;
						if(rw==0)
						{
							/*find the read and send*/
							send_reg=rw_reg(addr,0,0);
							rt_device_write(uart_dev,0,&send_reg,1);
							state=GET_OPERATION;							
							rt_kprintf("r Addr 0x%2x,Data 0x%x\n",addr,send_reg);
						}
						else
						{
							state=GET_DATA;
						}
					}
					break;
				case GET_DATA:	
					{		
						send_reg=rw_reg(addr,ch,1);
						rt_device_write(uart_dev,0,&send_reg,1);
						rt_kprintf("w Addr 0x%2x,Data 0x%x\n",addr,ch);
						state=GET_OPERATION;						
					}
					break;             
        } /* end of device read */
    }
}
}
