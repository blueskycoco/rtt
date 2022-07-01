#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#define MY_VID      0x0011
#define MY_PID		0x0022

#define INTF_BOOT   0
#define INTF_APP    4
#define INTF_RK		3

enum {
	MSG_R_PRODUCT_KEY = 1,
	MSG_W_PRODUCT_KEY,
	MSG_R_DEV_NAME,
	MSG_W_DEV_NAME,
	MSG_R_DEV_SECRET,
	MSG_W_DEV_SECRET,
	NR_MAX
};
void cmd_build(uint16_t msgid, uint8_t *payload, uint16_t payload_len,
			   uint8_t *cmd, uint16_t *len);

void parse_rsp(uint8_t *rsp, uint16_t len, uint8_t *payload,
			   uint16_t* payload_len, uint16_t *msgid);
long ts();
uint32_t cmd_crc(uint8_t* buf, uint32_t len);
#endif
