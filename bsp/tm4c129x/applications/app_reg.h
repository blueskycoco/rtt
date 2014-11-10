#include <rtthread.h>
#include <board.h>
#include <components.h>

typedef struct {
	rt_uint8_t mode;			/*0x00*/
	rt_uint8_t gwip0;			/*0x01 to 0x04*/
	rt_uint8_t gwip1;
	rt_uint8_t gwip2;
	rt_uint8_t gwip3;
	rt_uint8_t sub_msk0;	/*0x05 to 0x08*/;
	rt_uint8_t sub_msk1;
	rt_uint8_t sub_msk2;
	rt_uint8_t sub_msk3;
	rt_uint8_t local_mac0;	/*0x09 to 0x0e*/
	rt_uint8_t local_mac1;
	rt_uint8_t local_mac2;
	rt_uint8_t local_mac3;
	rt_uint8_t local_mac4;
	rt_uint8_t local_mac5;
	rt_uint8_t local_ip0;	/*0x0f to 0x12*/
	rt_uint8_t local_ip1;
	rt_uint8_t local_ip2;
	rt_uint8_t local_ip3;
	rt_uint8_t int_Ir;	/*0x15*/
	rt_uint8_t int_Imr;	/*0x16*/
	rt_uint8_t rty_val0;	/*0x17*/
	rt_uint8_t rty_val1;	/*0x18*/
	rt_uint8_t rty_cnt;	/*0x19*/
	rt_uint8_t rx_mem_size;	/*0x1a*/
	rt_uint8_t tx_mem_size;	/*0x1b*/
	rt_uint8_t un_reach_ip0;/*0x2a*/
	rt_uint8_t un_reach_ip1;/*0x2b*/
	rt_uint8_t un_reach_ip2;/*0x2c*/
	rt_uint8_t un_reach_ip3;/*0x2d*/
	rt_uint8_t un_reach_port0;/*0x2e*/
	rt_uint8_t un_reach_port1;	/*0x2f*/
}tm4c_lwip_common_reg,*ptm4c_lwip_common_reg;
typedef struct {
	rt_uint8_t mode;		/*0x00*/
	rt_uint8_t cmd;		/*0x01 to 0x04*/
	rt_uint8_t ir;
	rt_uint8_t sr;
	rt_uint8_t s_port0;
	rt_uint8_t s_port1;	/*0x05 to 0x08*/;
	rt_uint8_t d_mac0;
	rt_uint8_t d_mac1;
	rt_uint8_t d_mac2;
	rt_uint8_t d_mac3;	/*0x09 to 0x0e*/
	rt_uint8_t d_mac4;
	rt_uint8_t d_mac5;
	rt_uint8_t d_ip0;
	rt_uint8_t d_ip1;
	rt_uint8_t d_ip2;
	rt_uint8_t d_ip3;	/*0x0f to 0x12*/
	rt_uint8_t d_port0;
	rt_uint8_t d_port1;
	rt_uint8_t max_seg_size0;
	rt_uint8_t max_seg_size1;	/*0x15*/
	rt_uint8_t ip_proto;	/*0x16*/
	rt_uint8_t type_sevice;	/*0x17*/
	rt_uint8_t ip_time_live;	/*0x18*/
	rt_uint8_t tx_free_size;	/*0x19*/
	rt_uint8_t tx_read_index0;	/*0x1a*/
	rt_uint8_t tx_read_index1;	/*0x1b*/
	rt_uint8_t tx_write_index0;/*0x2a*/
	rt_uint8_t tx_write_index1;/*0x2b*/
	rt_uint8_t rx_size0;/*0x2c*/
	rt_uint8_t rx_size1;/*0x2d*/
	rt_uint8_t rx_read_index0;/*0x2e*/
	rt_uint8_t rx_read_index1;/*0x2f*/
}tm4c_lwip_socket_reg,*ptm4c_lwip_socket_reg;

typedef struct{
	tm4c_lwip_socket_reg socket[4];
	tm4c_lwip_common_reg common;
}lwip_app,*plwip_app;
ALIGN(RT_ALIGN_SIZE)
lwip_app g_lwip_app;