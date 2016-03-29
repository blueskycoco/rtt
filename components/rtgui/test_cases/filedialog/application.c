#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/textbox.h>
#include <rtgui/widgets/panel.h>
#include <rtgui/widgets/box.h>
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/button.h>
#include <filedialog.h>

void win_thread_entry(void *parameter)
{
    struct rtgui_app *application;
    rtgui_filedialog_t *dialog;
    rtgui_rect_t rect;
    rtgui_filedialog_mode_t mode = RTGUI_FILEDIALOG_OPEN;
    char *filename = (char *)rt_malloc(255);
    rt_memset(filename, 0, 255);
    application = rtgui_app_create("filedialog_app");
    if (application != RT_NULL)
    {
        char *full_file_name;
        RTGUI_RECT(rect, 100, 50, 320, 240);
        if (mode == RTGUI_FILEDIALOG_OPEN)
        {
            dialog = rtgui_filedialog_create(RT_NULL, "打开文件", &rect);
        }
        else
        {
            dialog = rtgui_filedialog_create(RT_NULL, "保存文件", &rect);
        }
        rtgui_filedialog_set_directory(dialog, "/applications");
        rtgui_filedialog_set_filename(dialog, "test.txt");
        rtgui_filedialog_set_mode(dialog, mode);
        if (RT_TRUE == rtgui_filedialog_show(dialog))
        {
            full_file_name = rtgui_filedialog_get_full_filename(dialog);
            rt_kprintf("filename:%s\n", full_file_name);
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
