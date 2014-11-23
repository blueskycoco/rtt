#include <rtthread.h>
#include <board.h>
#include <components.h>
typedef struct {
	rt_uint8_t local_ip[4];
	rt_uint8_t local_port[2];
	rt_uint8_t sub_msk[4];
	rt_uint8_t gw[4];
	rt_uint8_t mac[6];
	rt_uint8_t remote_ip0[4];
	rt_uint8_t remote_ip1[4];
	rt_uint8_t remote_ip2[4];
	rt_uint8_t remote_ip3[4];
	rt_uint8_t remote_port0[2];
	rt_uint8_t remote_port1[2];
	rt_uint8_t remote_port2[2];
	rt_uint8_t remote_port3[2];
	rt_uint8_t protol[4];
	rt_uint8_t server_mode[4];
	rt_uint8_t uart_baud[4];
}config,*pconfig;
typedef struct {
	rt_uint8_t *socket_buf_send[4];/*every buf use 10k*/
	rt_uint8_t *socket_buf_recv[4];
	rt_uint16_t w_send_index[4];
	rt_uint16_t r_send_index[4];	
	rt_uint16_t w_recv_index[4];
	rt_uint16_t r_recv_index[4];
}ringbuf,*pringbuf;
pringbuf g_ringbuf;
config g_conf;

void socket_send(int index,rt_uint8_t *data,int len);
#if debug
#define DBG rt_kprintf
#else
#define DBG 
#endif
