#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/textbox.h>
#include <rtgui/widgets/panel.h>
#include <rtgui/widgets/box.h>
#include <rtgui/widgets/button.h>
#include <filedialog.h>

char image_path[128];
rt_bool_t _on_browse(struct rtgui_object *object, struct rtgui_event *event)
{
	struct rtgui_filedialog *dialog;
	struct rtgui_win* parent;
	struct rtgui_rect rect;

	parent = RTGUI_WIDGET(object)->toplevel;

	rtgui_get_mainwin_rect(&rect);
	dialog = rtgui_filedialog_create(parent, "Open Image...", &rect);
	if (rtgui_filedialog_show(dialog))
	{
		char *path;

		path = rtgui_filedialog_get_full_filename(dialog);
		strcpy(image_path, path);
		rt_kprintf("Open Image: %s\n", image_path);
	}

	// rtgui_filedialog_destroy(dialog);
	return RT_TRUE;
}

rt_bool_t _on_show(struct rtgui_object *object, struct rtgui_event *event)
{
	if (event->type == RTGUI_EVENT_PAINT)
	{
        struct rtgui_dc *dc;
        struct rtgui_rect rect;
		struct rtgui_panel *panel;
		struct rtgui_image *image;
		
		panel = RTGUI_PANEL(object);
        rtgui_widget_get_rect(RTGUI_WIDGET(object), &rect);

        dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(object));
        if (dc == RT_NULL) return RT_FALSE;

        rtgui_dc_fill_rect(dc, &rect);

		image = rtgui_image_create(image_path, RT_TRUE);
		if (image != RT_NULL)
		{
			rtgui_image_blit(image, dc, RT_NULL);
			rtgui_image_destroy(image);
		}
		else
		{
			rt_kprintf("open image:%s failed!\n", image_path);
		}

		rtgui_dc_draw_border(dc, &rect, panel->border_style);
        /* paint on each child */
        rtgui_container_dispatch_event(RTGUI_CONTAINER(panel), event);

        rtgui_dc_end_drawing(dc);
		return RT_FALSE;
	}

	return rtgui_panel_event_handler(object, event);
}

void win_thread_entry(void *parameter)
{
    struct rtgui_app *app;
	struct rtgui_win *win;
	struct rtgui_box *box;

	app = rtgui_app_create("image");
	win = rtgui_mainwin_create(RT_NULL, "Image Demo", RTGUI_WIN_STYLE_MAINWIN);
	if (win != RT_NULL)
	{
		struct rtgui_textbox *text;
		struct rtgui_button *button;
		struct rtgui_panel *panel, *image_panel;

		box = rtgui_box_create(RTGUI_VERTICAL, 5);
		rtgui_container_set_box(RTGUI_CONTAINER(win), box);

		/* textbox and browse button */
		panel = rtgui_panel_create(RTGUI_BORDER_NONE);
		rtgui_widget_set_minheight(RTGUI_WIDGET(panel), 30);
		RTGUI_WIDGET_ALIGN(panel) = RTGUI_ALIGN_EXPAND;
		rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(panel));

			box = rtgui_box_create(RTGUI_HORIZONTAL, 5);
			rtgui_container_set_box(RTGUI_CONTAINER(panel), box);

			text = rtgui_textbox_create("", RTGUI_TEXTBOX_SINGLE);
			rtgui_widget_set_minheight(RTGUI_WIDGET(text), 30);
			RTGUI_WIDGET_ALIGN(text) = RTGUI_ALIGN_EXPAND | RTGUI_ALIGN_STRETCH;
			button = rtgui_button_create("Browse");
			rtgui_widget_set_minwidth(RTGUI_WIDGET(button), 80);
			rtgui_button_set_onbutton(button, _on_browse);

			rtgui_container_add_child(RTGUI_CONTAINER(panel), RTGUI_WIDGET(text));
			rtgui_container_add_child(RTGUI_CONTAINER(panel), RTGUI_WIDGET(button));

		/* image panel */
		image_panel = rtgui_panel_create(RTGUI_BORDER_SIMPLE);
		rtgui_object_set_event_handler(RTGUI_OBJECT(image_panel), _on_show);
		RTGUI_WIDGET_ALIGN(image_panel) = RTGUI_ALIGN_EXPAND | RTGUI_ALIGN_STRETCH;
		rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(image_panel));

		rtgui_container_layout(RTGUI_CONTAINER(win));
		rtgui_container_layout(RTGUI_CONTAINER(panel));
	}

	rtgui_win_show(win, RT_TRUE);
	rtgui_app_run(app);
	rtgui_app_destroy(app);
}

int rt_application_init()
{
    rt_thread_t tid;

    tid = rt_thread_create("image", win_thread_entry, RT_NULL,
                           2048, 20, 20);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }

    return 0;
}
