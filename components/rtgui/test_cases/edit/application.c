#include <rtthread.h>

#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/label.h>

void win_thread_entry(void* parameter)
{
    test_main();
}

int rt_application_init()
{
	rt_thread_t tid;

    tid = rt_thread_create("win", win_thread_entry, RT_NULL,
        2048, 20, 20);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }

	return 0;
}
