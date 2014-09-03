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
    /*局部变量*/
    struct rtgui_app *application;
    /*建立应用程序*/
    application = rtgui_app_create("test");
    /*判断是否成功建立应用程序*/
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