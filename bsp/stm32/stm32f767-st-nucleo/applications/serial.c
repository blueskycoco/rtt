#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
//#include <netinet/in.h>
//#include <linux/netlink.h>
//#include <sys/epoll.h>
#include <errno.h>
#include <hidapi/hidapi.h>
#include "usb.h"

bool debug = false;

bool poll_device(char *path)
{
	struct hid_device_info *devs, *cur_dev;
	int timeout = 0;

	do {
		devs = hid_enumerate(MY_VID, 0x0);
		cur_dev = devs;	
		while (cur_dev) {
			printf("Device Found\n  type: %04hx %04hx\n",
					cur_dev->vendor_id, cur_dev->product_id);
			printf("\n");
			printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
			printf("  Product:      %ls\n", cur_dev->product_string);
			printf("  Release:      %hx\n", cur_dev->release_number);
			printf("  Interface:    %d\n",  cur_dev->interface_number);
			printf("\n");
			if (cur_dev->product_id == MY_PID) {
				if (path)
					strcpy(path, cur_dev->path);
				return true;
			}
			cur_dev = cur_dev->next;
		}

		hid_free_enumeration(devs);

		sleep(1);
	} while (true);

	return true;
}

void normal_cmd(hid_device *hid_handle, char *param_1, char *param_2)
{
	uint16_t msgid;
	uint8_t *payload = NULL;
	uint8_t payload_len = 0;
	uint8_t packet[64] = {0};
	uint16_t packet_len, rsp_len;
	uint8_t response[64] = {0};
	uint8_t rsp[64] = {0};
	int act_len = 0, i;

	if (hid_handle == NULL || param_1 == NULL) {
		printf("[%d ]invalid param", __LINE__);
		return;
	}

	if (strstr(param_1, "dev_name"))
		msgid =	MSG_R_DEV_NAME;
	else if (strstr(param_1, "dev_secret"))
		msgid = MSG_R_DEV_SECRET;
	else if (strstr(param_1, "product_key"))
		msgid = MSG_R_PRODUCT_KEY;
	else {
		printf("[%d] unsupport cmd %s\n", __LINE__, param_1);
		return;
	}

	if (param_2) {
		msgid = msgid + 1;
		cmd_build(msgid, (uint8_t *)param_2, strlen((char *)param_2), packet,
				&packet_len);
	} else {
		cmd_build(msgid, NULL, 0, packet, &packet_len);
	}
	printf("[%ld] cmd %s\n", ts(), param_1);
	act_len = hid_write(hid_handle, packet, packet_len);
	printf("[%ld] send finish\n", ts());
	act_len = hid_read_timeout(hid_handle, response, 64, 1000);
	if (act_len > 0) {
		parse_rsp(response, act_len, rsp, &rsp_len, &msgid);
		printf("[%ld] result %s\n", ts(), rsp);
	} else {
		printf("[%ld] timeout\n", ts());
	}	
}

int main(int argc, char** argv)
{
	hid_device *hid_handle;
	char hid_path[256];
	bool result = false;
	int i = 0;

	for (i = 0; i < argc; i++) {
		if (strstr(argv[i], "-v") != NULL)
			debug = true;
		printf("argv[%d] %s\n", i, argv[i]);
	}

	if (argc < 2) {
		printf("uasge: ./radar {opt} [-v]\r\n"
			" -v: verbose log\r\n"
			"opt: \r\n"
			"\tdev_name:\t\tset/get device name\r\n"
			"\tdev_secret:\t\tset/get device secret\r\n"
			"\tproduct_key:\t\tset/get product key\r\n"
		);
		return 0;
	}

	if (hid_init())
		return -1;

	result = poll_device(hid_path);
	if (!result) {
		printf("can't find device\n");
		goto END;
	}

	hid_handle = hid_open_path(hid_path);
	if (hid_handle == NULL) {
		printf("can't open usb handle\n");
		goto END;
	}

	if (argc == 4 || (argc == 3 && strstr(argv[2], "-v") == NULL))
		normal_cmd(hid_handle, argv[1], argv[2]);
	else
		normal_cmd(hid_handle, argv[1], NULL);

	hid_close(hid_handle);

END:
	hid_exit();
	return 0;
}

