#include <rtthread.h>
#include <board.h>
#include <components.h>
typedef struct {
	rt_uint32_t local_ip;
	rt_uint16_t local_port;
	rt_uint32_t sub_msk;
	rt_uint32_t gw;
	rt_uint8_t mac[6];
	rt_uint32_t remote_ip[4];
	rt_uint8_t remote_ip6[4][16];
	rt_uint16_t remote_port[4];
	rt_uint8_t config[4];//bit0 ipv4 or ipv6 	,bit1 tcp or udp , bit2 server or client ,bit 3 to bit 7 uart baud
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
#define CONFIG_IPV6 			0x01
#define CONFIG_TCP 				0x02
#define CONFIG_SERVER 			0x04
#define CONFIG_BAUD_115200 		0x08
#define CONFIG_BAUD_460800 		0x10
#define CONFIG_BAUD_921600 		0x20
#define CONFIG_BAUD_2000000 	0x40
#define CONFIG_BAUD_4000000 	0x80
#define CONFIG_BAUD_6000000 	0x88

void socket_send(int index,rt_uint8_t *data,int len);
#define debug 0
#if debug
#define DBG rt_kprintf
#else
#define DBG 
#endif
