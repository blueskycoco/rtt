#include <rtthread.h>
#include <rtdevice.h>

static struct rt_semaphore rx_sem;
static rt_device_t serial;

static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&rx_sem);
	return RT_EOK;
}

static void serial_thread_entry(void *parameter)
{
	char ch;
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
				for (i = 0; i < cnt; i++) {
					rt_kprintf("%02x ", buf[i]);
				}
				rt_kprintf(" [%d]\r\n", cnt);
				cnt = 0;
			} else if (cnt > 50)
				cnt = 0;
			rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
		}
		if (cnt < 1024)
			buf[cnt++] = ch;
		//rt_kprintf("%02x ", ch);
		//ch = ch + 1;
		//rt_device_write(serial, 0, &ch, 1);
	}
}

int radar_init()
{
	rt_err_t ret = RT_EOK;
	struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
	serial = rt_device_find("uart1");
	if (!serial) {
		rt_kprintf("find uart1 failed!\n");
		return RT_ERROR;
	}

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
										  RT_NULL, 4096, 25, 10);
	if (thread != RT_NULL) {
		rt_thread_startup(thread);
	} else {
		ret = RT_ERROR;
	}

	return ret;
}
