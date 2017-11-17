/*****************************************************************************************
版权所有:        ---------------  
版本号:            1.0.0
生成日期:        2008.08.31
文件名:            gui_basic.c
作者:            影舞者
功能说明:        GUI基本画图函数
其它说明:       无   
所属文件关系:    本文件为工程规约代码文件

修改记录:
记录1:
修改者:
修改日期:
修改内容:
修改原因:
*****************************************************************************************/

#define  GUI_BASIC_GLOBALS                                        -
#include "app_types.h"

        


/*****************************************************************************************
函数名称:    void  GUI_HLine(INT16U x0, INT16U y0, INT16U x1) 
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    画水平线
输入参数:    x0        水平线起点所在列的位置
            y0        水平线起点所在行的位置
            x1      水平线终点所在列的位置
            color    显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_HLine(INT16U x0, INT16U y0, INT16U x1, uint16 color) 
{  
    INT16U  bak;

    if (x0 > x1)                                                             // 对x0、x1大小进行排列，以便画图
    { 
        bak = x1;
        x1 = x0;
        x0 = bak;
    }

    GUI_DispColor(x0, x1, y0, y0, color);
}




/*****************************************************************************************
函数名称:    void  GUI_VLine(INT16U x0, INT16U y0, INT16U y1) 
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    画竖直线。根据硬件特点，实现加速
输入参数:    x0        垂直线起点所在列的位置
            y0        垂直线起点所在行的位置
            y1      垂直线终点所在行的位置
            color    显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_VLine(INT16U x0, INT16U y0, INT16U y1, uint16 color) 
{ 
    INT16U  bak;
    
    if (y0 > y1)                                                             // 对y0、y1大小进行排列，以便画图
    { 
        bak = y1;
        y1 = y0;
        y0 = bak;
    }

    GUI_DispColor(x0, x0, y0, y1, color);    
}




/*****************************************************************************************
函数名称:    void  GUI_DrawPoint(INT16U x, INT16U y)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    画点
输入参数:    x0        矩形左上角的x坐标值
            y0        矩形左上角的y坐标值
            color    显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_DrawPoint(INT16U x, INT16U y, uint16 color)
{ 
    GUI_Point(x, y, color);
}




/*****************************************************************************************
函数名称:    void  GUI_Rectangle(INT16U x0, INT16U y0, INT16U x1, INT16U y1)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    画矩形
输入参数:    x0        矩形左上角的x坐标值
            y0        矩形左上角的y坐标值
            x1      矩形右下角的x坐标值
            y1      矩形右下角的y坐标值
            color    显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_Rectangle(INT16U x0, INT16U y0, INT16U x1, INT16U y1, uint16 color)
{ 
    GUI_HLine(x0, y0, x1, color);
    GUI_HLine(x0, y1, x1, color);
    GUI_VLine(x0, y0, y1, color);
    GUI_VLine(x1, y0, y1, color);
}




/*****************************************************************************************
函数名称:    void  GUI_RectangleFill(INT16U x0, INT16U y0, INT16U x1, INT16U y1)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    填充矩形。画一个填充的矩形，填充色与边框色一样
输入参数:    x0        矩形左上角的x坐标值
            y0        矩形左上角的y坐标值
            x1      矩形右下角的x坐标值
            y1      矩形右下角的y坐标值
            color    显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_RectangleFill(INT16U x0, INT16U y0, INT16U x1, INT16U y1, uint16 color)
{ 
    INT16U  i;

    if (x0 > x1)                                                             // 若x0>x1，则x0与x1交换
    { 
        i = x0;
        x0 = x1;
        x1 = i;
    }

    if (y0 > y1)                                                            // 若y0>y1，则y0与y1交换
    {
        i = y0;
        y0 = y1;
        y1 = i;
    }

    if (y0 == y1)                                                            // 判断是否只是直线
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
        GUI_HLine(x0, y0, x1, color);                                        // 当前画水平线
    }
}




/*****************************************************************************************
函数名称:    void  GUI_Square(INT16U x0, INT16U y0, INT16U  with)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    画正方形
输入参数:    x0        正方形左上角的x坐标值
            y0        正方形左上角的y坐标值
            with    正方形的边长
            color    显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_Square(INT16U x0, INT16U y0, INT16U  with, uint16 color)
{  
    GUI_Rectangle(x0, y0, x0 + with, y0 + with, color);
}




/*****************************************************************************************
函数名称:    void  GUI_Line(INT16U x0, INT16U y0, INT16U x1, INT16U y1)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    画任意两点之间的直线
输入参数:    x0        矩形左上角的x坐标值
            y0        矩形左上角的y坐标值
            x1      矩形右下角的x坐标值
            y1      矩形右下角的y坐标值
            color    显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_Line(INT16U x0, INT16U y0, INT16U x1, INT16U y1, uint16 color)
{
    INT32S   dx;                                                            // 直线x轴差值变量
    INT32S   dy;                                                              // 直线y轴差值变量
    INT08S   dx_sym;                                                        // x轴增长方向，为-1时减值方向，为1时增值方向
    INT08S   dy_sym;                                                        // y轴增长方向，为-1时减值方向，为1时增值方向
    INT32S   dx_x2;                                                            // dx*2值变量，用于加快运算速度
    INT32S   dy_x2;                                                            // dy*2值变量，用于加快运算速度
    INT32S   di;                                                            // 决策变量
    INT16U   bak;

    if (x0 > x1)                                                             // 对x0、x1大小进行排列，以便画图
    { 
        bak = x1;
        x1 = x0;
        x0 = bak;
    }
    
    if (y0 > y1)                                                             // 对y0、y1大小进行排列，以便画图
    { 
        bak = y1;
        y1 = y0;
        y0 = bak;
    }
    
    dx = x1-x0;                                                                // 求取两点之间的差值
    dy = y1-y0;

    // 判断增长方向，或是否为水平线、垂直线、点

    if (dx>0)                                                                // 判断x轴方向
    {  
        dx_sym = 1;                                                            // dx>0，设置dx_sym=1
    }
    else
    { 
        if (dx<0)
        { 
            dx_sym = -1;                                                    // dx<0，设置dx_sym=-1
        }
        else
        { 
            GUI_VLine(x0, y0, y1, color);                                    // dx==0，画垂直线，或一点
            return;
        }
    }

    if (dy>0)                                                                // 判断y轴方向
    {  
        dy_sym = 1;                                                            // dy>0，设置dy_sym=1
    }
    else
    { 
        if (dy<0)
        {  
            dy_sym = -1;                                                    // dy<0，设置dy_sym=-1
        }
        else
        {
            GUI_HLine(x0, y0, x1, color);                                    // dy==0，画水平线，或一点
            return;
        }
    }

    dx = dx_sym * dx;                                                        // 将dx、dy取绝对值
    dy = dy_sym * dy;

    dx_x2 = dx*2;                                                            // 计算2倍的dx及dy值
    dy_x2 = dy*2;

    // 使用Bresenham法进行画直线

    if (dx>=dy)                                                                // 对于dx>=dy，则使用x轴为基准
    { 
        di = dy_x2 - dx;

        while (x0!=x1)
        { 
            GUI_Point(x0, y0, color);
            x0 += dx_sym;

            if (di<0)
            { 
                di += dy_x2;                                                // 计算出下一步的决策值
            }
            else
            { 
                di += dy_x2 - dx_x2;
                y0 += dy_sym;
            }
        }

        GUI_Point(x0, y0, color);                        // 显示最后一点
    }
    else                                                                    // 对于dx<dy，则使用y轴为基准
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

        GUI_Point(x0, y0, color);                        // 显示最后一点
    }   
}




/*****************************************************************************************
函数名称:    void  GUI_LineDashed(INT16U x0, INT16U y0, INT16U x1, INT16U y1, INT16U dashed1, INT16U dashed2)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    画任意两点之间的直线
输入参数:    x0        左上角的x坐标值
            y0        左上角的y坐标值
            x1      右下角的x坐标值
            y1      右下角的y坐标值
            dashed1 亮段长度
            dashed2 暗段长度
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_LineDashed(INT16U x0, INT16U y0, INT16U x1, INT16U y1, INT16U dashed1, INT16U dashed2, uint16 color)
{
    INT16U  x, y, c;

    // 必须是直线
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
    
    // 竖线
    if (x0 == x1)
    {
        y = y0;
        c = 0;

        while (y1 > y)
        {
            // 画前景线
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

            // 是背景色, 不需要画
            if (c == 1)
            {
                c = 0;
                y = y + dashed2;
                continue;
            }
        }

        return;
    }

    // 横线
    if (y0 == y1)
    {
        x = x0;
        c = 0;

        while (x1 > x)
        {
            // 画前景线
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

            // 是背景色, 不需要画
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
函数名称:    void  GUI_LineWith(INT32U x0, INT32U y0, INT32U x1, INT32U y1, INT08U with)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    画任意两点之间的直线，并且可设置线的宽度
输入参数:    x0        矩形左上角的x坐标值
            y0        矩形左上角的y坐标值
            x1      矩形右下角的x坐标值
            y1      矩形右下角的y坐标值
            color    显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_LineWith(INT16U x0, INT16U y0, INT16U x1, INT16U y1, INT16U with, uint16 color)
{ 
    INT32S   dx;                                                            // 直线x轴差值变量
    INT32S   dy;                                                              // 直线y轴差值变量
    INT08S    dx_sym;                                                        // x轴增长方向，为-1时减值方向，为1时增值方向
    INT08S    dy_sym;                                                        // y轴增长方向，为-1时减值方向，为1时增值方向
    INT32S   dx_x2;                                                            // dx*2值变量，用于加快运算速度
    INT32S   dy_x2;                                                            // dy*2值变量，用于加快运算速度
    INT32S   di;                                                            // 决策变量
    INT32S   wx, wy;                                                        // 线宽变量
    INT32S   draw_a, draw_b;

    if (with == 0) 
        return;

    if (with > 50)
        with = 50;

    dx = x1-x0;                                                                // 求取两点之间的差值
    dy = y1-y0;

    wx = with/2;
    wy = with-wx-1;

    // 判断增长方向，或是否为水平线、垂直线、点

    if (dx > 0)                                                                // 判断x轴方向
    {  
        dx_sym = 1;                                                            // dx>0，设置dx_sym=1
    }
    else
    { 
        if (dx < 0)
        { 
            dx_sym = -1;                                                    // dx<0，设置dx_sym=-1
        }
        else                                                                // dx==0，画垂直线，或一点
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

    if (dy > 0)                                                                // 判断y轴方向
    {  
        dy_sym = 1;                                                            // dy>0，设置dy_sym=1
    }
    else
    {  
        if (dy < 0)
        {  
            dy_sym = -1;                                                    // dy<0，设置dy_sym=-1
        }
        else                                                                  // dy==0，画水平线，或一点
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

    dx = dx_sym * dx;                                                       // 将dx、dy取绝对值
    dy = dy_sym * dy;

    dx_x2 = dx*2;                                                            // 计算2倍的dx及dy值
    dy_x2 = dy*2;

    // 使用Bresenham法进行画直线

    if (dx >= dy)                                                            // 对于dx>=dy，则使用x轴为基准
    { 
        di = dy_x2 - dx;

        while (x0 != x1)                                                    // x轴向增长，则宽度在y方向，即画垂直线
        {
            draw_a = y0-wx;

            if (draw_a < 0) 
                draw_a = 0;

            draw_b = y0+wy;
            GUI_VLine(x0, draw_a, draw_b, color);

            x0 += dx_sym;    

            if (di < 0)
            { 
                di += dy_x2;                                                // 计算出下一步的决策值
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
    else                                                                    // 对于dx<dy，则使用y轴为基准
    { 
        di = dx_x2 - dy;

        while (y0 != y1)                                                    // y轴向增长，则宽度在x方向，即画水平线
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
函数名称:    void  GUI_Circle(INT32U x0, INT32U y0, INT32U r)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    指定圆心位置及半径，画圆
输入参数:    x0        圆心的x坐标值
            y0        圆心的y坐标值
            r       圆的半径
            color    显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_Circle(INT16U x0, INT16U y0, INT16U r, uint16 color)
{
    INT32S  draw_x0, draw_y0;                                                // 刽图点坐标变量
    INT32S  draw_x1, draw_y1;    
    INT32S  draw_x2, draw_y2;    
    INT32S  draw_x3, draw_y3;    
    INT32S  draw_x4, draw_y4;    
    INT32S  draw_x5, draw_y5;    
    INT32S  draw_x6, draw_y6;    
    INT32S  draw_x7, draw_y7;    
    INT32S  xx, yy;                                                            // 画圆控制变量
    INT32S  di;                                                                // 决策变量

    if (0 == r) 
        return;

    // 计算出8个特殊点(0、45、90、135、180、225、270度)，进行显示

    draw_x0 = draw_x1 = x0;
    draw_y0 = draw_y1 = y0 + r;

    if (draw_y0 < BOARD_LCD_HEIGHT) 
        GUI_Point(draw_x0, draw_y0, color);                // 90度

    draw_x2 = draw_x3 = x0;
    draw_y2 = draw_y3 = y0 - r;

    if (draw_y2 >= 0) 
        GUI_Point(draw_x2, draw_y2, color);                // 270度


    draw_x4 = draw_x6 = x0 + r;
    draw_y4 = draw_y6 = y0;

    if (draw_x4 < BOARD_LCD_WIDTH)
        GUI_Point(draw_x4, draw_y4, color);                // 0度

    draw_x5 = draw_x7 = x0 - r;
    draw_y5 = draw_y7 = y0;

    if (draw_x5 >= 0) 
        GUI_Point(draw_x5, draw_y5, color);                // 180度  

    if (1 == r) 
        return;                                                                // 若半径为1，则已圆画完

    // 使用Bresenham法进行画圆

    di = 3 - 2*r;                                                            // 初始化决策变量

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

        // 要判断当前点是否在有效范围内

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
函数名称:    void  GUI_CircleFill(INT32U x0, INT32U y0, INT32U r)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    指定圆心位置及半径，画圆并填充，填充色与边框色一样
输入参数:    x0        圆心的x坐标值
            y0        圆心的y坐标值
            r       圆的半径
            color    填充颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_CircleFill(INT16U x0, INT16U y0, INT16U r, uint16 color)
{ 
    INT32S  draw_x0, draw_y0;                                                // 刽图点坐标变量
    INT32S  draw_x1, draw_y1;    
    INT32S  draw_x2, draw_y2;    
    INT32S  draw_x3, draw_y3;    
    INT32S  draw_x4, draw_y4;    
    INT32S  draw_x5, draw_y5;    
    INT32S  draw_x6, draw_y6;    
    INT32S  draw_x7, draw_y7;    
    INT32S  fill_x0, fill_y0;                                                // 填充所需的变量，使用垂直线填充
    INT32S  fill_x1;
    INT32S  xx, yy;                                                            // 画圆控制变量
    INT32S  di;                                                                // 决策变量

    if (0 == r) 
        return;

    // 计算出4个特殊点(0、90、180、270度)，进行显示

    draw_x0 = draw_x1 = x0;
    draw_y0 = draw_y1 = y0 + r;

    if (draw_y0 < BOARD_LCD_HEIGHT)
        GUI_Point(draw_x0, draw_y0, color);                // 90度

    draw_x2 = draw_x3 = x0;
    draw_y2 = draw_y3 = y0 - r;

    if (draw_y2 >= 0)
        GUI_Point(draw_x2, draw_y2, color);                // 270度

    draw_x4 = draw_x6 = x0 + r;
    draw_y4 = draw_y6 = y0;

    if (draw_x4 < BOARD_LCD_WIDTH) 
    { 
        GUI_Point(draw_x4, draw_y4, color);                // 0度
        fill_x1 = draw_x4;
    }
    else
    {  
        fill_x1 = BOARD_LCD_WIDTH;
    }

    fill_y0 = y0;                                                            // 设置填充线条起始点fill_x0
    fill_x0 = x0 - r;                                                        // 设置填充线条结束点fill_y1

    if (fill_x0 < 0)
        fill_x0 = 0;

    GUI_HLine(fill_x0, fill_y0, fill_x1, color);

    draw_x5 = draw_x7 = x0 - r;
    draw_y5 = draw_y7 = y0;

    if (draw_x5 >= 0) 
        GUI_Point(draw_x5, draw_y5, color);                // 180度

    if (1 == r) 
        return;

    // 使用Bresenham法进行画圆

    di = 3 - 2*r;                                                            // 初始化决策变量

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

        if ((draw_x0 <= BOARD_LCD_WIDTH) && (draw_y0 >= 0))                    // 要判断当前点是否在有效范围内
            GUI_Point(draw_x0, draw_y0, color);

        if ((draw_x1 >= 0) && (draw_y1 >= 0))    
            GUI_Point(draw_x1, draw_y1, color);

        if (draw_x1 >= 0)                                                    // 第二点水直线填充(下半圆的点)
        {
            fill_x0 = draw_x1;                                                // 设置填充线条起始点fill_x0

            fill_y0 = draw_y1;                                                // 设置填充线条起始点fill_y0

            if (fill_y0 > BOARD_LCD_HEIGHT) 
                fill_y0 = BOARD_LCD_HEIGHT;

            if (fill_y0 < 0) 
                fill_y0 = 0; 

            fill_x1 = x0*2 - draw_x1;                                        // 设置填充线条结束点fill_x1

            if (fill_x1 > BOARD_LCD_WIDTH) 
                fill_x1 = BOARD_LCD_WIDTH;

            GUI_HLine(fill_x0, fill_y0, fill_x1, color);
        }


        if ((draw_x2 <= BOARD_LCD_WIDTH) && (draw_y2 <= BOARD_LCD_HEIGHT))    
            GUI_Point(draw_x2, draw_y2, color);   

        if ((draw_x3 >= 0) && (draw_y3 <= BOARD_LCD_HEIGHT))    
            GUI_Point(draw_x3, draw_y3, color);


        if (draw_x3 >= 0)                                                    // 第四点垂直线填充(上半圆的点)
        {
            fill_x0 = draw_x3;                                                // 设置填充线条起始点fill_x0

            fill_y0 = draw_y3;                                                // 设置填充线条起始点fill_y0

            if (fill_y0 > BOARD_LCD_HEIGHT) 
                fill_y0 = BOARD_LCD_HEIGHT;

            if (fill_y0 < 0) 
                fill_y0 = 0;

            fill_x1 = x0*2 - draw_x3;                                        // 设置填充线条结束点fill_x1    

            if (fill_x1 > BOARD_LCD_WIDTH)
                fill_x1 = BOARD_LCD_WIDTH;

            GUI_HLine(fill_x0, fill_y0, fill_x1, color);
        }


        if ((draw_x4 <= BOARD_LCD_WIDTH) && (draw_y4 >= 0))    
            GUI_Point(draw_x4, draw_y4, color);

        if ((draw_x5 >= 0) && (draw_y5 >= 0))    
            GUI_Point(draw_x5, draw_y5, color);



        if (draw_x5 >= 0)                                                    // 第六点垂直线填充(上半圆的点)
        { 
            fill_x0 = draw_x5;                                                // 设置填充线条起始点fill_x0

            fill_y0 = draw_y5;                                                // 设置填充线条起始点fill_y0

            if (fill_y0 > BOARD_LCD_HEIGHT) 
                fill_y0 = BOARD_LCD_HEIGHT;

            if (fill_y0 < 0) 
                fill_y0 = 0;

            fill_x1 = x0*2 - draw_x5;                                        // 设置填充线条结束点fill_x1    

            if (fill_x1 > BOARD_LCD_WIDTH)
                fill_x1 = BOARD_LCD_WIDTH;

            GUI_HLine(fill_x0, fill_y0, fill_x1, color);
        }


        if ((draw_x6 <= BOARD_LCD_WIDTH) && (draw_y6 <= BOARD_LCD_HEIGHT))    
            GUI_Point(draw_x6, draw_y6, color);


        if ((draw_x7 >= 0) && (draw_y7 <= BOARD_LCD_HEIGHT))    
            GUI_Point(draw_x7, draw_y7, color);


        if (draw_x7 >= 0)                                                    // 第八点垂直线填充(上半圆的点)
        { 
            fill_x0 = draw_x7;                                                // 设置填充线条起始点fill_x0

            fill_y0 = draw_y7;                                                // 设置填充线条起始点fill_y0

            if (fill_y0 > BOARD_LCD_HEIGHT) 
                fill_y0 = BOARD_LCD_HEIGHT;

            if (fill_y0 < 0)
                fill_y0 = 0;

            fill_x1 = x0*2 - draw_x7;                                        // 设置填充线条结束点fill_x1    

            if (fill_x1 > BOARD_LCD_WIDTH) 
                fill_x1 = BOARD_LCD_WIDTH;

            GUI_HLine(fill_x0, fill_y0, fill_x1, color);
        }      
    }
}




/*****************************************************************************************
函数名称:    void  GUI_Ellipse(INT32U x0, INT32U x1, INT32U y0, INT32U y1)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    画正椭圆。给定椭圆的四个点的参数，最左、最右点的x轴坐标值为x0、x1，最上、最下点
            的y轴坐标为y0、y1
输入参数:    x0        矩形左上角的x坐标值
            y0        矩形左上角的y坐标值
            x1      矩形右下角的x坐标值
            y1      矩形右下角的y坐标值
            color    显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_Ellipse(INT16U x0, INT16U x1, INT16U y0, INT16U y1, uint16 color)
{ 
    INT32S  draw_x0, draw_y0;                                                // 刽图点坐标变量
    INT32S  draw_x1, draw_y1;
    INT32S  draw_x2, draw_y2;
    INT32S  draw_x3, draw_y3;
    INT32S  xx, yy;                                                            // 画图控制变量
    INT32S  center_x, center_y;                                            // 椭圆中心点坐标变量
    INT32S  radius_x, radius_y;                                            // 椭圆的半径，x轴半径和y轴半径
    INT32S  radius_xx, radius_yy;                                            // 半径乘平方值
    INT32S  radius_xx2, radius_yy2;                                        // 半径乘平方值的两倍
    INT32S  di;                                                                // 定义决策变量

    if ((x0 == x1) || (y0 == y1)) 
        return;

    center_x = (x0 + x1) >> 1;                                                // 计算出椭圆中心点坐标
    center_y = (y0 + y1) >> 1;

    if (x0 > x1)                                                            // 计算出椭圆的半径，x轴半径和y轴半径
        radius_x = (x0 - x1) >> 1;
    else
        radius_x = (x1 - x0) >> 1;

    if (y0 > y1)
        radius_y = (y0 - y1) >> 1;
    else
        radius_y = (y1 - y0) >> 1;

    radius_xx = radius_x * radius_x;                                        // 计算半径平方值
    radius_yy = radius_y * radius_y;

    radius_xx2 = radius_xx<<1;                                                // 计算半径平方值乘2值
    radius_yy2 = radius_yy<<1;

    xx = 0;                                                                    // 初始化画图变量
    yy = radius_y;

    di = radius_yy2 + radius_xx - radius_xx2*radius_y ;                        // 初始化决策变量 

    draw_x0 = draw_x1 = draw_x2 = draw_x3 = center_x;                        // 计算出椭圆y轴上的两个端点坐标，作为作图起点
    draw_y0 = draw_y1 = center_y + radius_y;
    draw_y2 = draw_y3 = center_y - radius_y;

    GUI_Point(draw_x0, draw_y0, color);                    // 画y轴上的两个端点 
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

        xx ++;                                                                // x轴加1

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

            xx ++;                                                            // x轴加1             
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
函数名称:    void  GUI_EllipseFill(INT32U x0, INT32U x1, INT32U y0, INT32U y1)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    画正椭圆，并填充。给定椭圆的四个点的参数，最左、最右点的x轴坐标值为x0、x1，最上、最下点
            的y轴坐标为y0、y1
输入参数:    x0        矩形左上角的x坐标值
            y0        矩形左上角的y坐标值
            x1      矩形右下角的x坐标值
            y1      矩形右下角的y坐标值
            color    显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_EllipseFill(INT16U x0, INT16U x1, INT16U y0, INT16U y1, uint16 color)
{ 
    INT32S  draw_x0, draw_y0;                                                // 刽图点坐标变量
    INT32S  draw_x1, draw_y1;
    INT32S  draw_x2, draw_y2;
    INT32S  draw_x3, draw_y3;
    INT32S  xx, yy;                                                            // 画图控制变量
    INT32S  center_x, center_y;                                            // 椭圆中心点坐标变量
    INT32S  radius_x, radius_y;                                            // 椭圆的半径，x轴半径和y轴半径
    INT32S  radius_xx, radius_yy;                                            // 半径乘平方值
    INT32S  radius_xx2, radius_yy2;                                        // 半径乘平方值的两倍
    INT32S  di;                                                                // 定义决策变量

    if ((x0 == x1) || (y0 == y1))
        return;

    center_x = (x0 + x1) >> 1;                                                // 计算出椭圆中心点坐标        
    center_y = (y0 + y1) >> 1;

    if (x0 > x1)                                                            // 计算出椭圆的半径，x轴半径和y轴半径
        radius_x = (x0 - x1) >> 1;
    else
        radius_x = (x1 - x0) >> 1;


    if (y0 > y1)
        radius_y = (y0 - y1) >> 1;
    else
        radius_y = (y1 - y0) >> 1;

    radius_xx = radius_x * radius_x;                                        // 计算半径乘平方值
    radius_yy = radius_y * radius_y;

    radius_xx2 = radius_xx<<1;                                                // 计算半径乘4值
    radius_yy2 = radius_yy<<1;

    xx = 0;                                                                    // 初始化画图变量
    yy = radius_y;

    di = radius_yy2 + radius_xx - radius_xx2*radius_y ;                        // 初始化决策变量 

    draw_x0 = draw_x1 = draw_x2 = draw_x3 = center_x;                        // 计算出椭圆y轴上的两个端点坐标，作为作图起点
    draw_y0 = draw_y1 = center_y + radius_y;
    draw_y2 = draw_y3 = center_y - radius_y;

    GUI_Point(draw_x0, draw_y0, color);                    // 画y轴上的两个端点
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

        xx ++;                                                                // x轴加1

        draw_x0++;
        draw_x1--;
        draw_x2++;
        draw_x3--;

        GUI_Point(draw_x0, draw_y0, color);
        GUI_Point(draw_x1, draw_y1, color);
        GUI_Point(draw_x2, draw_y2, color);
        GUI_Point(draw_x3, draw_y3, color);

        if (di >= 0)                                                        // 若y轴已变化，进行填充
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

            xx ++;                                                            // x轴加1             
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

        GUI_HLine(draw_x0, draw_y0, draw_x1, color);                            // y轴已变化，进行填充
        GUI_HLine(draw_x2, draw_y2, draw_x3, color); 
    }     
}




/*****************************************************************************************
函数名称:    void  GUI_Arc4(INT32U x, INT32U y, INT32U r, INT08U angle)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    画弧。起点及终点只能为0度-90度、90度-180度、180度-270度、270度-0度等。即分别
            为第1-4像限的90度弧
输入参数:    x0        圆心的x坐标值
            y0        圆心的y坐标值
            r       圆弧的半径
            angle    画弧的像限(1-4)
            color    显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_Arc4(INT16U x, INT16U y, INT16U r, INT16U angle, uint16 color)
{ 
    INT32S  draw_x, draw_y;
    INT32S  op_x, op_y;
    INT32S  op_2rr;

    if (r == 0)
        return;

    op_2rr = 2*r*r;                                                            // 计算r平方乖以2

    switch(angle)
    { 
        case  1:
                draw_x = x+r;
                draw_y = y;

                op_x = r;
                op_y = 0;

                while (1)
                {  
                    GUI_Point(draw_x, draw_y, color);            // 开始画图

                    op_y++;                                                            // 计算下一点
                    draw_y++;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)        // 使用逐点比较法实现画圆弧
                    { 
                        op_x--;
                        draw_x--;
                    }

                    if (op_y>=op_x) 
                        break;
                }

                while (1)
                { 
                    GUI_Point(draw_x, draw_y, color);            // 开始画图
                
                    op_x--;                                                            // 计算下一点
                    draw_x--;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)        // 使用逐点比较法实现画圆弧
                    {  
                        op_y++;
                        draw_y++;
                    }

                    if (op_x <= 0)
                    {  
                        GUI_Point(draw_x, draw_y, color);        // 开始画图
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
                    GUI_Point(draw_x, draw_y, color);            // 开始画图

                    op_y++;                                                            // 计算下一点
                    draw_y++;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)        // 使用逐点比较法实现画圆弧
                    {  
                        op_x--;
                        draw_x++;
                    }

                    if (op_y >= op_x) 
                        break;
                }

                while (1)
                { 
                    GUI_Point(draw_x, draw_y, color);            // 开始画图

                    op_x--;                                                            // 计算下一点
                    draw_x++;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)        // 使用逐点比较法实现画圆弧
                    { 
                        op_y++;
                        draw_y++;
                    }

                    if (op_x <= 0)
                    { 
                        GUI_Point(draw_x, draw_y, color);        // 开始画图
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
                    GUI_Point(draw_x, draw_y, color);            // 开始画图

                    op_y++;                                                            // 计算下一点
                    draw_y--;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)        // 使用逐点比较法实现画圆弧
                    {  
                        op_x--;
                        draw_x++;
                    }

                    if (op_y >= op_x)
                        break;
                }

                while (1)
                {  
                    GUI_Point(draw_x, draw_y, color);            // 开始画图

                    op_x--;                                                            // 计算下一点
                    draw_x++;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)        // 使用逐点比较法实现画圆弧
                    {  
                        op_y++;
                        draw_y--;
                    }

                    if (op_x <= 0)
                    {  
                        GUI_Point(draw_x, draw_y, color);        // 开始画图
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
                    GUI_Point(draw_x, draw_y, color);            // 开始画图

                    op_y++;                                                            // 计算下一点
                    draw_y--;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)        // 使用逐点比较法实现画圆弧
                    { 
                        op_x--;
                        draw_x--;
                    }

                    if (op_y >= op_x)
                        break;
                }

                while (1)
                { 
                    GUI_Point(draw_x, draw_y, color);            // 开始画图

                    op_x--;                                                            // 计算下一点
                    draw_x--;

                    if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)        // 使用逐点比较法实现画圆弧
                    { 
                        op_y++;
                        draw_y--;
                    }

                    if (op_x <= 0)
                    { 
                        GUI_Point(draw_x, draw_y, color);        // 开始画图
                        break;
                    }
                }
                break;

        default:
                break;
    }
}




/*****************************************************************************************
函数名称:    void  GUI_Arc(INT32U x, INT32U y, INT32U r, INT32U stangle, INT32U endangle)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    指定起点、终点及半径画弧(不能画圆)。使用的是顺时针方向画图
输入参数:    x            圆心的x轴坐标值
            y            圆心的y轴坐标值
            stangle     起始角度(0-359度)
            endangle    终止角度(0-359度)
            r              圆的半径终点
            color        显示颜色
输出参数:    无
其它说明:    无
*****************************************************************************************/
void  GUI_Arc(INT16U x, INT16U y, INT16U r, INT16U stangle, INT16U endangle, uint16 color)
{ 
    INT32S  draw_x, draw_y;                                                // 画图坐标变量
    INT32S  op_x, op_y;                                                    // 操作坐标
    INT32S  op_2rr;                                                            // 2*r*r值变量
    INT32S  pno_angle;                                                        // 度角点的个数
    INT08U  draw_on;                                                        // 画点开关，为1时画点，为0时不画

    if (r == 0) 
        return;                                                                // 半径为0则直接退出

    if (stangle == endangle) 
        return;                                                                // 起始角度与终止角度相同，退出

    if ((stangle >= 360) || (endangle >= 360))
        return;

    op_2rr = 2*r*r;                                                            // 计算r平方乖以2
    pno_angle = 0;

    op_x = r;                                                                // 先计算出在此半径下的45度的圆弧的点数  
    op_y = 0;

    while (1)
    { 
        pno_angle++;                                                         // 画点计数         

        op_y++;                                                                // 计算下一点

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)             // 使用逐点比较法实现画圆弧
            op_x--;

        if (op_y >= op_x) 
            break;
    }

    draw_on = 0;                                                            // 最开始关画点开关


    if (endangle > stangle) 
        draw_on = 1;                                                        // 若终点大于起点，则从一开始即画点(359)

    stangle = (360-stangle)*pno_angle/45;
    endangle = (360-endangle)*pno_angle/45;

    if (stangle == 0) 
        stangle=1;

    if (endangle == 0) 
        endangle=1;


    pno_angle = 0;                                                            // 开始顺时针画弧，从359度开始(第4像限)

    draw_x = x+r;
    draw_y = y;         
    op_x = r;
    op_y = 0;

    while (1)
    { 
        op_y++;                                                                // 计算下一点
        draw_y--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)             // 使用逐点比较法实现画圆弧
        { 
            op_x--;
            draw_x--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
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
        op_x--;                                                                // 计算下一点
        draw_x--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)            // 使用逐点比较法实现画圆弧
        {  
            op_y++;
            draw_y--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
        } 

        if (op_x <= 0)
        { 
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);        // 开始画图

            break;
        }
    }

    draw_y = y-r;                                                            // 开始顺时针画弧，从269度开始(第3像限)
    draw_x = x;         
    op_y = r;
    op_x = 0;

    while (1)
    {
        op_x++;                                                                // 计算下一点
        draw_x--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_y +1) > 0)            // 使用逐点比较法实现画圆弧
        { 
            op_y--;
            draw_y++;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
        } 

        if (op_x >= op_y)
        {  
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);        // 开始画图
            break;
        }
    }

    while (1)
    {
        op_y--;                                                                // 计算下一点
        draw_y++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_x +1) <= 0)            // 使用逐点比较法实现画圆弧
        {  
            op_x++;
            draw_x--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
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

    draw_x = x-r;                                                            // 开始顺时针画弧，从179度开始(第2像限)
    draw_y = y;         
    op_x = r;
    op_y = 0;

    while (1)
    {
        op_y++;                                                                // 计算下一点
        draw_y++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)             // 使用逐点比较法实现画圆弧
        { 
            op_x--;
            draw_x++;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
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
        op_x--;                                                                // 计算下一点
        draw_x++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)            // 使用逐点比较法实现画圆弧
        { 
            op_y++;
            draw_y++;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1)
                GUI_Point(draw_x, draw_y, color);
        } 

        if (op_x <= 0)
        {
            if (draw_on == 1)
                GUI_Point(draw_x, draw_y, color);        // 开始画图
            break;
        }
    }

    draw_y = y+r;                                                            // 开始顺时针画弧，从89度开始(第1像限)
    draw_x = x;         
    op_y = r;
    op_x = 0;

    while (1)
    {
        op_x++;                                                                // 计算下一点
        draw_x++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_y +1) > 0)            // 使用逐点比较法实现画圆弧
        { 
            op_y--;
            draw_y--;
        }

        if (draw_on == 1)
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
        {  
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);
        } 

        if (op_x >= op_y)
        {
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);        // 开始画图
            break;
        }
    }

    while (1)
    {
        op_y--;                                                                // 计算下一点
        draw_y--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_x +1) <= 0)            // 使用逐点比较法实现画圆弧
        { 
            op_x++;
            draw_x++;
        }

        if (draw_on == 1)
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
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
函数名称:    void  GUI_Pieslice(INT32U x, INT32U y, INT32U r, INT32U stangle, INT32U endangle)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    指定起点、终点及半径扇形(不能画圆)。使用的是顺时针方向画图
输入参数:    x            圆心的x轴坐标值
            y            圆心的y轴坐标值
            stangle     起始角度(0-359度)
            endangle    终止角度(0-359度)
            r              圆的半径终点
            color        显示颜色
输出参数:    无    
其它说明:    无
*****************************************************************************************/
void  GUI_Pieslice(INT16U x, INT16U y, INT16U r, INT16U stangle, INT16U endangle, uint16 color)
{ 
    INT32S  draw_x, draw_y;                                                // 画图坐标变量
    INT32S  op_x, op_y;                                                    // 操作坐标
    INT32S  op_2rr;                                                            // 2*r*r值变量
    INT32S  pno_angle;                                                        // 度角点的个数
    INT08U  draw_on;                                                        // 画点开关，为1时画点，为0时不画

    if (r == 0) 
        return;                                                                // 半径为0则直接退出

    if (stangle == endangle) 
        return;                                                                // 起始角度与终止角度相同，退出

    if ((stangle >= 360) || (endangle >= 360)) 
        return;

    op_2rr = 2*r*r;                                                            // 计算r平方乖以2
    pno_angle = 0;

    op_x = r;                                                                // 先计算出在此半径下的45度的圆弧的点数      
    op_y = 0;

    while (1)
    {  
        pno_angle++;                                                         // 画点计数         
        op_y++;                                                                // 计算下一点

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)             // 使用逐点比较法实现画圆弧
            op_x--;

        if (op_y >= op_x)
            break;
    }

    draw_on = 0;                                                            // 最开始关画点开关

    if (endangle > stangle) 
        draw_on = 1;                                                        // 若终点大于起点，则从一开始即画点(359)

    stangle = (360-stangle)*pno_angle/45;
    endangle = (360-endangle)*pno_angle/45;

    if (stangle == 0) 
        stangle=1;

    if (endangle == 0) 
        endangle=1;

    pno_angle = 0;                                                            // 开始顺时针画弧，从359度开始(第4像限)

    draw_x = x+r;
    draw_y = y;         
    op_x = r;
    op_y = 0;

    while (1)
    {
        op_y++;                                                                // 计算下一点
        draw_y--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)             // 使用逐点比较法实现画圆弧
        { 
            op_x--;
            draw_x--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
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
        op_x--;                                                                // 计算下一点
        draw_x--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)            // 使用逐点比较法实现画圆弧
        { 
            op_y++;
            draw_y--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);

            GUI_Line(x, y, draw_x, draw_y, color);
        } 

        if (op_x <= 0)
        { 
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);        // 开始画图
            break;
        }
    }

    draw_y = y-r;                                                            // 开始顺时针画弧，从269度开始(第3像限)
    draw_x = x;         
    op_y = r;
    op_x = 0;

    while (1)
    {
        op_x++;                                                                // 计算下一点
        draw_x--;
        
        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_y +1) > 0)            // 使用逐点比较法实现画圆弧
        { 
            op_y--;
            draw_y++;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);

            GUI_Line(x, y, draw_x, draw_y, color);
        } 

        if (op_x >= op_y)
        { 
            if (draw_on == 1)
                GUI_Point(draw_x, draw_y, color);        // 开始画图
            break;
        }
    }

    while (1)
    { 
        op_y--;                                                                // 计算下一点
        draw_y++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_x +1) <= 0)            // 使用逐点比较法实现画圆弧
        { 
            op_x++;
            draw_x--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图
        
        pno_angle++;

        if ( (pno_angle == stangle)||(pno_angle == endangle) )            // 若遇到起点或终点，画点开关取反
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

    draw_x = x-r;                                                            // 开始顺时针画弧，从179度开始(第2像限)
    draw_y = y;         
    op_x = r;
    op_y = 0;

    while (1)
    {
        op_y++;                                                                // 计算下一点
        draw_y++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_x +1) > 0)             // 使用逐点比较法实现画圆弧
        { 
            op_x--;
            draw_x++;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
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
        op_x--;                                                                // 计算下一点
        draw_x++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_y +1) <= 0)            // 使用逐点比较法实现画圆弧
        {  
            op_y++;
            draw_y++;
        }

        if (draw_on == 1)
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ( (pno_angle == stangle)||(pno_angle == endangle) )            // 若遇到起点或终点，画点开关取反
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);

            GUI_Line(x, y, draw_x, draw_y, color);
        } 

        if (op_x <= 0)
        {  
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);        // 开始画图
            break;
        }
    }

    draw_y = y+r;                                                            // 开始顺时针画弧，从89度开始(第1像限)
    draw_x = x;         
    op_y = r;
    op_x = 0;

    while (1)
    {
        op_x++;                                                                // 计算下一点
        draw_x++;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr - 2*op_y +1) > 0)            // 使用逐点比较法实现画圆弧
        { 
            op_y--;
            draw_y--;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
        { 
            draw_on = 1-draw_on;

            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);

            GUI_Line(x, y, draw_x, draw_y, color);
        } 

        if (op_x >= op_y)
        { 
            if (draw_on == 1) 
                GUI_Point(draw_x, draw_y, color);        // 开始画图
            break;
        }
    }

    while (1)
    {
        op_y--;                                                                // 计算下一点
        draw_y--;

        if ((2*op_x*op_x + 2*op_y*op_y - op_2rr + 2*op_x +1) <= 0)            // 使用逐点比较法实现画圆弧
        { 
            op_x++;
            draw_x++;
        }

        if (draw_on == 1) 
            GUI_Point(draw_x, draw_y, color);            // 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))            // 若遇到起点或终点，画点开关取反
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





