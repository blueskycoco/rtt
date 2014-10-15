#include <rtthread.h>

#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/notebook.h>
#include <rtgui/widgets/panel.h>

struct rtgui_panel *_panel;
struct rtgui_notebook *_notebook;

void win_thread_entry(void* parameter)
{
	struct rtgui_app* app;
	struct rtgui_win *win;
	struct rtgui_panel *panel;
    struct rtgui_box *box;
    struct rtgui_label *label;
    struct rtgui_notebook *notebook;

    struct rtgui_rect rect = {50, 50, 350, 350};

	app = rtgui_app_create("MyApp");
	RT_ASSERT(app != RT_NULL);

    win = rtgui_mainwin_create(RT_NULL, "MyWindow", RTGUI_WIN_STYLE_DEFAULT);
    box = rtgui_box_create(RTGUI_VERTICAL, 10);
    rtgui_container_set_box(RTGUI_CONTAINER(win), box);

    /* create a panel */
    panel = rtgui_panel_create(RTGUI_BORDER_BOX);
    RTGUI_WIDGET_ALIGN(panel) = RTGUI_ALIGN_EXPAND;
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(panel));

    /* create sub-child for panel */
    box = rtgui_box_create(RTGUI_VERTICAL, 10);
    rtgui_container_set_box(RTGUI_CONTAINER(panel), box);

    label = rtgui_label_create("hello panel!");
    rtgui_container_add_child(RTGUI_CONTAINER(panel), RTGUI_WIDGET(label));

    /* create a notebook */
    notebook = rtgui_notebook_create(&rect, RTGUI_NOTEBOOK_TOP);
    RTGUI_WIDGET_ALIGN(notebook) = RTGUI_ALIGN_EXPAND;
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(notebook));
	_notebook = notebook;

    /* create tab-page for notebook  */
    panel = rtgui_panel_create(RTGUI_BORDER_STATIC);
	_panel = panel;
    box = rtgui_box_create(RTGUI_VERTICAL, 10);
    rtgui_container_set_box(RTGUI_CONTAINER(panel), box);

    label = rtgui_label_create("hello panel!");
    rtgui_container_add_child(RTGUI_CONTAINER(panel), RTGUI_WIDGET(label));
    rtgui_notebook_add(notebook, "Panel", RTGUI_WIDGET(panel));

    /* create another page with label */
    label = rtgui_label_create("hello notebook");
    rtgui_notebook_add(notebook, "Text", RTGUI_WIDGET(label));

    /* layout for window */
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
