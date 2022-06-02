#include <rtthread.h>
#include <rtdevice.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#ifdef BSP_WORKING_AS_APP
#include <fal.h>
#include "cJSON.h"
#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "infra_compat.h"
#include "ota_api.h"
#if defined(RT_USING_SAL)
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#endif
#if defined(RT_USING_NETDEV)
#include <netdev.h>
#elif defined(RT_USING_LWIP)
#include <lwip/netif.h>
#endif /* RT_USING_NETDEV */

static struct rt_semaphore rx_sem;
static rt_device_t serial;
static uint8_t mb_radar[2048] = {0};
static struct rt_mailbox mb;
rt_bool_t net_ok = RT_FALSE;
void                   *pclient = NULL;
void *h_ota = NULL;
static char DEMO_PRODUCT_KEY[IOTX_PRODUCT_KEY_LEN + 1] = {0};
static char DEMO_DEVICE_NAME[IOTX_DEVICE_NAME_LEN + 1] = {0};
static char DEMO_DEVICE_SECRET[IOTX_DEVICE_SECRET_LEN + 1] = {0};
static int example_publish(void *handle, char *payload);
void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
void HAL_Printf(const char *fmt, ...);
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1]);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1]);
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN]);
uint64_t HAL_UptimeMs(void);
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

#define OTA_MQTT_MSGLEN         (2048)
#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

static void example_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_t     *topic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            /* print topic name and topic message */
            //EXAMPLE_TRACE("Message Arrived:");
            //EXAMPLE_TRACE("Topic  : %.*s", topic_info->topic_len, topic_info->ptopic);
            EXAMPLE_TRACE("rcv: %.*s", topic_info->payload_len, topic_info->payload);
            //EXAMPLE_TRACE("\n");
            break;
        default:
            break;
    }
}

static int example_subscribe(void *handle)
{
    int res = 0;
    const char *fmt = "/%s/%s/user/get";
    char *topic = NULL;
    int topic_len = 0;

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Subscribe(handle, topic, IOTX_MQTT_QOS0, example_message_arrive, NULL);
    if (res < 0) {
        EXAMPLE_TRACE("subscribe failed");
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&rx_sem);
	return RT_EOK;
}

void build_json(uint8_t *data, uint8_t **str)
{
    cJSON *pJson;
    char array_num[32] = {0};
    int i = 0;
    time_t time_p;
    struct tm *tmp_ptr;

    pJson = cJSON_CreateObject();
    if (!pJson) {
        rt_kprintf("can't create obj %d\n", __LINE__);
        return;
    }

	time(&time_p);
	tmp_ptr = localtime(&time_p);
	if (tmp_ptr->tm_year == 100)
		ntp_sync_to_rtc(RT_NULL);
	rt_sprintf(array_num, "%04d-%02d-%02d %02d:%02d:%02d",
			1900 + tmp_ptr->tm_year,
			1 + tmp_ptr->tm_mon,
			tmp_ptr->tm_mday,
			tmp_ptr->tm_hour, tmp_ptr->tm_min, tmp_ptr->tm_sec);
	cJSON_AddNumberToObject(pJson, "deviceid", *(uint32_t*)(0x1FF1E800) << 0 |
			*(uint32_t*)(0x1FF1E800 + 4) << 8 |
			*(uint32_t*)(0x1FF1E800 + 8) << 16);
	cJSON_AddStringToObject(pJson, "timestamp", array_num);
	cJSON_AddNumberToObject(pJson, "targetnum", data[4]);
    for (i = 0; i < data[4]; i++) {
    	cJSON *tmp = cJSON_CreateObject();
		cJSON_AddNumberToObject(tmp, "targetid", data[5 + i*20]);
		if (data[13 + i*20] == 0 || data[14 + i*20] == 0)
			cJSON_AddNumberToObject(tmp, "vsvalid", 0);
		else
			cJSON_AddNumberToObject(tmp, "vsvalid", 1);
		cJSON_AddNumberToObject(tmp, "heartrate", data[14 + i*20]);
		cJSON_AddNumberToObject(tmp, "breathrate", data[13 + i*20]);
		int16_t x = (int16_t)((data[7 + i*20] << 8) | data[8 + i*20]);
		float _x = x / 100.00;
		sprintf(array_num, "%.2f", _x);
		cJSON_AddStringToObject(tmp, "positionx", array_num);
		int16_t y = (data[9 + i*20] << 8) | data[10 + i*20];
		float _y = y / 100.00;
		sprintf(array_num, "%.2f", _y);
		cJSON_AddStringToObject(tmp, "positiony", array_num);
		cJSON_AddNumberToObject(tmp, "fdstate", data[12 + i*20]);
		rt_sprintf(array_num, "%d", i);
		cJSON_AddItemToObject(pJson, array_num, tmp);
	}
    
    *str = cJSON_PrintUnformatted(pJson);
//    rt_kprintf("json is: %s\n", *str);
    cJSON_Delete(pJson);
}

static void serial_thread_entry(void *parameter)
{
	char ch;
	uint8_t *str;
	char buf[1024] = {0};
	int cnt = 0, i;

	while (RT_TRUE) {
		while (rt_device_read(serial, -1, &ch, 1) != 1) {
			//rt_kprintf("\ncnt is %d\n", cnt);
			if (cnt > 0 &&
					buf[0] == 0xfa && buf[1] == 0x5a &&
					buf[2] == 0xfa && buf[3] == 0x5a &&
					buf[cnt - 4] == 0xfa && buf[cnt - 3] == 0x6a &&
					buf[cnt - 2] == 0xfa && buf[cnt - 1] == 0x6a) {
#if 0
				for (i = 0; i < cnt; i++) {
					rt_kprintf("%02x ", buf[i]);
				}
				rt_kprintf(" [%d]\r\n", cnt);
				rt_kprintf("targetnum\t: %d\n", buf[4]);
				for (i = 0; i < buf[4]; i++) {
					rt_kprintf("_targetnum[%d]\t: %d\n", i, buf[5 + i*20]);
					rt_kprintf("targetid[%d]\t: %d\n", i, buf[6 + i*20]);
					rt_kprintf("positionx[%d]\t: %d\n", i, (int16_t)((buf[7 + i*20] << 8) | buf[8 + i*20]));
					rt_kprintf("positiony[%d]\t: %d\n", i, (buf[9 + i*20] << 8) | buf[10 + i*20]);
					rt_kprintf("high[%d]\t: %d\n", i, buf[11 + i*20]);
					rt_kprintf("fdstate[%d]\t: %d\n", i, buf[12 + i*20]);
					rt_kprintf("breathrate[%d] : %d\n", i, buf[13 + i*20]);
					rt_kprintf("heartrate[%d]\t: %d\n", i, buf[14 + i*20]);
					if (buf[13 + i*20] == 0 || buf[14 + i*20] == 0)
						rt_kprintf("vsvalid[%d]\t: 0\n", i);
					else
						rt_kprintf("vsvalid[%d]\t: 1\n", i);
				}
				rt_kprintf("\r\n\r\n");
#endif
				build_json(buf, &str);
				if (net_ok) {
					if (buf[4] > 0) {
						if (RT_EOK != rt_mb_send(&mb, (rt_uint32_t)str)) {
    						cJSON_free((void *)str);
							rt_kprintf("send to net failed\n");
						}
					} else {
    					cJSON_free((void *)str);
					}
				} else {
    				cJSON_free((void *)str);
				}
				cnt = 0;
			} else if (cnt > 50)
				cnt = 0;
			rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
		}
		if (cnt < 1024)
			buf[cnt++] = ch;
	}
}

static void net_thread_entry(void *parameter)
{
	rt_uint32_t addr;
	while (RT_TRUE) {
		if (rt_mb_recv(&mb, (rt_ubase_t *)&addr, RT_WAITING_FOREVER) == RT_EOK) {
            	EXAMPLE_TRACE("send: %.*s", strlen((char *)addr), addr);
    			//rt_kprintf("send[%x]: %s\n", addr, (char *)addr);
    			if (net_ok) {
    				example_publish(pclient, (char *)addr);
        			IOT_MQTT_Yield(pclient, 200);
				}
    			cJSON_free((void *)addr);
		}
	}
}

static int example_publish(void *handle, char *payload)
{
    int             res = 0;
    const char     *fmt = "/%s/%s/user/get";
    char           *topic = NULL;
    int             topic_len = 0;
//    char           *payload = "{\"message\":\"hello!\"}";

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
    if (res < 0) {
        EXAMPLE_TRACE("publish failed, res = %d", res);
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}
static void example_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    EXAMPLE_TRACE("msg->event_type : %d", msg->event_type);
}

int get_ota_len()
{
	uint32_t size_file = 0;
	if (h_ota)
		if (IOT_OTA_IsFetching(h_ota)) {
			IOT_OTA_Ioctl(h_ota, IOT_OTAG_FILE_SIZE, &size_file, 4);
			rt_kprintf("ota file len %d\n", size_file);
			net_ok = RT_FALSE;
		}

	return size_file;
}

void mcu_ota()
{
#define OTA_BUF_LEN        (5000)

    int rc = 0, ota_over = 0;
    iotx_mqtt_param_t mqtt_params;
    char *msg_buf = NULL, *msg_readbuf = NULL, ver[128] = {0};
    char buf_ota[OTA_BUF_LEN];
    static const struct fal_partition *part_dev = NULL, *param_dev = NULL;
	if ((part_dev = fal_partition_find("ota")) == NULL) {
		EXAMPLE_TRACE("can't find ota zone");
		return;
	}
	if ((param_dev = fal_partition_find("param")) == NULL) {
		EXAMPLE_TRACE("can't find param zone");
		return;
	}
	char *ptr = 0x30000000;
	char *param = (char *)HAL_Malloc(10 * 1024);
	if (param == NULL) {
		EXAMPLE_TRACE("can not get param sector size");
		return;
	}
	fal_partition_read(param_dev, 0, param, 10*1024);
    do {
        uint32_t firmware_valid;

        EXAMPLE_TRACE("wait ota upgrade command....");

        /* handle the MQTT packet received from TCP or SSL connection */
        IOT_MQTT_Yield(pclient, 200);

        if (IOT_OTA_IsFetching(h_ota)) {
            uint32_t last_percent = 0, percent = 0;
            char md5sum[33];
            char version[128] = {0};
            int len;
            uint32_t size_downloaded, size_file, ofs = 0;
            IOT_OTA_Ioctl(h_ota, IOT_OTAG_FILE_SIZE, &size_file, 4);
            EXAMPLE_TRACE("get fw size: %d", size_file);
            if (fal_partition_erase(part_dev, 0, size_file) < 0)
            	EXAMPLE_TRACE("erase ota zone failed %d", ofs);
            do {
				len = IOT_OTA_FetchYield(h_ota, ptr, 256*1024 + 1, 1);
                if (len > 0) {
                	EXAMPLE_TRACE("get fw len: %d, ofs %d", len, ofs);
					if (fal_partition_write(part_dev, ofs, (const uint8_t *)ptr, len) < 0)
						EXAMPLE_TRACE("write to ota zone failed %d", ofs);
					else
						EXAMPLE_TRACE("write fw at ofs %d, len %d ok", ofs, len);
					ofs += len;
                } else {
                    IOT_OTA_ReportProgress(h_ota, IOT_OTAP_FETCH_FAILED, NULL);
                    EXAMPLE_TRACE("ota fetch fail");
                	rt_hw_cpu_reset();
                }

                /* get OTA information */
                IOT_OTA_Ioctl(h_ota, IOT_OTAG_FETCHED_SIZE, &size_downloaded, 4);
                IOT_OTA_Ioctl(h_ota, IOT_OTAG_FILE_SIZE, &size_file, 4);
                IOT_OTA_Ioctl(h_ota, IOT_OTAG_MD5SUM, md5sum, 33);
                IOT_OTA_Ioctl(h_ota, IOT_OTAG_VERSION, version, 128);

                last_percent = percent;
                percent = (size_downloaded * 100) / size_file;
                if (percent - last_percent > 0) {
                    //IOT_OTA_ReportProgress(h_ota, percent, NULL);
                    //IOT_OTA_ReportProgress(h_ota, percent, "hello");
                }
                IOT_MQTT_Yield(pclient, 100);
            } while (!IOT_OTA_IsFetchFinish(h_ota));

            IOT_OTA_Ioctl(h_ota, IOT_OTAG_CHECK_FIRMWARE, &firmware_valid, 4);
            if (0 == firmware_valid) {
                EXAMPLE_TRACE("The firmware is invalid");
            } else {
                EXAMPLE_TRACE("The firmware is valid: %s", version);
                IOT_OTA_ReportProgress(h_ota, 100, NULL);
                IOT_OTA_ReportVersion(h_ota, version);
                memcpy(param, version, 128);
                if (fal_partition_erase(param_dev, 0, 128*1024) < 0)
                	EXAMPLE_TRACE("erase param zone failed");
                if (fal_partition_write(param_dev, 0, (const uint8_t *)param, 10*1024) < 0)
                	EXAMPLE_TRACE("write to param zone failed");
                rt_hw_cpu_reset();
            }

            ota_over = 1;
        }
        HAL_SleepMs(2000);
    } while (!ota_over);

    HAL_SleepMs(200);

    if (NULL != h_ota) {
        IOT_OTA_Deinit(h_ota);
    }

    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }

    if (ptr)
    	HAL_Free(ptr);
    if (param)
    	HAL_Free(param);
}

void mqtt_init()
{
	static rt_bool_t mqtt_inited = RT_FALSE;
    int                     res = 0;
    int                     loop_cnt = 0;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t       mqtt_params;

	if (mqtt_inited)
		return;
	mqtt_inited = RT_TRUE;
    HAL_GetProductKey(DEMO_PRODUCT_KEY);
    HAL_GetDeviceName(DEMO_DEVICE_NAME);
    HAL_GetDeviceSecret(DEMO_DEVICE_SECRET);
    if (0 != IOT_SetupConnInfo(DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME,
    			DEMO_DEVICE_SECRET, (void **)&pconn_info)) {
        EXAMPLE_TRACE("AUTH request failed!");
        return;
    }
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.host = "iot-06z00friwrngpm4.mqtt.iothub.aliyuncs.com";
    mqtt_params.handle_event.h_fp = example_event_handle;
    mqtt_params.port = pconn_info->port;
    //mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.read_buf_size = OTA_MQTT_MSGLEN;
    mqtt_params.write_buf_size = OTA_MQTT_MSGLEN;
    mqtt_params.handle_event.pcontext = NULL;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return;
    }
    h_ota = IOT_OTA_Init(DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME, pclient);
    if (NULL == h_ota) {
        EXAMPLE_TRACE("initialize OTA failed");
        return;
    }
//    HAL_SleepMs(1000);
    res = example_subscribe(pclient);
    if (res < 0) {
        IOT_MQTT_Destroy(&pclient);
        return;
    }
    net_ok = RT_TRUE;
}
#if 0
static rt_bool_t radar_check_network(void)
{
#ifdef RT_USING_NETDEV
    struct netdev * netdev = netdev_get_by_family(AF_INET);
    return (netdev && netdev_is_link_up(netdev));
#else
    return RT_TRUE;
#endif
}

static struct rt_work radar_sync_work;
static void radar_sync_work_func(struct rt_work *work, void *work_data)
{
    if (radar_check_network())
    {
        mqtt_init();
    }
    else
    {
        rt_work_submit(work, rt_tick_from_millisecond(5 * 1000));
    }
}

static int radar_auto_sync_init(void)
{
    rt_work_init(&radar_sync_work, radar_sync_work_func, RT_NULL);
    rt_work_submit(&radar_sync_work, rt_tick_from_millisecond(30 * 1000));
    return RT_EOK;
}
//INIT_COMPONENT_EXPORT(radar_auto_sync_init);
#endif
int radar_init()
{
	rt_err_t ret = RT_EOK;
	struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
	serial = rt_device_find("uart1");
	if (!serial) {
		rt_kprintf("find uart1 failed!\n");
		return RT_ERROR;
	}

	rt_mb_init(&mb, "mbt", &mb_radar[0], sizeof(mb_radar)/4, RT_IPC_FLAG_FIFO);

	rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
	rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
	rt_device_set_rx_indicate(serial, uart_input);
	//rt_device_write(serial, 0, str, (sizeof(str) - 1));
	config.baud_rate = BAUD_RATE_921600;
	config.data_bits = DATA_BITS_8;
	config.stop_bits = STOP_BITS_1;
	config.parity = PARITY_NONE;
	rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);

	rt_thread_t thread = rt_thread_create("serial", serial_thread_entry,
										  RT_NULL, 8192, 25, 10);
	if (thread != RT_NULL) {
		rt_thread_startup(thread);
	} else {
		ret = RT_ERROR;
	}

	rt_thread_t net_thread = rt_thread_create("net", net_thread_entry,
										  RT_NULL, 4096, 25, 10);
	if (net_thread != RT_NULL) {
		rt_thread_startup(net_thread);
	} else {
		ret = RT_ERROR;
	}
	return ret;
}
#endif
