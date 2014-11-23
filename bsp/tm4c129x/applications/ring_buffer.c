#include "con_socket.h"
#include "ring_buffer.h"
#define BUF_SZ 4096
struct rt_semaphore socket_rx_sem[4],interface_rx_sem[4];
struct rt_thread ring_buf_thread[8];
static char ring_interface_thread_stack[4][2048];
static char ring_socket_thread_stack[4][2048];
int length_socket[4],length_interface[4];
extern rt_device_t uart_dev[4];

int interface_write_buf(int index,unsigned char* ch,int len)
{
	int r_index=g_ringbuf->r_send_index[index];
	int i=0;
	while(len!=0)
	{
		g_ringbuf->socket_buf_send[g_ringbuf->w_send_index[index]] = ch[i];
		g_ringbuf->w_send_index[index] += 1;
		if (g_ringbuf->w_send_index[index] >= BUF_SZ) g_ringbuf->w_send_index[index] = 0;

		/* if the next position is read index, discard this 'read char' */
		if (g_ringbuf->w_send_index[index] == r_index)
		{
		  r_index += 1;
		  if (r_index >= BUF_SZ) r_index = 0;
		}
		len--;
		i++;
	}
	g_ringbuf->r_send_index[index]=r_index;
	rt_sem_release(&(interface_rx_sem[index]));
}
int socket_write_buf(int index,unsigned char* ch,int len)
{
	int r_index=g_ringbuf->r_recv_index[index];
	int i=0;
	while(len!=0)
	{
		g_ringbuf->socket_buf_recv[g_ringbuf->w_recv_index[index]] = ch[i];
		g_ringbuf->w_recv_index[index] += 1;
		if (g_ringbuf->w_recv_index[index] >= BUF_SZ) g_ringbuf->w_recv_index[index] = 0;

		/* if the next position is read index, discard this 'read char' */
		if (g_ringbuf->w_recv_index[index] == r_index)
		{
		  r_index += 1;
		  if (r_index >= BUF_SZ) r_index = 0;
		}
		len--;
		i++;
	}
	g_ringbuf->r_recv_index[index]=r_index;
	rt_sem_release(&(socket_rx_sem[index]));
}
void ring_interface_thread_entry(void* parameter)
{
	int socket=(int)parameter;
	int length=0,r_index,w_index;
	
	while (1)
	{
		/* wait receive */
		if (rt_sem_take(&(interface_rx_sem[socket]), RT_WAITING_FOREVER) != RT_EOK) continue;
		{
			length=0;
			r_index=g_ringbuf->r_send_index[socket];
			w_index=g_ringbuf->w_send_index[socket];
			while (1)
			{
				int ch;
				if (w_index != r_index)
				{
					r_index += 1;
					if (r_index >= BUF_SZ) r_index = 0;
				}
				else
				{
					break;
				}

				length++;
			}
			socket_send(uart_dev[socket],g_ringbuf->socket_buf_send[g_ringbuf->r_send_index[socket]],length);
			g_ringbuf->r_send_index[socket]=r_index;
		}

	}
}
void ring_socket_thread_entry(void* parameter)
{
	int uart_dev1=(int)parameter;
	int length=0,r_index=0,w_index;
	
	while (1)
	{
		/* wait receive */
		if (rt_sem_take(&(socket_rx_sem[uart_dev1]), RT_WAITING_FOREVER) != RT_EOK) continue;
		{
			length=0;	
			r_index=g_ringbuf->r_recv_index[uart_dev1];
			w_index=g_ringbuf->w_recv_index[uart_dev1];
			while (1)
			{
				int ch;
				if (r_index != w_index)
				{
					r_index += 1;
					if (r_index >= BUF_SZ) r_index = 0;
				}
				else
				{
					break;
				}

				length++;
			}
			rt_device_write(uart_dev[uart_dev1],0,g_ringbuf->socket_buf_recv[g_ringbuf->r_recv_index[uart_dev1]],length*sizeof(rt_uint8_t));
			g_ringbuf->r_recv_index[uart_dev1]=r_index;
		}

	}
}
int ring_buffer_init()
{
	int i;
	rt_err_t result;
	rt_uint8_t thread_string[8][10];
	g_ringbuf = (ringbuf *)rt_malloc(sizeof(ringbuf) + BUF_SZ*8);
	RT_ASSERT(g_ringbuf != RT_NULL);
	for(i=0;i<BUF_SZ*4;i=i+BUF_SZ)
	{
		g_ringbuf->socket_buf_send[i] = (rt_uint8_t*) (g_ringbuf + 1 + i);
		rt_memset(g_ringbuf->socket_buf_send[i], 0, BUF_SZ);
	}
	for(i=BUF_SZ*4;i<BUF_SZ*8;i=i+BUF_SZ)
	{
		g_ringbuf->socket_buf_recv[i] = (rt_uint8_t*) (g_ringbuf + 1 + i);
		rt_memset(g_ringbuf->socket_buf_recv[i], 0, BUF_SZ);
	}	
	for(i=0;i<4;i++)
	{
		g_ringbuf->w_send_index[i] = 0;
		g_ringbuf->r_send_index[i] = 0;		
		g_ringbuf->w_recv_index[i] = 0;
		g_ringbuf->r_recv_index[i] = 0;
	}
	rt_sem_init(&(interface_rx_sem[0]), "socket0_tx_sem", 0, 0);
	rt_sem_init(&(interface_rx_sem[1]), "socket1_tx_sem", 0, 0);
	rt_sem_init(&(interface_rx_sem[2]), "socket2_tx_sem", 0, 0);
	rt_sem_init(&(interface_rx_sem[3]), "socket3_tx_sem", 0, 0);	
	rt_sem_init(&(socket_rx_sem[0]), 	"socket0_rx_sem", 0, 0);
	rt_sem_init(&(socket_rx_sem[1]), 	"socket1_rx_sem", 0, 0);
	rt_sem_init(&(socket_rx_sem[2]), 	"socket2_rx_sem", 0, 0);
	rt_sem_init(&(socket_rx_sem[3]), 	"socket3_rx_sem", 0, 0);
	for(i=0;i<4;i++)
	{
		rt_sprintf(thread_string[i],"ring_buf_%d",i);
		result = rt_thread_init(&(ring_buf_thread[0]),thread_string[i],ring_interface_thread_entry, &i,&(ring_interface_thread_stack[i][0]), sizeof(ring_interface_thread_stack[i]),20, 10);
		if(result==RT_EOK)
			rt_thread_startup(&ring_buf_thread[i]);
	}	
	for(i=0;i<4;i++)
	{
		rt_sprintf(thread_string[i],"ring_buf_%d",i);
		result = rt_thread_init(&(ring_buf_thread[0]),thread_string[i],ring_socket_thread_entry, &i,&(ring_socket_thread_stack[i][0]), sizeof(ring_socket_thread_stack[i]),20, 10);
		if(result==RT_EOK)
			rt_thread_startup(&ring_buf_thread[i]);
	}
}

