/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-05     whj4674672   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <fal.h>
#include <stdbool.h>

/* defined the LED1 pin: PB0 */
#define LED1_PIN    GET_PIN(B, 0)
#ifndef BSP_WORKING_AS_APP
#include <stdint.h>
#include "md5.h"

typedef  void (*jump_app)(void);
#define APP_ADDRESS 	0x08040000	//app
#define APP_LEN_ADDR 	0x08020000	//param
#define APP_MD5_ADDR 	0x08100004
#define OTA_ADDRESS		0x08100000
#define SECTOR_SIZE		(128*1024)
jump_app jump;

static void app_boot()
{
	rt_hw_interrupt_disable();
	if (((*(__IO uint32_t*)(APP_ADDRESS + 4)) & 0xFF000000 ) == 0x08000000)
	{
	//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	//	PWR_BackupAccessCmd(ENABLE);
	//	RTC_WriteBackupRegister(RTC_BKP_DR0, 0x00);
	//	PWR_BackupAccessCmd(DISABLE);
		__set_FAULTMASK(1);
		uint32_t JumpAddress = *(__IO uint32_t*) (APP_ADDRESS + 4);
		jump = (jump_app) JumpAddress;
		__set_MSP(*(__IO uint32_t*) APP_ADDRESS);
		jump();
	}
}

bool ota()
{
	/* verify ota zone */
	MD5_CTX md5; 
	uint8_t decrypt[16] = {0};
	uint32_t i;
	uint8_t *ptr = (uint8_t *)(OTA_ADDRESS);
	uint32_t ota_len = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
	uint8_t *md5_sum = (uint8_t *)(OTA_ADDRESS + 4);
	rt_kprintf("app len: %d\n", ota_len);
	if (ota_len == 0xffffffff)
		return false;
	MD5Init(&md5);
	MD5Update(&md5, ptr + 20, ota_len);
	MD5Final(&md5, decrypt);
	for (i = 0; i < 16; i++) {
		//rt_kprintf("md5[%d] %02x %02x\n", i, decrypt[i], md5_sum[i]);
		if (decrypt[i] != md5_sum[i]) {
			//rt_kprintf("ota zone download part fw\n");
			return false;
		}
	}
	/* compare ota zone with app zone */
	ptr = (uint8_t *)(APP_ADDRESS);
	MD5Init(&md5);
	MD5Update(&md5, ptr, ota_len);
	MD5Final(&md5, decrypt);
	for (i = 0; i < 16; i++) {
		//rt_kprintf("app md5[%d] %02x %02x\n", i, decrypt[i], md5_sum[i]);
		if (decrypt[i] != md5_sum[i]) {
			rt_kprintf("need do ota\n");
			break;
		}
	}

	/* cp ota zone to app zone as they are different */
	if (i != 16) {
    	static const struct fal_partition *part_dev = NULL;
		if ((part_dev = fal_partition_find("app")) == NULL) {
			rt_kprintf("can't find app zone");
			return false;
		}
		ptr = (uint8_t *)(OTA_ADDRESS + 20);
		if (fal_partition_erase(part_dev, 0, ota_len) < 0) {
			rt_kprintf("erase app zone failed %d", ota_len);
			return false;
		}
		if (fal_partition_write(part_dev, 0, (const uint8_t *)ptr, ota_len) < 0) {
			rt_kprintf("write to app zone failed %d", ota_len);
			return false;
		}
		/* recheck app zone */	
		ptr = (uint8_t *)(APP_ADDRESS);
		MD5Init(&md5);
		MD5Update(&md5, ptr, ota_len);
		MD5Final(&md5, decrypt);
		for (i = 0; i < 16; i++) {
			//rt_kprintf("recheck app md5[%d] %02x %02x\n", i, decrypt[i], md5_sum[i]);
			if (decrypt[i] != md5_sum[i]) {
				rt_kprintf("recheck app md5 failed\n");
				return false;
			}
		}

		return true;
	} else
		rt_kprintf("no need ota\n");

	return false;
}
#else
#include "infra_compat.h"
#include "mqtt_api.h"
#include "ota_api.h"
/*void get_cur_ver()
{
    uint8_t ver[128] = {0};
    static const struct fal_partition *param_dev = NULL;
	if ((param_dev = fal_partition_find("param")) == NULL) {
		rt_kprintf("can't find param zone %d\n", __LINE__);
		return;
	}

	fal_partition_read(param_dev, 0, ver, 128);
	if (ver[0] != 'a' || ver[1] != 'p' || ver[2] != 'p')
		strcpy((char *)ver, "app-1.0.1-20220523.0000");

	rt_kprintf("111 cur version: %s\n", ver);
}*/
static rt_device_t mcu_hid_device;
#define MSGID_OFS			15
#define PAYLOAD_OFS			22
#define LEN_OFS				5
#define CRC_OFS				1
#define TS_OFS				7
#define RESERVED_OFS		17
#define HOST_CMD 0
enum {
	MSG_R_PRODUCT_KEY = 1,
	MSG_W_PRODUCT_KEY,
	MSG_R_DEV_NAME,
	MSG_W_DEV_NAME,
	MSG_R_DEV_SECRET,
	MSG_W_DEV_SECRET,
	NR_MAX
};
enum McuErrCode {
    ERR_CODE_SUCCESS = 0,
    ERR_CODE_FAILURE = 1,
    ERR_CODE_INVALID_ARGUMENT = 2,
    ERR_CODE_NOT_ENOUGH_MEMORY = 3,
    ERR_CODE_UNSUPPORTED_CMD = 4,
    ERR_CODE_CRC_MISMATCH = 5,
    ERR_CODE_VER_MISMATCH = 6,
    ERR_CODE_MSG_ID_MISMATCH = 7,
    ERR_CODE_MSG_STX_MISMATCH = 8,
    ERR_CODE_NOT_WRITTEN = 9,
};

static const uint32_t CRC32_table[256] = {
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
	0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
	0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
	0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
	0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
	0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
	0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
	0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
	0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
	0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
	0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
	0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
	0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
	0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
	0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
	0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
	0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
	0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
	0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
	0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
	0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
	0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
	0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
	0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
	0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
	0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
	0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
	0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
	0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
	0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
	0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
	0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
	0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
	0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
	0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
	0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
	0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
	0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
	0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
	0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
	0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
	0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
	0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
	0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
	0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
	0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
	0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
	0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
	0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

uint32_t cmd_crc(uint8_t* buf, uint32_t len)
{
	uint32_t CRC32_data = 0xFFFFFFFF;
	for (uint32_t i = 0; i != len; ++i) {
		uint32_t t = (CRC32_data ^ buf[i]) & 0xFF;
		CRC32_data = ((CRC32_data >> 8) & 0xFFFFFF) ^ CRC32_table[t];
	}

	return ~CRC32_data;
}

typedef uint16_t (*mcu_func)( uint8_t *in, uint16_t in_len, uint8_t *out);
#define MAX_STRING_LEN	41
#define MCU_ERR_SUCCESS	0x00
#define MCU_ERR_FAILE	0x01
#define MCU_ERR_INV_ARG 0x02
#define MCU_ERR_NO_MEM 0x03
#define MCU_ERR_UNSUPPORT 0x04
#define MCU_ERR_CRC	0x05
#define PAYLOAD_LEN_BASE 1	//err+payload, 0 + uint32_t
#define DEFAULT_PARAM_LEN	512//4096
static uint8_t *local_buf = RT_NULL;
static int debug = 0;
#define PARAM_FW_VER			0x01
#define PARAM_PRODUCT_KEY		(PARAM_FW_VER + 64)
#define PARAM_DEV_NAME			(PARAM_PRODUCT_KEY + 64)
#define PARAM_DEV_SECRET		(PARAM_DEV_NAME + 64)

void param_get(uint32_t ofs, uint8_t *buf, uint32_t len)
{
	if (ofs + len > DEFAULT_PARAM_LEN)
		return;

	rt_memcpy(buf, local_buf + ofs, len);
}

void param_set_once(uint32_t ofs, uint8_t *buf, uint32_t len)
{
	if (ofs + len > DEFAULT_PARAM_LEN)
		return;

	if (rt_memcmp(buf, local_buf + ofs, len) == 0)
		return;

	rt_memcpy(local_buf + ofs, buf, len);
}

void param_set(uint32_t ofs, uint8_t *buf, uint32_t len)
{
	int t_len;
	if (ofs + len > DEFAULT_PARAM_LEN)
		return;

	if (rt_memcmp(buf, local_buf + ofs, len) == 0)
		return;

	rt_kprintf("0fw ver: %s\n", local_buf + PARAM_FW_VER);
	rt_kprintf("0product key: %s\n", local_buf + PARAM_PRODUCT_KEY);
	local_buf[ofs - 1] = 0xaa;
	rt_memcpy(local_buf + ofs, buf, len);

	rt_kprintf("fw ver: %s\n", local_buf + PARAM_FW_VER);
	rt_kprintf("product key: %s\n", local_buf + PARAM_PRODUCT_KEY);
	rt_kprintf("dev name: %s\n", local_buf + PARAM_DEV_NAME);
	rt_kprintf("dev secret: %s\n", local_buf + PARAM_DEV_SECRET);
	static const struct fal_partition *param_dev = NULL;
	if ((param_dev = fal_partition_find("param")) == NULL) {
		rt_kprintf("can't find param zone %d\n", __LINE__);
		return;
	}
    if (fal_partition_erase(param_dev, 0, 128*1024) < 0)
	    rt_kprintf("erase param zone failed");
    if (fal_partition_write(param_dev, 0, (const uint8_t *)local_buf,
    						DEFAULT_PARAM_LEN) < 0)
    	rt_kprintf("write to param zone failed");
}

void param_init()
{
	int t_len;

	local_buf = (uint8_t *)rt_malloc(DEFAULT_PARAM_LEN*sizeof(uint8_t));
	if (local_buf == RT_NULL) {
		rt_kprintf("Init param failed, not enough memory\r\n");
		return;
	}
    static const struct fal_partition *param_dev = NULL;
	if ((param_dev = fal_partition_find("param")) == NULL) {
		rt_kprintf("can't find param zone %d\n", __LINE__);
		return;
	}

	fal_partition_read(param_dev, 0, local_buf, DEFAULT_PARAM_LEN);
}

static uint16_t _handle_info(uint16_t ofs, uint8_t *in, uint16_t in_len,
							 uint8_t *out)
{
	uint8_t writed = 0x00;
	uint8_t info[MAX_STRING_LEN] = {0x00};
	out[0] = MCU_ERR_SUCCESS;

	if (in != NULL && in_len > 0) {
		rt_memcpy(info, in,
				(in_len >= MAX_STRING_LEN) ? MAX_STRING_LEN : in_len);
		param_set(ofs, info, MAX_STRING_LEN);
	} else if (out != NULL) {
		param_get(ofs - 1, (uint8_t *)&writed, 1);
		if (writed == 0xaa) {
			param_get(ofs, info, MAX_STRING_LEN);
			rt_memcpy(out + 1, info, rt_strlen((const char *)info));
			return PAYLOAD_LEN_BASE + rt_strlen((const char *)info);
		} else {
			out[0] = ERR_CODE_NOT_WRITTEN;
			return PAYLOAD_LEN_BASE;
		}
	}

	return PAYLOAD_LEN_BASE;
}

uint16_t handle_r_product_key(uint8_t *in, uint16_t in_len, uint8_t *out) //0x01
{
	rt_kprintf("%s %d", __func__, __LINE__);
	return _handle_info(PARAM_PRODUCT_KEY, NULL, 0, out);
}

uint16_t handle_w_product_key(uint8_t *in, uint16_t in_len, uint8_t *out) //0x02
{
	rt_kprintf("%s %d", __func__, __LINE__);
	return _handle_info(PARAM_PRODUCT_KEY, in, in_len, out);
}

uint16_t handle_r_dev_name(uint8_t *in, uint16_t in_len, uint8_t *out) //0x03
{
	rt_kprintf("%s %d", __func__, __LINE__);
	return _handle_info(PARAM_DEV_NAME, NULL, 0, out);
}

uint16_t handle_w_dev_name(uint8_t *in, uint16_t in_len, uint8_t *out) //0x04
{
	rt_kprintf("%s %d", __func__, __LINE__);
	return _handle_info(PARAM_DEV_NAME, in, in_len, out);
}

uint16_t handle_r_dev_secret(uint8_t *in, uint16_t in_len, uint8_t *out) //0x05
{
	rt_kprintf("%s %d", __func__, __LINE__);
	return _handle_info(PARAM_DEV_SECRET, NULL, 0, out);
}

uint16_t handle_w_dev_secret(uint8_t *in, uint16_t in_len, uint8_t *out) //0x06
{
	rt_kprintf("%s %d", __func__, __LINE__);
	return _handle_info(PARAM_DEV_SECRET, in, in_len, out);
}

mcu_func g_mcu_func[NR_MAX] = {
	NULL,
	&handle_r_product_key,
	&handle_w_product_key,
	&handle_r_dev_name,
	&handle_w_dev_name,
	&handle_r_dev_secret,
	&handle_w_dev_secret
};

void get_cur_ver()
{
    uint8_t ver[128] = {0};
#if 0
    static const struct fal_partition *param_dev = NULL;
	if ((param_dev = fal_partition_find("param")) == NULL) {
		rt_kprintf("can't find param zone %d\n", __LINE__);
		return;
	}

	fal_partition_read(param_dev, 0, ver, 128);
	if (ver[0] != 'a' || ver[1] != 'p' || ver[2] != 'p')
		strcpy((char *)ver, "app-1.0.1-20220523.0000");
#else
	param_init();
	param_get(PARAM_FW_VER, ver, 128);
#endif
	rt_kprintf("cur version: %s\n", ver);
}

void dump(uint8_t *tag, uint8_t *cmd, uint32_t len)
{
	uint32_t i;
	if (debug) {
		rt_kprintf("\n==========> %s [%d]: \n", tag, len);
		for (i = 0; i < len; i++) {
			if (i % 16 == 0 && i != 0)
				rt_kprintf("\n");
			rt_kprintf("%02x ", cmd[i]);
		}
		if ((len % 16) != 0)
			rt_kprintf("\n");
		rt_kprintf("<==========\r\n");
	}
}

static uint16_t handle_mcu_proto(uint16_t msg_id, uint8_t *cmd, uint16_t cmd_len,
								 uint8_t *rsp)
{
	uint16_t payload_len = 1;

	rsp[0] = ERR_CODE_UNSUPPORTED_CMD;

	if (msg_id > NR_MAX)
		return payload_len;

	if (g_mcu_func[msg_id]) {
		rsp[0] = ERR_CODE_SUCCESS;
		payload_len = g_mcu_func[msg_id](cmd, cmd_len, rsp);
	}

	return payload_len;
}

static uint16_t parse_host_cmd(uint8_t *cmd, uint16_t len, uint8_t *msg)
{
	/* protocol
	 *
	 * STX		CRC		LEN		REQUEST ID		TIMESTAMP		MSG ID		RESERVED	Payload
	 * 0xFD	uint32_t uint16_t   uint32_t		uint32_t(ms)	uint16_t	uint8_t[5]	0-42 bytes
	 *
	 * LEN (LEN -> Payload)
	 * CRC (LEN -> Payload)
	 */

	uint32_t crc, i;
	uint16_t payload_len;
	uint16_t _len = 0;

	if (len < 21) {
		rt_kprintf("[%d] invalid len %d", __LINE__, len);
		msg[0] = ERR_CODE_INVALID_ARGUMENT;
		goto POS;
	}

	_len = (cmd[LEN_OFS] << 8) | cmd[LEN_OFS - 1];

	crc = cmd[CRC_OFS - 1] | (cmd[CRC_OFS] << 8) |
		(cmd[CRC_OFS + 1] << 16) | (cmd[CRC_OFS + 2] << 24);

	if (crc != cmd_crc((uint8_t *)(cmd + LEN_OFS - 1), _len)) {
		msg[0] = ERR_CODE_CRC_MISMATCH;
		rt_kprintf("invalid crc h %x != c %x", crc,
				cmd_crc((uint8_t *)(cmd + LEN_OFS - 1), _len));
		goto POS;
	}

POS:
	msg[1] = HOST_CMD;
	if (msg[0] == 0) {
		msg[2] = cmd[MSGID_OFS - 1];
		msg[3] = cmd[MSGID_OFS];
		rt_memcpy(msg + 6, cmd + RESERVED_OFS -1, 5);
		rt_memcpy(msg + 11, cmd + TS_OFS -1, 4);	//request id
		payload_len = _len - 17;
		msg[4] = (payload_len >> 8) & 0xff;
		msg[5] = payload_len & 0xff;

		if (payload_len <= 42) {
			rt_memcpy(msg + 15, cmd + PAYLOAD_OFS - 1, payload_len);
			return payload_len + 15;
		} else {
			msg[0] = ERR_CODE_INVALID_ARGUMENT;
			rt_kprintf("[%d] invalid payload len %d", __LINE__, payload_len);
			return 2;
		}
	} else {
		return 2;
	}
}

static void mcu_msg_handler(uint8_t *msg)
{
	/* protocol
	 *
	 * STX		CRC		LEN		REQUEST ID		TIMESTAMP		MSG ID		RESERVED	Payload
	 * 0xFD	uint32_t uint16_t   uint32_t		uint32_t(ms)	uint16_t	uint8_t[5]	0-42 bytes
	 *
	 * LEN (LEN -> Payload)
	 * CRC (LEN -> Payload)
	 */
	uint8_t rsp[64] = {0};
	uint8_t err = msg[0];
	uint8_t *cmd = (uint8_t *)(msg + 15);
	uint16_t out_payload_len = 0, len, msg_id;
	uint16_t cmd_len = (msg[4] << 8) | msg[5];
	uint32_t crc;
	//uint64_t ts = read_ts();

	rsp[0] = 0xfd;					//stx
	memcpy(rsp + TS_OFS, msg + 11, 4);		//request id
	//read_ts(rsp + TS_OFS + 4, rsp + TS_OFS + 8); 				//ts
	//memcpy(rsp + TS_OFS + 4, (uint8_t *)&ts, 4);
	msg_id = (msg[3] << 8) | msg[2];
	rsp[MSGID_OFS] = msg[2]; 				//msgid
	rsp[MSGID_OFS + 1] = msg[3];			//msgid
	memcpy(rsp + RESERVED_OFS, msg + 6, 5);	//reserved

	if (err == MCU_ERR_SUCCESS) {
		out_payload_len = handle_mcu_proto(msg_id, cmd, cmd_len, rsp + 22);
	} else {
		rsp[23] = err;
		out_payload_len = 2;
	}

	len = 17 + out_payload_len;
	rsp[LEN_OFS] = (len >> 0) & 0xff;
	rsp[LEN_OFS + 1] = (len >> 8) & 0xff;

	crc = cmd_crc((uint8_t *)(rsp + LEN_OFS), len);
	rsp[CRC_OFS + 3]   = (crc >> 24) & 0xff;
	rsp[CRC_OFS + 2] = (crc >> 16) & 0xff;
	rsp[CRC_OFS + 1] = (crc >> 8) & 0xff;
	rsp[CRC_OFS + 0] = (crc >> 0) & 0xff;

	if (err != MCU_ERR_SUCCESS) {
		for (int i = 0; i < len + 5; i++)
			rt_kprintf("%02x ", rsp[i]);
	}

	dump("dev out", rsp, len + 5);
	if (out_payload_len > 1)
		rt_kprintf("out: %s\n", rsp + 23);
	rt_device_write(mcu_hid_device, 0xfd, rsp + 1, len + 4);
}

void HID_Report_Received(hid_report_t report)
{
	uint8_t msg[64] = {0};

    dump("ori host", report->report, report->size);
    if (report->report_id != 0xfd) {
    	rt_kprintf("unknown packet");
    	return;
	}

	if (parse_host_cmd(report->report, report->size, msg) >= 15) {
		mcu_msg_handler(msg);
	}
}

static int mcu_hid_init(void)
{
	int err = 0;
	mcu_hid_device = rt_device_find("hidd");

	RT_ASSERT(mcu_hid_device != RT_NULL);

	err = rt_device_open(mcu_hid_device, RT_DEVICE_FLAG_RDWR);

	if (err != RT_EOK)
	{
		rt_kprintf("open dev failed!");
		return -1;
	}
	return 0;
}
#endif

#define L601_W_DIS	GET_PIN(C, 5)
#define L601_RST_EN	GET_PIN(B, 0)
#define L601_PWR_EN	GET_PIN(B, 1)
#define L601_POW_EN	GET_PIN(B, 2)
#define L601_DTR	GET_PIN(E, 7)
#define L601_CTL_EN	GET_PIN(E, 8)
extern int radar_init();
extern void *pclient;
extern int get_ota_len();
extern void mcu_ota();
int main(void)
{
	uint32_t size_file;
    /* set LED1 pin mode to output */
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
	fal_init();
#ifndef BSP_WORKING_AS_APP
	if (ota())
		rt_kprintf("ota success\n");
	app_boot();
#else
	get_cur_ver();
	mcu_hid_init();
	radar_init();
#endif
    while (1)
    {
      //  rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
      //  rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
#ifdef BSP_WORKING_AS_APP
    if (pclient) {
	    IOT_MQTT_Yield(pclient, 200);
	    if (get_ota_len() > 0) {
	    	mcu_ota();
		}
	}
#endif
	//rt_kprintf("dis %d, rst %d, pwr %d, pow %d, dtr %d, ctl %d\n",
	//			L601_W_DIS, L601_RST_EN, L601_PWR_EN,
	//			L601_POW_EN, L601_DTR, L601_CTL_EN);
    }
}
