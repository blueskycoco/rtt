#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/dc.h>
#include <rtgui/font.h>


extern void win1_ui_init(void);
void test_main(void)
{
    /*�ֲ�����*/
    struct rtgui_app *application;
    /*����Ӧ�ó���*/
    application = rtgui_app_create("test");
    /*�ж��Ƿ�ɹ�����Ӧ�ó���*/
    if (RT_NULL == application)
    {
        rt_kprintf("create application \"test\" failed!\n");
        return ;
    }
	//rtgui_font_system_init();
    win1_ui_init();
    rtgui_app_run(application);
    rtgui_app_destroy(application);
}
#include <finsh.h>
FINSH_FUNCTION_EXPORT(test_main, test run)