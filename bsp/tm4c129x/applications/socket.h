
typedef struct {
	rt_uint8_t local_ip0;
	rt_uint8_t local_ip1;
	rt_uint8_t local_ip2;
	rt_uint8_t local_ip3;
	rt_uint8_t local_port0;
	rt_uint8_t local_port1;
	rt_uint8_t sub_msk0;
	rt_uint8_t sub_msk1;
	rt_uint8_t sub_msk2;
	rt_uint8_t sub_msk3;
	rt_uint8_t gw0;
	rt_uint8_t gw1;
	rt_uint8_t gw2;
	rt_uint8_t gw3;
	rt_uint8_t mac0;
	rt_uint8_t mac1;
	rt_uint8_t mac2;
	rt_uint8_t mac3;
	rt_uint8_t mac4;
	rt_uint8_t mac5;
	rt_uint8_t remote_ip0[4];
	rt_uint8_t remote_ip1[4];
	rt_uint8_t remote_ip2[4];
	rt_uint8_t remote_ip3[4];
	rt_uint8_t remote_port0[4];
	rt_uint8_t remote_port1[4];
	rt_uint8_t protol[4];
	rt_uint8_t server_mode[4];
}config,*pconfig;
typedef struct {
	rt_uint8_t *socket_buf_send[4];/*every buf use 10k*/
	rt_uint8_t *socket_buf_recv[4];
	rt_uint16_t *w_send_index;
	rt_uint16_t *r_send_index;
	rt_uint16_t *w_recv_index;
	rt_uint16_t *r_recv_index;
}ringbuf,*pringbuf;