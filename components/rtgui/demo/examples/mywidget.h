/*
 * �����嵥���Զ���ؼ�
 *
 * ���������Ҫʵ��һ���Զ���ؼ�����۴�����
 *   |
 * --o--
 *   |
 * ����״���м��oɫ�ʱ�ʾ�˵�ǰ��״̬��ON״̬ʱ����ɫ��OFF״̬ʱ�Ǻ�ɫ
 * ���ң����oλ�ý����������������л�����Ӧ��״̬
 */
#ifndef __MY_WIDGET_H__
#define __MY_WIDGET_H__

#include <rtgui/rtgui.h>
#include <rtgui/widgets/widget.h>

/* �Զ���ؼ���״ֵ̬���� */
#define MYWIDGET_STATUS_ON  1
#define MYWIDGET_STATUS_OFF 0

DECLARE_CLASS_TYPE(mywidget);
/** ÿ���ؼ�����һ�����ͣ�ͨ�����µĺ��ÿؼ���Ӧ��������Ϣ */
#define RTGUI_MYWIDGET_TYPE       (RTGUI_TYPE(mywidget))
/** ��һ������ʵ��������ͨ������ĺ�ʵ������ת�� */
#define RTGUI_MYWIDGET(obj)       (RTGUI_OBJECT_CAST((obj), RTGUI_MYWIDGET_TYPE, rtgui_mywidget_t))
/** ����ͨ������ĺ��Ծ���һ������ʵ���Ƿ����Զ���ؼ��� */
#define RTGUI_IS_MYWIDGET(obj)    (RTGUI_OBJECT_CHECK_TYPE((obj), RTGUI_MYWIDGET_TYPE))

/* ���Ի��ؼ��ඨ�� */
struct rtgui_mywidget
{
    /* ����ؼ��Ǽ̳���rtgui_widget�ؼ� */
    struct rtgui_widget parent;

    /* ״̬��ON��OFF */
    rt_uint8_t status;
};
typedef struct rtgui_mywidget rtgui_mywidget_t;

/* �ؼ��Ĵ�����ɾ�� */
struct rtgui_mywidget *rtgui_mywidget_create(rtgui_rect_t *r);
void rtgui_mywidget_destroy(struct rtgui_mywidget *me);

/* �ؼ���Ĭ���¼�������
 * ��һ���ؼ����ԣ���������������ӿؼ��ܿ��ܻ���ø��ؼ����¼���������
 * ����������ù��������ķ�ʽ
 */
rt_bool_t rtgui_mywidget_event_handler(struct rtgui_object *object, struct rtgui_event *event);

#endif
