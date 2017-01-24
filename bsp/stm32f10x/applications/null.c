#include <ymodem.h>
extern rt_uint8_t *key ;
extern rt_uint32_t key_len ;
static enum rym_code _rym_dummy_write(
        struct rym_ctx *ctx,
        rt_uint8_t *buf,
        rt_size_t len)
{
	if (key == RT_NULL)
	{
		key = (rt_uint8_t *)rt_malloc(len);
		rt_memcpy(key,buf,len);
		key_len = len;
	}
	else
	{		
		key = (rt_uint8_t *)rt_realloc(key, key_len+len);
		rt_memcpy(key+key_len,buf,len);
		key_len += len;
	}
    return RYM_CODE_ACK;
}

rt_err_t rym_null(char *devname)
{
    struct rym_ctx rctx;
    rt_device_t dev = rt_device_find(devname);
    if (!dev)
    {
        rt_kprintf("could not find device %s\n", devname);
        return -1;
    }

    return rym_recv_on_device(&rctx, dev,
            RT_NULL, _rym_dummy_write, RT_NULL, 1000);
}
#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(rym_null, dump data to null);
#endif

