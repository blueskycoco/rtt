#include "lwip/opt.h"
#include "lwip/tcp.h"
#include "con_socket.h"
struct rt_thread socket_thread[4];
static char socket_thread_stack[4][2048];
static rt_uint8_t buf[4096];
struct tcp_pcb *pcb[4];

static err_t socket_tcp_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  //struct netio_state *ns = arg;
  u8_t * data_ptr;
  u32_t data_cntr;
  rt_uint8_t *ptr=buf;
  int len=0;
  struct pbuf *q = p;

  if (p != NULL) {
    tcp_recved(pcb, p->tot_len);
  }

  if (err == ERR_OK && q != NULL) {

    while (q != NULL) {
      data_cntr = q->len;
      data_ptr = q->payload;
      while (data_cntr--) 
	{
		/* end of while data still in this pbuf */
		*ptr++=*data_ptr++;
		len++;
	}
      q = q->next;
    }

    pbuf_free(p);

  } else {

    /* error or closed by other side */
    if (p != NULL) {
      pbuf_free(p);
    }
  }
  socket_write_buf(arg,buf,len);
  return ERR_OK;

}

void socket_tcp_send(struct tcp_pcb *pcb, rt_uint8_t *data,u16_t len)
{

	err_t err = ERR_OK;
	int pcb_len;
	rt_uint8_t *ptr=data;
	while(len>0)
	{
		pcb_len = tcp_sndbuf(pcb);
		pcb_len = LWIP_MIN(len, pcb_len);



		do {
			err = tcp_write(pcb, ptr, pcb_len, TCP_WRITE_FLAG_COPY);
			if (err == ERR_MEM) 
			{
				pcb_len /= 2;
			}
		} while ((err == ERR_MEM) && (pcb_len > 1));

		ptr += pcb_len;
		len=len-pcb_len;
	}
  return ERR_OK;
}
void socket_send(int index,rt_uint8_t *data,int len)
{
	socket_tcp_send(NULL,pcb[index],len);
}
static err_t socket_tcp_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
	LWIP_UNUSED_ARG(err);
	tcp_arg(pcb, arg);
	//tcp_sent(pcb, netio_sent);
	tcp_recv(pcb, socket_tcp_recv);
	//tcp_poll(pcb, netio_poll, 4); /* every 2 seconds */
	DBG("incoming connection got\r\n");
	cnn_out(0,1);
	return ERR_OK;
}
err_t socket_tcp_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
	rt_kprintf("socket is connected to target\r\n");
	cnn_out(0,1);
    return ERR_OK;
}

void socket_thread_entry(void* parameter)
{
	int dev=(int)parameter;	
	int i=0;
	pcb[dev] = tcp_new();
	if(g_conf.server_mode[dev]==1)
	{
		int port=(g_conf.local_port[0]<<8)|g_conf.local_port[1];
		tcp_bind(pcb[dev], IP_ADDR_ANY, port);
		pcb[dev] = tcp_listen(pcb[dev]);
		tcp_accept(pcb[dev], socket_tcp_accept);
	}
	else
	{
		struct ip_addr ipaddr;
		rt_uint16_t port;
		if(dev==0)
		{
			IP4_ADDR(&ipaddr,g_conf.remote_ip0[0],g_conf.remote_ip0[1],g_conf.remote_ip0[2],g_conf.remote_ip0[3]);
			port=(g_conf.remote_port0[0]<<8)|g_conf.remote_port0[1];
		}
		else if(dev==1)
		{
			IP4_ADDR(&ipaddr,g_conf.remote_ip1[0],g_conf.remote_ip1[1],g_conf.remote_ip1[2],g_conf.remote_ip1[3]);
			port=(g_conf.remote_port1[0]<<8)|g_conf.remote_port1[1];
		}
		else if(dev==2)
		{
			IP4_ADDR(&ipaddr,g_conf.remote_ip2[0],g_conf.remote_ip2[1],g_conf.remote_ip2[2],g_conf.remote_ip2[3]);
			port=(g_conf.remote_port2[0]<<8)|g_conf.remote_port2[1];
		}
		else
		{
			IP4_ADDR(&ipaddr,g_conf.remote_ip3[0],g_conf.remote_ip3[1],g_conf.remote_ip3[2],g_conf.remote_ip3[3]);
			port=(g_conf.remote_port3[0]<<8)|g_conf.remote_port3[1];
		}
		tcp_recv(pcb[dev], socket_tcp_recv);
		tcp_connect(pcb[dev],&ipaddr,port,socket_tcp_connected);
	}
}
/*init socket 1,2,3,4*/
int socket_init()
{
	rt_uint8_t thread_string[4][10];
	int i=0;
	for(i=0;i<4;i++)
	{
		rt_memset(thread_string[i],'\0',10);
		rt_sprintf(thread_string[i],"socket_%d",i);
		rt_err_t result = rt_thread_init(&(socket_thread[0]),
						thread_string,
						socket_thread_entry, (void *)(&i),
						&(socket_thread_stack[i][0]), sizeof(socket_thread_stack[0]),		
						20, 10);
		if (result == RT_EOK)
			rt_thread_startup(&socket_thread[i]);
	}
	return 1;
}
