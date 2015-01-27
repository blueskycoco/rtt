#include <rtgui/rtgui_object.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/rtgui_theme.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/button.h>
#include <rtgui/widgets/radiobox.h>
#include "messagedialog.h"

#define warning_icon   "/ui/warning.bmp"

static rtgui_button_t *button1;
static rtgui_label_t *label;
static rtgui_radiobox_t *radiobox;
static rtgui_messagedialog_t *msgboox;

static char *items[6] = {"Ok", "Ok Cancel", "Yes No", "Retry Cancel", "Yes No Cancel", "with icon"};

static void button1_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
    rtgui_dialog_result_t result;
    rtgui_rect_t rect;
    rtgui_image_t *icon;
    RTGUI_RECT(rect, 150, 80, 220, 100);
    switch (radiobox->item_selection)
    {
    case 0:
        msgboox = rtgui_messagedialog_create(RT_NULL, "hello\nI am messagedialog", "message",
                                             &rect, RTGUI_MB_FLAG_OK, RT_NULL);
        break;
    case 1:
        msgboox = rtgui_messagedialog_create(RT_NULL, "hello\nI am messagedialog", "message",
                                             &rect, RTGUI_MB_FLAG_OKCANCEL, RT_NULL);
        break;
    case 2:
        msgboox = rtgui_messagedialog_create(RT_NULL, "hello\nI am messagedialog", "message",
                                             &rect, RTGUI_MB_FLAG_YESNO, RT_NULL);
        break;
    case 3:
        msgboox = rtgui_messagedialog_create(RT_NULL, "hello\nI am messagedialog", "message",
                                             &rect, RTGUI_MB_FLAG_RETRY | RTGUI_MB_FLAG_CANCEL, RT_NULL);
        break;
    case 4:
        msgboox = rtgui_messagedialog_create(RT_NULL, "hello\nI am messagedialog", "message",
                                             &rect, RTGUI_MB_FLAG_YESNO | RTGUI_MB_FLAG_CANCEL, RT_NULL);
        break;
    case 5:
        icon = rtgui_image_create(warning_icon, RT_FALSE);
        msgboox = rtgui_messagedialog_create(RT_NULL, "hello\nI am messagedialog", "message",
                                             &rect, RTGUI_MB_FLAG_YESNO , icon);
        break;
    default:
        msgboox = rtgui_messagedialog_create(RT_NULL, "hello\nI am messagedialog", "message",
                                             &rect, RTGUI_MB_FLAG_DEFAULT, RT_NULL);
        break;
    }
    result = rtgui_messagedialog_show(msgboox);
    switch (result)
    {
    case RTGUI_DR_OK:
        rtgui_label_set_text(label, "result:Ok");
        break;
    case RTGUI_DR_CANCEL:
        rtgui_label_set_text(label, "result:Cancel");
        break;
    case RTGUI_DR_YES:
        rtgui_label_set_text(label, "result:Yes");
        break;
    case RTGUI_DR_NO:
        rtgui_label_set_text(label, "result:No");
        break;
    case RTGUI_DR_RETRY:
        rtgui_label_set_text(label, "result:Retry");
        break;
    default:
        break;
    }
    rtgui_messagedialog_destroy(msgboox);
    rtgui_widget_update(RTGUI_WIDGET(label));
}


void win_thread_entry(void *parameter)
{
    struct rtgui_app *application;
    rtgui_win_t *win;

    rtgui_rect_t rect;

    application = rtgui_app_create("messagedialog");
    if (application != RT_NULL)
    {
        char *full_file_name;
        RTGUI_RECT(rect, 0, 0, 480, 272);
        win = rtgui_win_create(RT_NULL, "messagedialog", &rect, RTGUI_WIN_STYLE_NO_TITLE | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
        if (win != RT_NULL)
        {
            radiobox = rtgui_radiobox_create("messagedialog buttons", RTGUI_VERTICAL, items, 6);
            rtgui_radiobox_set_selection(radiobox, 0);
            RTGUI_RECT(rect, 20, 40, 250, 140);
            rtgui_widget_set_rect(RTGUI_WIDGET(radiobox), &rect);

            label = rtgui_label_create("result:");
            RTGUI_RECT(rect, 300, 120, 150, 60);
            rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
            button1 = rtgui_button_create("Show");
            RTGUI_RECT(rect, 50, 200, 50, 40);
            rtgui_widget_set_rect(RTGUI_WIDGET(button1), &rect);
            rtgui_button_set_onbutton(button1, button1_onbutton);

            rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(radiobox));
            rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));
            rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(button1));
            rtgui_win_show(win, RT_TRUE);
        }
        rtgui_app_destroy(application);
    }

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

