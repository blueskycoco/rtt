#include <rtthread.h>

#include <rtgui/rtgui.h>
#include <rtgui/event.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/widget.h>

static struct rtgui_win *win1;
static struct rtgui_win *win2;

static rt_bool_t _dump_mouse_press(struct rtgui_object *object, struct rtgui_event *event)
{
    if (event->type == RTGUI_EVENT_MOUSE_BUTTON)
    {
        if (((struct rtgui_event_mouse*)event)->button & RTGUI_MOUSE_BUTTON_UP)
        {
            struct rtgui_win *win = (struct rtgui_win *)object;
            rt_kprintf("win %s on mouse, update other\n", win->title);
            if (object == win1)
                rtgui_widget_update(RTGUI_WIDGET(win2));
            else
                rtgui_widget_update(RTGUI_WIDGET(win1));
        }
    }
    return rtgui_win_event_handler(object, event);
}

static void _app(void *parameter)
{
    /* create application */
    struct rtgui_app *app;
    struct rtgui_rect rect1 = {0, 0, 240, 320};

    app = rtgui_app_create("app");

    if (!app)
    {
        rt_kprintf("Create application \"app\" failed!\n");
        return;
    }

    /* create main window */
    win1 = rtgui_win_create(RT_NULL, "main", &rect1,
            RTGUI_WIN_STYLE_DEFAULT);
    RT_ASSERT(win1);
    rtgui_object_set_event_handler(RTGUI_OBJECT(win1), _dump_mouse_press);

    rect1.x1 += 20;
    rect1.y1 += 40;
    rect1.x2 -= 20;
    rect1.y2 -= 40;
    win2 = rtgui_win_create(RT_NULL, "modal", &rect1,
            RTGUI_WIN_STYLE_DEFAULT & ~RTGUI_WIN_STYLE_CLOSEBOX);
    RT_ASSERT(win2);
    rtgui_object_set_event_handler(RTGUI_OBJECT(win2), _dump_mouse_press);

    rtgui_win_show(win1, RT_FALSE);
    rtgui_win_show(win2, RT_TRUE);

    rtgui_app_destroy(app);
}

int rt_application_init()
{
    rt_thread_t init_thread;

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
            _app, RT_NULL,
            2048, 20, 20);
#else
    init_thread = rt_thread_create("init",
            _app, RT_NULL,
            2048, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}

