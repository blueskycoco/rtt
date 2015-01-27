#include <rtthread.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/rtgui_app.h>
#include "appmgr.h"
#include "statusbar.h"

void init_entry(void *param)
{
    app_mgr_init();
}

void rt_application_init(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
                           init_entry, RT_NULL,
                           2048, RT_THREAD_PRIORITY_MAX / 4, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
}

