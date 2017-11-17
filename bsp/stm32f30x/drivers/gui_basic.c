/*****************************************************************************************
��Ȩ����:        ---------------  
�汾��:            1.0.0
��������:        2008.08.31
�ļ���:            gui_basic.c
����:            Ӱ����
����˵��:        GUI������ͼ����
����˵��:       ��   
�����ļ���ϵ:    ���ļ�Ϊ���̹�Լ�����ļ�

�޸ļ�¼:
��¼1:
�޸���:
�޸�����:
�޸�����:
�޸�ԭ��:
*****************************************************************************************/

#define  GUI_BASIC_GLOBALS                                        -
#include "app_types.h"

        


/*****************************************************************************************
��������:    void  GUI_HLine(INT16U x0, INT16U y0, INT16U x1) 
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ��ˮƽ��
�������:    x0        ˮƽ����������е�λ��
            y0        ˮƽ����������е�λ��
            x1      ˮƽ���յ������е�λ��
            color    ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_HLine(INT16U x0, INT16U y0, INT16U x1, uint16 color) 
{  
    INT16U  bak;

    if (x0 > x1)                                                             // ��x0��x1��С�������У��Ա㻭ͼ
    { 
        bak = x1;
        x1 = x0;
        x0 = bak;
    }

    GUI_DispColor(x0, x1, y0, y0, color);
}




/*****************************************************************************************
��������:    void  GUI_VLine(INT16U x0, INT16U y0, INT16U y1) 
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ����ֱ�ߡ�����Ӳ���ص㣬ʵ�ּ���
�������:    x0        ��ֱ����������е�λ��
            y0        ��ֱ����������е�λ��
            y1      ��ֱ���յ������е�λ��
            color    ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_VLine(INT16U x0, INT16U y0, INT16U y1, uint16 color) 
{ 
    INT16U  bak;
    
    if (y0 > y1)                                                             // ��y0��y1��С�������У��Ա㻭ͼ
    { 
        bak = y1;
        y1 = y0;
        y0 = bak;
    }

    GUI_DispColor(x0, x0, y0, y1, color);    
}




/*****************************************************************************************
��������:    void  GUI_DrawPoint(INT16U x, INT16U y)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ����
�������:    x0        �������Ͻǵ�x����ֵ
            y0        �������Ͻǵ�y����ֵ
            color    ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_DrawPoint(INT16U x, INT16U y, uint16 color)
{ 
    GUI_Point(x, y, color);
}




/*****************************************************************************************
��������:    void  GUI_Rectangle(INT16U x0, INT16U y0, INT16U x1, INT16U y1)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ������
�������:    x0        �������Ͻǵ�x����ֵ
            y0        �������Ͻǵ�y����ֵ
            x1      �������½ǵ�x����ֵ
            y1      �������½ǵ�y����ֵ
            color    ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_Rectangle(INT16U x0, INT16U y0, INT16U x1, INT16U y1, uint16 color)
{ 
    GUI_HLine(x0, y0, x1, color);
    GUI_HLine(x0, y1, x1, color);
    GUI_VLine(x0, y0, y1, color);
    GUI_VLine(x1, y0, y1, color);
}




/*****************************************************************************************
��������:    void  GUI_RectangleFill(INT16U x0, INT16U y0, INT16U x1, INT16U y1)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    �����Ρ���һ�����ľ��Σ����ɫ��߿�ɫһ��
�������:    x0        �������Ͻǵ�x����ֵ
            y0        �������Ͻǵ�y����ֵ
            x1      �������½ǵ�x����ֵ
            y1      �������½ǵ�y����ֵ
            color    ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_RectangleFill(INT16U x0, INT16U y0, INT16U x1, INT16U y1, uint16 color)
{ 
    INT16U  i;

    if (x0 > x1)                                                             // ��x0>x1����x0��x1����
    { 
        i = x0;
        x0 = x1;
        x1 = i;
    }

    if (y0 > y1)                                                            // ��y0>y1����y0��y1����
    {
        i = y0;
        y0 = y1;
        y1 = i;
    }

    if (y0 == y1)                                                            // �ж��Ƿ�ֻ��ֱ��
    { 
        GUI_HLine(x0, y0, x1, color);
        return;
    }

    if (x0 == x1) 
    {  
        GUI_VLine(x0, y0, y1, color);
        return;
    }

    for (y0 = y0; y0 <= y1; y0++)
    {
        GUI_HLine(x0, y0, x1, color);                                        // ��ǰ��ˮƽ��
    }
}




/*****************************************************************************************
��������:    void  GUI_Square(INT16U x0, INT16U y0, INT16U  with)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ��������
�������:    x0        ���������Ͻǵ�x����ֵ
            y0        ���������Ͻǵ�y����ֵ
            with    �����εı߳�
            color    ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_Square(INT16U x0, INT16U y0, INT16U  with, uint16 color)
{  
    GUI_Rectangle(x0, y0, x0 + with, y0 + with, color);
}




/*****************************************************************************************
��������:    void  GUI_Line(INT16U x0, INT16U y0, INT16U x1, INT16U y1)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ����������֮���ֱ��
�������:    x0        �������Ͻǵ�x����ֵ
            y0        �������Ͻǵ�y����ֵ
            x1      �������½ǵ�x����ֵ
            y1      �������½ǵ�y����ֵ
            color    ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_Line(INT16U x0, INT16U y0, INT16U x1, INT16U y1, uint16 color)
{
    INT32S   dx;                                                            // ֱ��x���ֵ����
    INT32S   dy;                                                              // ֱ��y���ֵ����
    INT08S   dx_sym;                                                        // x����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
    INT08S   dy_sym;                                                        // y����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
    INT32S   dx_x2;                                                            // dx*2ֵ���������ڼӿ������ٶ�
    INT32S   dy_x2;                                                            // dy*2ֵ���������ڼӿ������ٶ�
    INT32S   di;                                                            // ���߱���
    INT16U   bak;

    if (x0 > x1)                                                             // ��x0��x1��С�������У��Ա㻭ͼ
    { 
        bak = x1;
        x1 = x0;
        x0 = bak;
    }
    
    if (y0 > y1)                                                             // ��y0��y1��С�������У��Ա㻭ͼ
    { 
        bak = y1;
        y1 = y0;
        y0 = bak;
    }
    
    dx = x1-x0;                                                                // ��ȡ����֮��Ĳ�ֵ
    dy = y1-y0;

    // �ж��������򣬻��Ƿ�Ϊˮƽ�ߡ���ֱ�ߡ���

    if (dx>0)                                                                // �ж�x�᷽��
    {  
        dx_sym = 1;                                                            // dx>0������dx_sym=1
    }
    else
    { 
        if (dx<0)
        { 
            dx_sym = -1;                                                    // dx<0������dx_sym=-1
        }
        else
        { 
            GUI_VLine(x0, y0, y1, color);                                    // dx==0������ֱ�ߣ���һ��
            return;
        }
    }

    if (dy>0)                                                                // �ж�y�᷽��
    {  
        dy_sym = 1;                                                            // dy>0������dy_sym=1
    }
    else
    { 
        if (dy<0)
        {  
            dy_sym = -1;                                                    // dy<0������dy_sym=-1
        }
        else
        {
            GUI_HLine(x0, y0, x1, color);                                    // dy==0����ˮƽ�ߣ���һ��
            return;
        }
    }

    dx = dx_sym * dx;                                                        // ��dx��dyȡ����ֵ
    dy = dy_sym * dy;

    dx_x2 = dx*2;                                                            // ����2����dx��dyֵ
    dy_x2 = dy*2;

    // ʹ��Bresenham�����л�ֱ��

    if (dx>=dy)                                                                // ����dx>=dy����ʹ��x��Ϊ��׼
    { 
        di = dy_x2 - dx;

        while (x0!=x1)
        { 
            GUI_Point(x0, y0, color);
            x0 += dx_sym;

            if (di<0)
            { 
                di += dy_x2;                                                // �������һ���ľ���ֵ
            }
            else
            { 
                di += dy_x2 - dx_x2;
                y0 += dy_sym;
            }
        }

        GUI_Point(x0, y0, color);                        // ��ʾ���һ��
    }
    else                                                                    // ����dx<dy����ʹ��y��Ϊ��׼
    { 
        di = dx_x2 - dy;

        while (y0!=y1)
        { 
            GUI_Point(x0, y0, color);
            y0 += dy_sym;

            if (di<0)
            { 
                di += dx_x2;
            }
            else
            { 
                di += dx_x2 - dy_x2;
                x0 += dx_sym;
            }
        }

        GUI_Point(x0, y0, color);                        // ��ʾ���һ��
    }   
}




/*****************************************************************************************
��������:    void  GUI_LineDashed(INT16U x0, INT16U y0, INT16U x1, INT16U y1, INT16U dashed1, INT16U dashed2)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ����������֮���ֱ��
�������:    x0        ���Ͻǵ�x����ֵ
            y0        ���Ͻǵ�y����ֵ
            x1      ���½ǵ�x����ֵ
            y1      ���½ǵ�y����ֵ
            dashed1 ���γ���
            dashed2 ���γ���
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_LineDashed(INT16U x0, INT16U y0, INT16U x1, INT16U y1, INT16U dashed1, INT16U dashed2, uint16 color)
{
    INT16U  x, y, c;

    // ������ֱ��
    if ((x0 != x1) && (y0 != y1))
        return;

    if (x0 > x1)
    {
        x  = x0;
        x0 = x1;
        x1 = x;
    }

    if (y0 > y1)
    {
        y  = y0;
        y0 = y1;
        y1 = y;
    }
    
    // ����
    if (x0 == x1)
    {
        y = y0;
        c = 0;

        while (y1 > y)
        {
            // ��ǰ����
            if (c == 0)
            {
                if ((y + dashed1) <= y1)
                {
                    GUI_Line(x0, y, x1, y + dashed1, color);
                    c = 1;
                    y = y + dashed1;
                }
                else
                {
                    GUI_Line(x0, y, x1, y1, color);
                    c = 1;
                    y = y1;
                }
                continue;
            }

            // �Ǳ���ɫ, ����Ҫ��
            if (c == 1)
            {
                c = 0;
                y = y + dashed2;
                continue;
            }
        }

        return;
    }

    // ����
    if (y0 == y1)
    {
        x = x0;
        c = 0;

        while (x1 > x)
        {
            // ��ǰ����
            if (c == 0)
            {
                if ((x + dashed1) <= x1)
                {
                    GUI_Line(x, y0, x + dashed1, y1, color);
                    c = 1;
                    x = x + dashed1;
                }
                else
                {
                    GUI_Line(x, y0, x1, y1, color);
                    c = 1;
                    x = x1;
                }
                continue;
            }

            // �Ǳ���ɫ, ����Ҫ��
            if (c == 1)
            {
                c = 0;
                x = x + dashed2;
                continue;
            }
        }

        return;
    }
}




/*****************************************************************************************
��������:    void  GUI_LineWith(INT32U x0, INT32U y0, INT32U x1, INT32U y1, INT08U with)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ����������֮���ֱ�ߣ����ҿ������ߵĿ��
�������:    x0        �������Ͻǵ�x����ֵ
            y0        �������Ͻǵ�y����ֵ
            x1      �������½ǵ�x����ֵ
            y1      �������½ǵ�y����ֵ
            color    ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_LineWith(INT16U x0, INT16U y0, INT16U x1, INT16U y1, INT16U with, uint16 color)
{ 
    INT32S   dx;                                                            // ֱ��x���ֵ����
    INT32S   dy;                                                              // ֱ��y���ֵ����
    INT08S    dx_sym;                                                        // x����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
    INT08S    dy_sym;                                                        // y����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
    INT32S   dx_x2;                                                            // dx*2ֵ���������ڼӿ������ٶ�
    INT32S   dy_x2;                                                            // dy*2ֵ���������ڼӿ������ٶ�
    INT32S   di;                                                            // ���߱���
    INT32S   wx, wy;                                                        // �߿����
    INT32S   draw_a, draw_b;

    if (with == 0) 
        return;

    if (with > 50)
        with = 50;

    dx = x1-x0;                                                                // ��ȡ����֮��Ĳ�ֵ
    dy = y1-y0;

    wx = with/2;
    wy = with-wx-1;

    // �ж��������򣬻��Ƿ�Ϊˮƽ�ߡ���ֱ�ߡ���

    if (dx > 0)                                                                // �ж�x�᷽��
    {  
        dx_sym = 1;                                                            // dx>0������dx_sym=1
    }
    else
    { 
        if (dx < 0)
        { 
            dx_sym = -1;                                                    // dx<0������dx_sym=-1
        }
        else                                                                // dx==0������ֱ�ߣ���һ��
        {
            wx = x0-wx;

            if (wx < 0)
                wx = 0;

            wy = x0+wy;

            while (1)
            {
                x0 = wx;
                GUI_VLine(x0, y0, y1, color);

                if (wx >= wy)
                    break;

                wx++;
            }

            return;
        }
    }

    if (dy > 0)                                                                // �ж�y�᷽��
    {  
        dy_sym = 1;                                                            // dy>0������dy_sym=1
    }
    else
    {  
        if (dy < 0)
        {  
            dy_sym = -1;                                                    // dy<0������dy_sym=-1
        }
        else                                                                  // dy==0����ˮƽ�ߣ���һ��
        {
            wx = y0-wx;

            if (wx < 0) 
                wx = 0;

            wy = y0+wy;

            while (1)
            { 
                y0 = wx;
                GUI_HLine(x0, y0, x1, color);

                if (wx >= wy) 
                    break;

                wx++;
            }

            return;
        }
    }

    dx = dx_sym * dx;                                                       // ��dx��dyȡ����ֵ
    dy = dy_sym * dy;

    dx_x2 = dx*2;                                                            // ����2����dx��dyֵ
    dy_x2 = dy*2;

    // ʹ��Bresenham�����л�ֱ��

    if (dx >= dy)                                                            // ����dx>=dy����ʹ��x��Ϊ��׼
    { 
        di = dy_x2 - dx;

        while (x0 != x1)                                                    // x����������������y���򣬼�����ֱ��
        {
            draw_a = y0-wx;

            if (draw_a < 0) 
                draw_a = 0;

            draw_b = y0+wy;
            GUI_VLine(x0, draw_a, draw_b, color);

            x0 += dx_sym;    

            if (di < 0)
            { 
                di += dy_x2;                                                // �������һ���ľ���ֵ
            }
            else
            { 
                di += dy_x2 - dx_x2;
                y0 += dy_sym;
            }
        }

        draw_a = y0-wx;

        if (draw_a < 0) 
            draw_a = 0;

        draw_b = y0+wy;
        GUI_VLine(x0, draw_a, draw_b, color);
    }
    else                                                                    // ����dx<dy����ʹ��y��Ϊ��׼
    { 
        di = dx_x2 - dy;

        while (y0 != y1)                                                    // y����������������x���򣬼���ˮƽ��
        {
            draw_a = x0-wx;

            if (draw_a < 0)
                draw_a = 0;

            draw_b = x0+wy;
            GUI_HLine(draw_a, y0, draw_b, color);

            y0 += dy_sym;

            if (di < 0)
            { 
                di += dx_x2;
            }
            else
            { 
                di += dx_x2 - dy_x2;
                x0 += dx_sym;
            }
        }

        draw_a = x0-wx;

        if (draw_a < 0)
            draw_a = 0;

        draw_b = x0+wy;
        GUI_HLine(draw_a, y0, draw_b, color);
    }   
}




/*****************************************************************************************
��������:    void  GUI_Circle(INT32U x0, INT32U y0, INT32U r)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ָ��Բ��λ�ü��뾶����Բ
�������:    x0        Բ�ĵ�x����ֵ
            y0        Բ�ĵ�y����ֵ
            r       Բ�İ뾶
            color    ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_Circle(INT16U x0, INT16U y0, INT16U r, uint16 color)
{
    INT32S  draw_x0, draw_y0;                                                // ��ͼ���������
    INT32S  draw_x1, draw_y1;    
    INT32S  draw_x2, draw_y2;    
    INT32S  draw_x3, draw_y3;    
    INT32S  draw_x4, draw_y4;    
    INT32S  draw_x5, draw_y5;    
    INT32S  draw_x6, draw_y6;    
    INT32S  draw_x7, draw_y7;    
    INT32S  xx, yy;                                                            // ��Բ���Ʊ���
    INT32S  di;                                                                // ���߱���

    if (0 == r) 
        return;

    // �����8�������(0��45��90��135��180��225��270��)��������ʾ

    draw_x0 = draw_x1 = x0;
    draw_y0 = draw_y1 = y0 + r;

    if (draw_y0 < BOARD_LCD_HEIGHT) 
        GUI_Point(draw_x0, draw_y0, color);                // 90��

    draw_x2 = draw_x3 = x0;
    draw_y2 = draw_y3 = y0 - r;

    if (draw_y2 >= 0) 
        GUI_Point(draw_x2, draw_y2, color);                // 270��


    draw_x4 = draw_x6 = x0 + r;
    draw_y4 = draw_y6 = y0;

    if (draw_x4 < BOARD_LCD_WIDTH)
        GUI_Point(draw_x4, draw_y4, color);                // 0��

    draw_x5 = draw_x7 = x0 - r;
    draw_y5 = draw_y7 = y0;

    if (draw_x5 >= 0) 
        GUI_Point(draw_x5, draw_y5, color);                // 180��  

    if (1 == r) 
        return;                                                                // ���뾶Ϊ1������Բ����

    // ʹ��Bresenham�����л�Բ

    di = 3 - 2*r;                                                            // ��ʼ�����߱���

    xx = 0;
    yy = r;    

    while (xx < yy)
    { 
        if (di < 0)
        {  
            di += 4*xx + 6;          
        }
        else
        { 
            di += 4*(xx - yy) + 10;

            yy--;      

            draw_y0--;
            draw_y1--;
            draw_y2++;
            draw_y3++;
            draw_x4--;
            draw_x5++;
            draw_x6--;
            draw_x7++;         
        }

        xx++;   
        draw_x0++;
        draw_x1--;
        draw_x2++;
        draw_x3--;
        draw_y4++;
        draw_y5++;
        draw_y6--;
        draw_y7--;

        // Ҫ�жϵ�ǰ���Ƿ�����Ч��Χ��

        if ((draw_x0 <= BOARD_LCD_WIDTH) && (draw_y0 >= 0))    
            GUI_Point(draw_x0, draw_y0, color);

        if ((draw_x1 >= 0) && (draw_y1 >= 0))    
            GUI_Point(draw_x1, draw_y1, color);

        if ((draw_x2 <= BOARD_LCD_WIDTH) && (draw_y2 <= BOARD_LCD_HEIGHT))    
            GUI_Point(draw_x2, draw_y2, color);   

        if ((draw_x3 >= 0) && (draw_y3 <= BOARD_LCD_HEIGHT))    
            GUI_Point(draw_x3, draw_y3, color);

        if ((draw_x4 <= BOARD_LCD_WIDTH) && (draw_y4 >= 0))    
            GUI_Point(draw_x4, draw_y4, color);

        if ((draw_x5 >= 0) && (draw_y5 >= 0))    
            GUI_Point(draw_x5, draw_y5, color);

        if ((draw_x6 <= BOARD_LCD_WIDTH) && (draw_y6 <= BOARD_LCD_HEIGHT))    
            GUI_Point(draw_x6, draw_y6, color);

        if ((draw_x7 >= 0) && (draw_y7 <= BOARD_LCD_HEIGHT))    
            GUI_Point(draw_x7, draw_y7, color);
    }
}




/*****************************************************************************************
��������:    void  GUI_CircleFill(INT32U x0, INT32U y0, INT32U r)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ָ��Բ��λ�ü��뾶����Բ����䣬���ɫ��߿�ɫһ��
�������:    x0        Բ�ĵ�x����ֵ
            y0        Բ�ĵ�y����ֵ
            r       Բ�İ뾶
            color    �����ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_CircleFill(INT16U x0, INT16U y0, INT16U r, uint16 color)
{ 
    INT32S  draw_x0, draw_y0;                                                // ��ͼ���������
    INT32S  draw_x1, draw_y1;    
    INT32S  draw_x2, draw_y2;    
    INT32S  draw_x3, draw_y3;    
    INT32S  draw_x4, draw_y4;    
    INT32S  draw_x5, draw_y5;    
    INT32S  draw_x6, draw_y6;    
    INT32S  draw_x7, draw_y7;    
    INT32S  fill_x0, fill_y0;                                                // �������ı�����ʹ�ô�ֱ�����
    INT32S  fill_x1;
    INT32S  xx, yy;                                                            // ��Բ���Ʊ���
    INT32S  di;                                                                // ���߱���

    if (0 == r) 
        return;

    // �����4�������(0��90��180��270��)��������ʾ

    draw_x0 = draw_x1 = x0;
    draw_y0 = draw_y1 = y0 + r;

    if (draw_y0 < BOARD_LCD_HEIGHT)
        GUI_Point(draw_x0, draw_y0, color);                // 90��

    draw_x2 = draw_x3 = x0;
    draw_y2 = draw_y3 = y0 - r;

    if (draw_y2 >= 0)
        GUI_Point(draw_x2, draw_y2, color);                // 270��

    draw_x4 = draw_x6 = x0 + r;
    draw_y4 = draw_y6 = y0;

    if (draw_x4 < BOARD_LCD_WIDTH) 
    { 
        GUI_Point(draw_x4, draw_y4, color);                // 0��
        fill_x1 = draw_x4;
    }
    else
    {  
        fill_x1 = BOARD_LCD_WIDTH;
    }

    fill_y0 = y0;                                                            // �������������ʼ��fill_x0
    fill_x0 = x0 - r;                                                        // �����������������fill_y1

    if (fill_x0 < 0)
        fill_x0 = 0;

    GUI_HLine(fill_x0, fill_y0, fill_x1, color);

    draw_x5 = draw_x7 = x0 - r;
    draw_y5 = draw_y7 = y0;

    if (draw_x5 >= 0) 
        GUI_Point(draw_x5, draw_y5, color);                // 180��

    if (1 == r) 
        return;

    // ʹ��Bresenham�����л�Բ

    di = 3 - 2*r;                                                            // ��ʼ�����߱���

    xx = 0;
    yy = r;

    while (xx < yy)
    {  
        if (di<0)
        {  
            di += 4*xx + 6;
        }
        else
        { 
            di += 4*(xx - yy) + 10;

            yy--;      
            draw_y0--;
            draw_y1--;
            draw_y2++;
            draw_y3++;
            draw_x4--;
            draw_x5++;
            draw_x6--;
            draw_x7++;         
        }

        xx++;   
        draw_x0++;
        draw_x1--;
        draw_x2++;
        draw_x3--;
        draw_y4++;
        draw_y5++;
        draw_y6--;
        draw_y7--;

        if ((draw_x0 <= BOARD_LCD_WIDTH) && (draw_y0 >= 0))                    // Ҫ�жϵ�ǰ���Ƿ�����Ч��Χ��
            GUI_Point(draw_x0, draw_y0, color);

        if ((draw_x1 >= 0) && (draw_y1 >= 0))    
            GUI_Point(draw_x1, draw_y1, color);

        if (draw_x1 >= 0)                                                    // �ڶ���ˮֱ�����(�°�Բ�ĵ�)
        {
            fill_x0 = draw_x1;                                                // �������������ʼ��fill_x0

            fill_y0 = draw_y1;                                                // �������������ʼ��fill_y0

            if (fill_y0 > BOARD_LCD_HEIGHT) 
                fill_y0 = BOARD_LCD_HEIGHT;

            if (fill_y0 < 0) 
                fill_y0 = 0; 

            fill_x1 = x0*2 - draw_x1;                                        // �����������������fill_x1

            if (fill_x1 > BOARD_LCD_WIDTH) 
                fill_x1 = BOARD_LCD_WIDTH;

            GUI_HLine(fill_x0, fill_y0, fill_x1, color);
        }


        if ((draw_x2 <= BOARD_LCD_WIDTH) && (draw_y2 <= BOARD_LCD_HEIGHT))    
            GUI_Point(draw_x2, draw_y2, color);   

        if ((draw_x3 >= 0) && (draw_y3 <= BOARD_LCD_HEIGHT))    
            GUI_Point(draw_x3, draw_y3, color);


        if (draw_x3 >= 0)                                                    // ���ĵ㴹ֱ�����(�ϰ�Բ�ĵ�)
        {
            fill_x0 = draw_x3;                                                // �������������ʼ��fill_x0

            fill_y0 = draw_y3;                                                // �������������ʼ��fill_y0

            if (fill_y0 > BOARD_LCD_HEIGHT) 
                fill_y0 = BOARD_LCD_HEIGHT;

            if (fill_y0 < 0) 
                fill_y0 = 0;

            fill_x1 = x0*2 - draw_x3;                                        // �����������������fill_x1    

            if (fill_x1 > BOARD_LCD_WIDTH)
                fill_x1 = BOARD_LCD_WIDTH;

            GUI_HLine(fill_x0, fill_y0, fill_x1, color);
        }


        if ((draw_x4 <= BOARD_LCD_WIDTH) && (draw_y4 >= 0))    
            GUI_Point(draw_x4, draw_y4, color);

        if ((draw_x5 >= 0) && (draw_y5 >= 0))    
            GUI_Point(draw_x5, draw_y5, color);



        if (draw_x5 >= 0)                                                    // �����㴹ֱ�����(�ϰ�Բ�ĵ�)
        { 
            fill_x0 = draw_x5;                                                // �������������ʼ��fill_x0

            fill_y0 = draw_y5;                                                // �������������ʼ��fill_y0

            if (fill_y0 > BOARD_LCD_HEIGHT) 
                fill_y0 = BOARD_LCD_HEIGHT;

            if (fill_y0 < 0) 
                fill_y0 = 0;

            fill_x1 = x0*2 - draw_x5;                                        // �����������������fill_x1    

            if (fill_x1 > BOARD_LCD_WIDTH)
                fill_x1 = BOARD_LCD_WIDTH;

            GUI_HLine(fill_x0, fill_y0, fill_x1, color);
        }


        if ((draw_x6 <= BOARD_LCD_WIDTH) && (draw_y6 <= BOARD_LCD_HEIGHT))    
            GUI_Point(draw_x6, draw_y6, color);


        if ((draw_x7 >= 0) && (draw_y7 <= BOARD_LCD_HEIGHT))    
            GUI_Point(draw_x7, draw_y7, color);


        if (draw_x7 >= 0)                                                    // �ڰ˵㴹ֱ�����(�ϰ�Բ�ĵ�)
        { 
            fill_x0 = draw_x7;                                                // �������������ʼ��fill_x0

            fill_y0 = draw_y7;                                                // �������������ʼ��fill_y0

            if (fill_y0 > BOARD_LCD_HEIGHT) 
                fill_y0 = BOARD_LCD_HEIGHT;

            if (fill_y0 < 0)
                fill_y0 = 0;

            fill_x1 = x0*2 - draw_x7;                                        // �����������������fill_x1    

            if (fill_x1 > BOARD_LCD_WIDTH) 
                fill_x1 = BOARD_LCD_WIDTH;

            GUI_HLine(fill_x0, fill_y0, fill_x1, color);
        }      
    }
}




/*****************************************************************************************
��������:    void  GUI_Ellipse(INT32U x0, INT32U x1, INT32U y0, INT32U y1)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ������Բ��������Բ���ĸ���Ĳ������������ҵ��x������ֵΪx0��x1�����ϡ����µ�
            ��y������Ϊy0��y1
�������:    x0        �������Ͻǵ�x����ֵ
            y0        �������Ͻǵ�y����ֵ
            x1      �������½ǵ�x����ֵ
            y1      �������½ǵ�y����ֵ
            color    ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_Ellipse(INT16U x0, INT16U x1, INT16U y0, INT16U y1, uint16 color)
{ 
    INT32S  draw_x0, draw_y0;                                                // ��ͼ���������
    INT32S  draw_x1, draw_y1;
    INT32S  draw_x2, draw_y2;
    INT32S  draw_x3, draw_y3;
    INT32S  xx, yy;                                                            // ��ͼ���Ʊ���
    INT32S  center_x, center_y;                                            // ��Բ���ĵ��������
    INT32S  radius_x, radius_y;                                            // ��Բ�İ뾶��x��뾶��y��뾶
    INT32S  radius_xx, radius_yy;                                            // �뾶��ƽ��ֵ
    INT32S  radius_xx2, radius_yy2;                                        // �뾶��ƽ��ֵ������
    INT32S  di;                                                                // ������߱���

    if ((x0 == x1) || (y0 == y1)) 
        return;

    center_x = (x0 + x1) >> 1;                                                // �������Բ���ĵ�����
    center_y = (y0 + y1) >> 1;

    if (x0 > x1)                                                            // �������Բ�İ뾶��x��뾶��y��뾶
        radius_x = (x0 - x1) >> 1;
    else
        radius_x = (x1 - x0) >> 1;

    if (y0 > y1)
        radius_y = (y0 - y1) >> 1;
    else
        radius_y = (y1 - y0) >> 1;

    radius_xx = radius_x * radius_x;                                        // ����뾶ƽ��ֵ
    radius_yy = radius_y * radius_y;

    radius_xx2 = radius_xx<<1;                                                // ����뾶ƽ��ֵ��2ֵ
    radius_yy2 = radius_yy<<1;

    xx = 0;                                                                    // ��ʼ����ͼ����
    yy = radius_y;

    di = radius_yy2 + radius_xx - radius_xx2*radius_y ;                        // ��ʼ�����߱��� 

    draw_x0 = draw_x1 = draw_x2 = draw_x3 = center_x;                        // �������Բy���ϵ������˵����꣬��Ϊ��ͼ���
    draw_y0 = draw_y1 = center_y + radius_y;
    draw_y2 = draw_y3 = center_y - radius_y;

    GUI_Point(draw_x0, draw_y0, color);                    // ��y���ϵ������˵� 
    GUI_Point(draw_x2, draw_y2, color);

    while ((radius_yy*xx) < (radius_xx*yy)) 
    {
        if (di < 0)
        {  
            di+= radius_yy2*(2*xx+3);
        }
        else
        { 
            di += radius_yy2*(2*xx+3) + 4*radius_xx - 4*radius_xx*yy;

            yy--;
            draw_y0--;
            draw_y1--;
            draw_y2++;
            draw_y3++;                 
        }

        xx ++;                                                                // x���1

        draw_x0++;
        draw_x1--;
        draw_x2++;
        draw_x3--;

        GUI_Point(draw_x0, draw_y0, color);
        GUI_Point(draw_x1, draw_y1, color);
        GUI_Point(draw_x2, draw_y2, color);
        GUI_Point(draw_x3, draw_y3, color);
    }

    di = radius_xx2*(yy-1)*(yy-1) + radius_yy2*xx*xx + radius_yy + radius_yy2*xx - radius_xx2*radius_yy;

    while (yy >= 0) 
    {  
        if (di < 0)
        { 
            di+= radius_xx2*3 + 4*radius_yy*xx + 4*radius_yy - 2*radius_xx2*yy;

            xx ++;                                                            // x���1             
            draw_x0++;
            draw_x1--;
            draw_x2++;
            draw_x3--;  
        }
        else
        {  
            di += radius_xx2*3 - 2*radius_xx2*yy;                                    
        }

        yy--;
        draw_y0--;
        draw_y1--;
        draw_y2++;
        draw_y3++;    

        GUI_Point(draw_x0, draw_y0, color);
        GUI_Point(draw_x1, draw_y1, color);
        GUI_Point(draw_x2, draw_y2, color);
        GUI_Point(draw_x3, draw_y3, color);
    }     
}




/*****************************************************************************************
��������:    void  GUI_EllipseFill(INT32U x0, INT32U x1, INT32U y0, INT32U y1)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ������Բ������䡣������Բ���ĸ���Ĳ������������ҵ��x������ֵΪx0��x1�����ϡ����µ�
            ��y������Ϊy0��y1
�������:    x0        �������Ͻǵ�x����ֵ
            y0        �������Ͻǵ�y����ֵ
            x1      �������½ǵ�x����ֵ
            y1      �������½ǵ�y����ֵ
            color    ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_EllipseFill(INT16U x0, INT16U x1, INT16U y0, INT16U y1, uint16 color)
{ 
    INT32S  draw_x0, draw_y0;                                                // ��ͼ���������
    INT32S  draw_x1, draw_y1;
    INT32S  draw_x2, draw_y2;
    INT32S  draw_x3, draw_y3;
    INT32S  xx, yy;                                                            // ��ͼ���Ʊ���
    INT32S  center_x, center_y;                                            // ��Բ���ĵ��������
    INT32S  radius_x, radius_y;                                            // ��Բ�İ뾶��x��뾶��y��뾶
    INT32S  radius_xx, radius_yy;                                            // �뾶��ƽ��ֵ
    INT32S  radius_xx2, radius_yy2;                                        // �뾶��ƽ��ֵ������
    INT32S  di;                                                                // ������߱���

    if ((x0 == x1) || (y0 == y1))
        return;

    center_x = (x0 + x1) >> 1;                                                // �������Բ���ĵ�����        
    center_y = (y0 + y1) >> 1;

    if (x0 > x1)                                                            // �������Բ�İ뾶��x��뾶��y��뾶
        radius_x = (x0 - x1) >> 1;
    else
        radius_x = (x1 - x0) >> 1;


    if (y0 > y1)
        radius_y = (y0 - y1) >> 1;
    else
        radius_y = (y1 - y0) >> 1;

    radius_xx = radius_x * radius_x;                                        // ����뾶��ƽ��ֵ
    radius_yy = radius_y * radius_y;

    radius_xx2 = radius_xx<<1;                                                // ����뾶��4ֵ
    radius_yy2 = radius_yy<<1;

    xx = 0;                                                                    // ��ʼ����ͼ����
    yy = radius_y;

    di = radius_yy2 + radius_xx - radius_xx2*radius_y ;                        // ��ʼ�����߱��� 

    draw_x0 = draw_x1 = draw_x2 = draw_x3 = center_x;                        // �������Բy���ϵ������˵����꣬��Ϊ��ͼ���
    draw_y0 = draw_y1 = center_y + radius_y;
    draw_y2 = draw_y3 = center_y - radius_y;

    GUI_Point(draw_x0, draw_y0, color);                    // ��y���ϵ������˵�
    GUI_Point(draw_x2, draw_y2, color);

    while ((radius_yy*xx) < (radius_xx*yy)) 
    { 
        if (di < 0)
        {  
            di+= radius_yy2*(2*xx+3);
        }
        else
        {  
            di += radius_yy2*(2*xx+3) + 4*radius_xx - 4*radius_xx*yy;

            yy--;
            draw_y0--;
            draw_y1--;
            draw_y2++;
            draw_y3++;                 
        }

        xx ++;                                                                // x���1

        draw_x0++;
        draw_x1--;
        draw_x2++;
        draw_x3--;

        GUI_Point(draw_x0, draw_y0, color);
        GUI_Point(draw_x1, draw_y1, color);
        GUI_Point(draw_x2, draw_y2, color);
        GUI_Point(draw_x3, draw_y3, color);

        if (di >= 0)                                                        // ��y���ѱ仯���������
        { 
            GUI_HLine(draw_x0, draw_y0, draw_x1, color);
            GUI_HLine(draw_x2, draw_y2, draw_x3, color);
        }
    }

    di = radius_xx2*(yy-1)*(yy-1) + radius_yy2*xx*xx + radius_yy + radius_yy2*xx - radius_xx2*radius_yy;

    while (yy >= 0) 
    {  
        if (di < 0)
        { 
            di+= radius_xx2*3 + 4*radius_yy*xx + 4*radius_yy - 2*radius_xx2*yy;

            xx ++;                                                            // x���1             
            draw_x0++;
            draw_x1--;
            draw_x2++;
            draw_x3--;  
        }
        else
        { 
            di += radius_xx2*3 - 2*radius_xx2*yy;                                    
        }

        yy--;
        draw_y0--;
        draw_y1--;
        draw_y2++;
        draw_y3++;    

        GUI_Point(draw_x0, draw_y0, color);
        GUI_Point(draw_x1, draw_y1, color);
        GUI_Point(draw_x2, draw_y2, color);
        GUI_Point(draw_x3, draw_y3, color);

        GUI_HLine(draw_x0, draw_y0, draw_x1, color);                            // y���ѱ仯���������
        GUI_HLine(draw_x2, draw_y2, draw_x3, color); 
    }     
}




/*****************************************************************************************
��������:    void  GUI_Arc4(INT32U x, INT32U y, INT32U r, INT08U angle)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ��������㼰�յ�ֻ��Ϊ0��-90�ȡ�90��-180�ȡ�180��-270�ȡ�270��-0�ȵȡ����ֱ�
            Ϊ��1-4���޵�90�Ȼ�
�������:    x0        Բ�ĵ�x����ֵ
            y0        Բ�ĵ�y����ֵ
            r       Բ���İ뾶
            angle    ����������(1-4)
            color    ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_Arc4(INT16U x, INT16U y, INT16U r, INT16U angle, uint16 color)
{ 
    INT32S  draw_x, draw_y;
    INT32S  op_x, op_y;
    INT32S  op_2rr;

    if (r == 0)
        return;

    op_2rr = 2*r*r;                                                            // ����rƽ������2

    switch(angle)
    { 
        case  1:
                draw_x = x+r;
                draw_y = y;

                op_x = r;
                op_y = 0;

                while (1)
                {  
                    GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

                    op_y++;                                                            // ������һ��
                    draw_y++;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)        // ʹ�����ȽϷ�ʵ�ֻ�Բ��
                    { 
                        op_x--;
                        draw_x--;
                    }

                    if (op_y>=op_x) 
                        break;
                }

                while (1)
                { 
                    GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ
                
                    op_x--;                                                            // ������һ��
                    draw_x--;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)        // ʹ�����ȽϷ�ʵ�ֻ�Բ��
                    {  
                        op_y++;
                        draw_y++;
                    }

                    if (op_x <= 0)
                    {  
                        GUI_Point(draw_x, draw_y, color);        // ��ʼ��ͼ
                        break;
                    }
                }
                break;      

        case  2:
                draw_x = x-r;
                draw_y = y;

                op_x = r;
                op_y = 0;

                while (1)
                {  
                    GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

                    op_y++;                                                            // ������һ��
                    draw_y++;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)        // ʹ�����ȽϷ�ʵ�ֻ�Բ��
                    {  
                        op_x--;
                        draw_x++;
                    }

                    if (op_y >= op_x) 
                        break;
                }

                while (1)
                { 
                    GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

                    op_x--;                                                            // ������һ��
                    draw_x++;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)        // ʹ�����ȽϷ�ʵ�ֻ�Բ��
                    { 
                        op_y++;
                        draw_y++;
                    }

                    if (op_x <= 0)
                    { 
                        GUI_Point(draw_x, draw_y, color);        // ��ʼ��ͼ
                        break;
                    }
                }
                break;

        case  3:
                draw_x = x-r;
                draw_y = y;

                op_x = r;
                op_y = 0;

                while (1)
                { 
                    GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

                    op_y++;                                                            // ������һ��
                    draw_y--;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)        // ʹ�����ȽϷ�ʵ�ֻ�Բ��
                    {  
                        op_x--;
                        draw_x++;
                    }

                    if (op_y >= op_x)
                        break;
                }

                while (1)
                {  
                    GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

                    op_x--;                                                            // ������һ��
                    draw_x++;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)        // ʹ�����ȽϷ�ʵ�ֻ�Բ��
                    {  
                        op_y++;
                        draw_y--;
                    }

                    if (op_x <= 0)
                    {  
                        GUI_Point(draw_x, draw_y, color);        // ��ʼ��ͼ
                        break;
                    }
                }
                break;

        case  4:
                draw_x = x+r;
                draw_y = y;

                op_x = r;
                op_y = 0;

                while (1)
                { 
                    GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

                    op_y++;                                                            // ������һ��
                    draw_y--;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)        // ʹ�����ȽϷ�ʵ�ֻ�Բ��
                    { 
                        op_x--;
                        draw_x--;
                    }

                    if (op_y >= op_x)
                        break;
                }

                while (1)
                { 
                    GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

                    op_x--;                                                            // ������һ��
                    draw_x--;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)        // ʹ�����ȽϷ�ʵ�ֻ�Բ��
                    { 
                        op_y++;
                        draw_y--;
                    }

                    if (op_x <= 0)
                    { 
                        GUI_Point(draw_x, draw_y, color);        // ��ʼ��ͼ
                        break;
                    }
                }
                break;

        default:
                break;
    }
}




/*****************************************************************************************
��������:    void  GUI_Arc(INT32U x, INT32U y, INT32U r, INT32U stangle, INT32U endangle)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ָ����㡢�յ㼰�뾶����(���ܻ�Բ)��ʹ�õ���˳ʱ�뷽��ͼ
�������:    x            Բ�ĵ�x������ֵ
            y            Բ�ĵ�y������ֵ
            stangle     ��ʼ�Ƕ�(0-359��)
            endangle    ��ֹ�Ƕ�(0-359��)
            r              Բ�İ뾶�յ�
            color        ��ʾ��ɫ
�������:    ��
����˵��:    ��
*****************************************************************************************/
void  GUI_Arc(INT16U x, INT16U y, INT16U r, INT16U stangle, INT16U endangle, uint16 color)
{ 
    INT32S  draw_x, draw_y;                                                // ��ͼ�������
    INT32S  op_x, op_y;                                                    // ��������
    INT32S  op_2rr;                                                            // 2*r*rֵ����
    INT32S  pno_angle;                                                        // �Ƚǵ�ĸ���
    INT08U  draw_on;                                                        // ���㿪�أ�Ϊ1ʱ���㣬Ϊ0ʱ����

    if (r == 0) 
        return;                                                                // �뾶Ϊ0��ֱ���˳�

    if (stangle == endangle) 
        return;                                                                // ��ʼ�Ƕ�����ֹ�Ƕ���ͬ���˳�

    if ((stangle >= 360) || (endangle >= 360))
        return;

    op_2rr = 2*r*r;                                                            // ����rƽ������2
    pno_angle = 0;

    op_x = r;                                                                // �ȼ�����ڴ˰뾶�µ�45�ȵ�Բ���ĵ���  
    op_y = 0;

    while (1)
    { 
        pno_angle++;                                                         // �������         

        op_y++;                                                                // ������һ��

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)             // ʹ�����ȽϷ�ʵ�ֻ�Բ��
            op_x--;

        if (op_y >= op_x) 
            break;
    }

    draw_on = 0;                                                            // �ʼ�ػ��㿪��


    if (endangle > stangle) 
        draw_on = 1;                                                        // ���յ������㣬���һ��ʼ������(359)

    stangle = (360-stangle)*pno_angle/45;
    endangle = (360-endangle)*pno_angle/45;

    if (stangle == 0) 
        stangle=1;

    if (endangle == 0) 
        endangle=1;


    pno_angle = 0;                                                            // ��ʼ˳ʱ�뻭������359�ȿ�ʼ(��4����)

    draw_x = x+r;
    draw_y = y;         
    op_x = r;
    op_y = 0;

    while (1)
    { 
        op_y++;                                                                // ������һ��
        draw_y--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)             // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_x--;
            draw_x--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
        } 

        if (op_y >= op_x)
        { 
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
            break;
        }
    }

    while (1)
    { 
        op_x--;                                                                // ������һ��
        draw_x--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)            // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        {  
            op_y++;
            draw_y--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
        } 

        if (op_x <= 0)
        { 
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);        // ��ʼ��ͼ

            break;
        }
    }

    draw_y = y-r;                                                            // ��ʼ˳ʱ�뻭������269�ȿ�ʼ(��3����)
    draw_x = x;         
    op_y = r;
    op_x = 0;

    while (1)
    {
        op_x++;                                                                // ������һ��
        draw_x--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_y +1) > 0)            // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_y--;
            draw_y++;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
        } 

        if (op_x >= op_y)
        {  
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);        // ��ʼ��ͼ
            break;
        }
    }

    while (1)
    {
        op_y--;                                                                // ������һ��
        draw_y++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_x +1) <= 0)            // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        {  
            op_x++;
            draw_x--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
        } 

        if (op_y <= 0)
        { 
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
            break;
        }
    }

    draw_x = x-r;                                                            // ��ʼ˳ʱ�뻭������179�ȿ�ʼ(��2����)
    draw_y = y;         
    op_x = r;
    op_y = 0;

    while (1)
    {
        op_y++;                                                                // ������һ��
        draw_y++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)             // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_x--;
            draw_x++;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1)
                GUI_Point(draw_x, draw_y, color);
        } 

        if (op_y >= op_x)
        { 
            if (draw_on == 1)
                GUI_Point(draw_x, draw_y, color);
            break;
        }
    }

    while (1)
    { 
        op_x--;                                                                // ������һ��
        draw_x++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)            // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_y++;
            draw_y++;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1)
                GUI_Point(draw_x, draw_y, color);
        } 

        if (op_x <= 0)
        {
            if (draw_on == 1)
                GUI_Point(draw_x, draw_y, color);        // ��ʼ��ͼ
            break;
        }
    }

    draw_y = y+r;                                                            // ��ʼ˳ʱ�뻭������89�ȿ�ʼ(��1����)
    draw_x = x;         
    op_y = r;
    op_x = 0;

    while (1)
    {
        op_x++;                                                                // ������һ��
        draw_x++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_y +1) > 0)            // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_y--;
            draw_y--;
        }

        if (draw_on == 1)
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        {  
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
        } 

        if (op_x >= op_y)
        {
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);        // ��ʼ��ͼ
            break;
        }
    }

    while (1)
    {
        op_y--;                                                                // ������һ��
        draw_y--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_x +1) <= 0)            // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_x++;
            draw_x++;
        }

        if (draw_on == 1)
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
        } 

        if (op_y <= 0)
        { 
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
            break;
        }
    }   
}




/*****************************************************************************************
��������:    void  GUI_Pieslice(INT32U x, INT32U y, INT32U r, INT32U stangle, INT32U endangle)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ָ����㡢�յ㼰�뾶����(���ܻ�Բ)��ʹ�õ���˳ʱ�뷽��ͼ
�������:    x            Բ�ĵ�x������ֵ
            y            Բ�ĵ�y������ֵ
            stangle     ��ʼ�Ƕ�(0-359��)
            endangle    ��ֹ�Ƕ�(0-359��)
            r              Բ�İ뾶�յ�
            color        ��ʾ��ɫ
�������:    ��    
����˵��:    ��
*****************************************************************************************/
void  GUI_Pieslice(INT16U x, INT16U y, INT16U r, INT16U stangle, INT16U endangle, uint16 color)
{ 
    INT32S  draw_x, draw_y;                                                // ��ͼ�������
    INT32S  op_x, op_y;                                                    // ��������
    INT32S  op_2rr;                                                            // 2*r*rֵ����
    INT32S  pno_angle;                                                        // �Ƚǵ�ĸ���
    INT08U  draw_on;                                                        // ���㿪�أ�Ϊ1ʱ���㣬Ϊ0ʱ����

    if (r == 0) 
        return;                                                                // �뾶Ϊ0��ֱ���˳�

    if (stangle == endangle) 
        return;                                                                // ��ʼ�Ƕ�����ֹ�Ƕ���ͬ���˳�

    if ((stangle >= 360) || (endangle >= 360)) 
        return;

    op_2rr = 2*r*r;                                                            // ����rƽ������2
    pno_angle = 0;

    op_x = r;                                                                // �ȼ�����ڴ˰뾶�µ�45�ȵ�Բ���ĵ���      
    op_y = 0;

    while (1)
    {  
        pno_angle++;                                                         // �������         
        op_y++;                                                                // ������һ��

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)             // ʹ�����ȽϷ�ʵ�ֻ�Բ��
            op_x--;

        if (op_y >= op_x)
            break;
    }

    draw_on = 0;                                                            // �ʼ�ػ��㿪��

    if (endangle > stangle) 
        draw_on = 1;                                                        // ���յ������㣬���һ��ʼ������(359)

    stangle = (360-stangle)*pno_angle/45;
    endangle = (360-endangle)*pno_angle/45;

    if (stangle == 0) 
        stangle=1;

    if (endangle == 0) 
        endangle=1;

    pno_angle = 0;                                                            // ��ʼ˳ʱ�뻭������359�ȿ�ʼ(��4����)

    draw_x = x+r;
    draw_y = y;         
    op_x = r;
    op_y = 0;

    while (1)
    {
        op_y++;                                                                // ������һ��
        draw_y--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)             // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_x--;
            draw_x--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);

            GUI_Line(x, y, draw_x, draw_y, color);
        } 

        if (op_y >= op_x)
        {  
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
            break;
        }
    }

    while (1)
    {
        op_x--;                                                                // ������һ��
        draw_x--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)            // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_y++;
            draw_y--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);

            GUI_Line(x, y, draw_x, draw_y, color);
        } 

        if (op_x <= 0)
        { 
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);        // ��ʼ��ͼ
            break;
        }
    }

    draw_y = y-r;                                                            // ��ʼ˳ʱ�뻭������269�ȿ�ʼ(��3����)
    draw_x = x;         
    op_y = r;
    op_x = 0;

    while (1)
    {
        op_x++;                                                                // ������һ��
        draw_x--;
        
        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_y +1) > 0)            // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_y--;
            draw_y++;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);

            GUI_Line(x, y, draw_x, draw_y, color);
        } 

        if (op_x >= op_y)
        { 
            if (draw_on == 1)
                GUI_Point(draw_x, draw_y, color);        // ��ʼ��ͼ
            break;
        }
    }

    while (1)
    { 
        op_y--;                                                                // ������һ��
        draw_y++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_x +1) <= 0)            // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_x++;
            draw_x--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ
        
        pno_angle++;

        if ( (pno_angle == stangle)||(pno_angle == endangle) )            // �����������յ㣬���㿪��ȡ��
        {  
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);

            GUI_Line(x, y, draw_x, draw_y, color);
        } 

        if (op_y <= 0)
        { 
            if (draw_on == 1)
                GUI_Point(draw_x, draw_y, color);
            break;
        }
    }

    draw_x = x-r;                                                            // ��ʼ˳ʱ�뻭������179�ȿ�ʼ(��2����)
    draw_y = y;         
    op_x = r;
    op_y = 0;

    while (1)
    {
        op_y++;                                                                // ������һ��
        draw_y++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)             // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_x--;
            draw_x++;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);

            GUI_Line(x, y, draw_x, draw_y, color);
        } 

        if (op_y >= op_x)
        { 
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
            break;
        }
    }

    while (1)
    {
        op_x--;                                                                // ������һ��
        draw_x++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)            // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        {  
            op_y++;
            draw_y++;
        }

        if (draw_on == 1)
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ( (pno_angle == stangle)||(pno_angle == endangle) )            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);

            GUI_Line(x, y, draw_x, draw_y, color);
        } 

        if (op_x <= 0)
        {  
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);        // ��ʼ��ͼ
            break;
        }
    }

    draw_y = y+r;                                                            // ��ʼ˳ʱ�뻭������89�ȿ�ʼ(��1����)
    draw_x = x;         
    op_y = r;
    op_x = 0;

    while (1)
    {
        op_x++;                                                                // ������һ��
        draw_x++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_y +1) > 0)            // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_y--;
            draw_y--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);

            GUI_Line(x, y, draw_x, draw_y, color);
        } 

        if (op_x >= op_y)
        { 
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);        // ��ʼ��ͼ
            break;
        }
    }

    while (1)
    {
        op_y--;                                                                // ������һ��
        draw_y--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_x +1) <= 0)            // ʹ�����ȽϷ�ʵ�ֻ�Բ��
        { 
            op_x++;
            draw_x++;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // ��ʼ��ͼ

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // �����������յ㣬���㿪��ȡ��
        {  
            draw_on = 1-draw_on;

            if (draw_on == 1)
                GUI_Point(draw_x, draw_y, color);

            GUI_Line(x, y, draw_x, draw_y, color);
        } 

        if (op_y <= 0)
        { 
            if (draw_on == 1)
                GUI_Point(draw_x, draw_y, color);
            break;
        }
    }   
}





