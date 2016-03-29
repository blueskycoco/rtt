#include <rtthread.h>

#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/textbox.h>
#include <rtgui/widgets/panel.h>
#include <rtgui/widgets/box.h>
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/button.h>

void win_thread_entry(void* parameter)
{
    struct rtgui_app* app;
    struct rtgui_win *win;
    struct rtgui_box *box;
    struct rtgui_button *button;
    struct rtgui_panel* panel;
    struct rtgui_textbox* text;
    struct rtgui_label *label;

    struct rtgui_rect rect = {50, 50, 350, 170};

    app = rtgui_app_create("MyApp");
    RT_ASSERT(app != RT_NULL);

    win = rtgui_win_create(RT_NULL, "Login", &rect, RTGUI_WIN_STYLE_DEFAULT);

    box = rtgui_box_create(RTGUI_VERTICAL, 10);
    rtgui_container_set_box(RTGUI_CONTAINER(win), box);

    /* Usename */
    {

        struct rtgui_box *box;

        panel = rtgui_panel_create(RTGUI_BORDER_NONE);
        rtgui_widget_set_minheight(RTGUI_WIDGET(panel), 25);
        RTGUI_WIDGET_ALIGN(panel) = RTGUI_ALIGN_EXPAND;
        rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(panel));

        box = rtgui_box_create(RTGUI_HORIZONTAL, 3);
        rtgui_container_set_box(RTGUI_CONTAINER(panel), box);

        label = rtgui_label_create("Usename:");
        rtgui_widget_set_minwidth(RTGUI_WIDGET(label), 80);
        rtgui_container_add_child(RTGUI_CONTAINER(panel), RTGUI_WIDGET(label));

        text = rtgui_textbox_create("", RTGUI_TEXTBOX_SINGLE);
        RTGUI_WIDGET_ALIGN(text) = RTGUI_ALIGN_STRETCH;
        rtgui_widget_set_minwidth(RTGUI_WIDGET(text), 80);
        /*rtgui_widget_set_minheight(RTGUI_WIDGET(text), 25);*/
        rtgui_container_add_child(RTGUI_CONTAINER(panel), RTGUI_WIDGET(text));
    }

    /* Password */
    {
        struct rtgui_box *box;

        panel = rtgui_panel_create(RTGUI_BORDER_NONE);
        rtgui_widget_set_minheight(RTGUI_WIDGET(panel), 25);
        RTGUI_WIDGET_ALIGN(panel) = RTGUI_ALIGN_EXPAND;
        rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(panel));

        box = rtgui_box_create(RTGUI_HORIZONTAL, 3);
        rtgui_container_set_box(RTGUI_CONTAINER(panel), box);

        label = rtgui_label_create("Password:");
        rtgui_widget_set_minwidth(RTGUI_WIDGET(label), 80);
        rtgui_container_add_child(RTGUI_CONTAINER(panel), RTGUI_WIDGET(label));

        text = rtgui_textbox_create("", RTGUI_TEXTBOX_MASK);
        RTGUI_WIDGET_ALIGN(text) = RTGUI_ALIGN_STRETCH;
        rtgui_widget_set_minwidth(RTGUI_WIDGET(text), 80);
        /*rtgui_widget_set_minheight(RTGUI_WIDGET(text), 25);*/
        rtgui_container_add_child(RTGUI_CONTAINER(panel), RTGUI_WIDGET(text));
    }

    /* Button */
    button = rtgui_button_create("OK");
    rtgui_widget_set_minwidth(RTGUI_WIDGET(button), 80);
    rtgui_widget_set_minheight(RTGUI_WIDGET(button), 25);
    RTGUI_WIDGET_ALIGN(button) = RTGUI_ALIGN_RIGHT;
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(button));

    /* re-layout */
    rtgui_container_layout(RTGUI_CONTAINER(win));

    rtgui_win_show(win, RT_FALSE);
    rtgui_app_run(app);

    rtgui_win_destroy(win);
    rtgui_app_destroy(app);
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
