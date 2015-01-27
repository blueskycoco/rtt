#include <rtthread.h>

#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/label.h>

void win_thread_entry(void* parameter)
{
	struct rtgui_app* app;
	struct rtgui_win *win;
	struct rtgui_label *label;
    struct rtgui_box *box;

	struct rtgui_rect rect = {50, 50, 350, 350};

	app = rtgui_app_create("MyApp");
	RT_ASSERT(app != RT_NULL);

    win = rtgui_win_create(RT_NULL, "MyWindow", &rect, RTGUI_WIN_STYLE_DEFAULT);

    box = rtgui_box_create(RTGUI_HORIZONTAL, 10);
    rtgui_container_set_box(RTGUI_CONTAINER(win), box);

	label = rtgui_label_create("Hello World");
	rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));

    rtgui_container_layout(RTGUI_CONTAINER(win));

	rtgui_win_show(win, RT_FALSE);

	rtgui_app_run(app);

	rtgui_win_destroy(win);
	rtgui_app_destroy(app);
	rt_kprintf("MyApp Quit.\n");
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
