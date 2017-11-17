/*****************************************************************************************
版权所有:        ----------------  
版本号:            1.0.0
生成日期:        2008.08.31
文件名:            gui_font.c
作者:            影舞者
功能说明:        ASCII码字体及显示函数。
其它说明:        无
所属文件关系:    本文件为工程规约代码文件

修改记录:
记录1:
修改者:
修改日期:
修改内容:
修改原因:
*****************************************************************************************/

#define  GUI_FONT_GLOBALS                                        
#include "app_types.h"




/*****************************************************************************************
版权所有:   影舞者
版本号:     1.0.0
生成日期:   2008.08.31
功能说明:   定义十进制(0-7)==>十六进制位转换表，由于显示点数据是由左到右，所以十六进制位顺序是倒的
其它说明:	无
*****************************************************************************************/
const  INT08U  DCB2HEX_TAB[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};




/*****************************************************************************************
函数名称:    void  GUI_PutString(uint8 fontType, INT16U x, INT16U y, char *str)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    输出显示字符串(没有自动换行功能)
输入参数:    x        指定显示位置，x坐标
            y        指定显示位置，y坐标
            str        要显示的ASCII码字符串
输出参数:    无
其它说明:    操作失败原因是指定地址超出有效范围
*****************************************************************************************/
void  GUI_PutString(uint8 fontType, INT16U x, INT16U y, char *str, uint16 color, uint16 bkcolor)
{ 
    switch (fontType)
    {
        //case FONT_14:
        //        GUI_PutString14(fontType, x, y, str, color, bkcolor);
        //        break;

        //case FONT_16:
        //        GUI_PutString16(fontType, x, y, str, color, bkcolor);
        //        break;
        
        //case FONT_24:
        //        GUI_PutString24(fontType, x, y, str, color, bkcolor);
        //        break;
        
        //case FONT_35:
        //        GUI_PutString35(fontType, x, y, str, color, bkcolor);
        //        break;
        
        case FONT_64:
                GUI_PutString64(fontType, x, y, str, color, bkcolor);
                break;
        
        default:
                break;    
    };
}



#if 0

/*****************************************************************************************
函数名称:    void  GUI_PutRectangleString(uint8 fontType, INT16U x0, INT16U y0, INT16U x1, INT16U y1, char *str, INT08U algin)
版本号:        1.0.0
生成日期:    2008.08.31
作者:        影舞者
功能说明:    在指定的区域内居中显示
输入参数:    x1        指定显示位置，x坐标
            y1        指定显示位置，y坐标
            x2        指定显示位置，x坐标
            y2        指定显示位置，y坐标
            str        要显示的ASCII码字符串。
输出参数:    无
其它说明:    操作失败原因是指定地址超出有效范围(一行最多50个汉字,100个字符)
*****************************************************************************************/
void  GUI_PutRectangleString(uint8 fontType, INT16U x1, INT16U y1, INT16U x2, INT16U y2, char *str, INT08U algin, uint16 color, uint16 bkcolor)
{
    char    Temp[100];
    INT08U    line;
    INT16U    x, y, i, n, k, num;

    // 扫描有几行
    for (num = GUI_strlen(str), line = 0, i = 0; i < num; i++)
    {
        if (str[i] == '\n')
            line++;
    }

    // 注意,必须加一行,比如 "ddd \n tyui1234 \n 45"
    if (line > 0)
        line++;

    // 计算出Y轴起点
    if (line == 0)
    {
        if ((y2 - y1) > GUI_GetCharDistY())
            y = y1 + (((y2 - y1) - GUI_GetCharDistY()) / 2);
        else
            y = y1;
    }
    else
    {
        if ((y2 - y1) > (GUI_GetCharDistY() * line + GUI_GetCharSpaceY() * (line - 1)))
            y = y1 + (((y2 - y1) - (GUI_GetCharDistY() * line + GUI_GetCharSpaceY() * (line - 1))) / 2);
        else
            y = y1;
    }

    // 显示行数据
    if (line == 0)
    {
        // 对齐方式
        switch (algin)
        {
            case GUI_LeftAlgin:
                x = x1;
                break;

            case GUI_CenterAlgin:
                if ((x2 - x1) > (GUI_GetCharDistX() * GUI_strlen(str)))
                    x = x1 + (((x2 - x1) - (GUI_GetCharDistX() * GUI_strlen(str))) / 2);
                else
                    x = x1;
                break;

            case GUI_RightAlgin:
                if ((x2 - x1) > (GUI_GetCharDistX() * GUI_strlen(str)))
                    x = x1 + ((x2 - x1) - (GUI_GetCharDistX() * GUI_strlen(str)));
                else
                    x = x1;
                break;
        
        }

        // 显示
        GUI_PutString(fontType, x, y, str);
    }
    else
    {
        for (num = GUI_strlen(str), n = 0, i = 0; i < line; i++)
        {
            // 分离出每一行
            GUI_memset(Temp, '\0', GUI_sizeof(Temp));
            k = 0;

            for (; n < num; n++)
            {
                if (str[n] == '\0')
                {
                    break;
                }
                else if (str[n] == '\n')
                {
                    n++;
                    break;
                }
                else if (str[n] == '\r')
                {
                    ;    // 不处理
                }
                else
                {
                    Temp[k] = str[n];
                    k++;
                }
            }

            // 对齐方式
            switch (algin)
            {
                case GUI_LeftAlgin:
                    x = x1;
                    break;

                case GUI_CenterAlgin:
                    if ((x2 - x1) > (GUI_GetCharDistX() * GUI_strlen(Temp)))
                        x = x1 + (((x2 - x1) - (GUI_GetCharDistX() * GUI_strlen(Temp))) / 2);
                    else
                        x = x1;
                    break;

                case GUI_RightAlgin:
                    if ((x2 - x1) > (GUI_GetCharDistX() * GUI_strlen(Temp)))
                        x = x1 + ((x2 - x1) - (GUI_GetCharDistX() * GUI_strlen(Temp)));
                    else
                        x = x1;
                    break;
            
            }

            // 显示
            GUI_PutString(fontType, x, y, Temp);

            y = y + GUI_GetCharDistY() + GUI_GetCharSpaceY();
        }
    }
}

#endif



