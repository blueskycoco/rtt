/*****************************************************************************************
��Ȩ����:        ----------------  
�汾��:            1.0.0
��������:        2008.08.31
�ļ���:            gui_font.c
����:            Ӱ����
����˵��:        ASCII�����弰��ʾ������
����˵��:        ��
�����ļ���ϵ:    ���ļ�Ϊ���̹�Լ�����ļ�

�޸ļ�¼:
��¼1:
�޸���:
�޸�����:
�޸�����:
�޸�ԭ��:
*****************************************************************************************/

#define  GUI_FONT_GLOBALS                                        
#include "app_types.h"




/*****************************************************************************************
��Ȩ����:   Ӱ����
�汾��:     1.0.0
��������:   2008.08.31
����˵��:   ����ʮ����(0-7)==>ʮ������λת����������ʾ�������������ң�����ʮ������λ˳���ǵ���
����˵��:	��
*****************************************************************************************/
const  INT08U  DCB2HEX_TAB[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};




/*****************************************************************************************
��������:    void  GUI_PutString(uint8 fontType, INT16U x, INT16U y, char *str)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    �����ʾ�ַ���(û���Զ����й���)
�������:    x        ָ����ʾλ�ã�x����
            y        ָ����ʾλ�ã�y����
            str        Ҫ��ʾ��ASCII���ַ���
�������:    ��
����˵��:    ����ʧ��ԭ����ָ����ַ������Ч��Χ
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
��������:    void  GUI_PutRectangleString(uint8 fontType, INT16U x0, INT16U y0, INT16U x1, INT16U y1, char *str, INT08U algin)
�汾��:        1.0.0
��������:    2008.08.31
����:        Ӱ����
����˵��:    ��ָ���������ھ�����ʾ
�������:    x1        ָ����ʾλ�ã�x����
            y1        ָ����ʾλ�ã�y����
            x2        ָ����ʾλ�ã�x����
            y2        ָ����ʾλ�ã�y����
            str        Ҫ��ʾ��ASCII���ַ�����
�������:    ��
����˵��:    ����ʧ��ԭ����ָ����ַ������Ч��Χ(һ�����50������,100���ַ�)
*****************************************************************************************/
void  GUI_PutRectangleString(uint8 fontType, INT16U x1, INT16U y1, INT16U x2, INT16U y2, char *str, INT08U algin, uint16 color, uint16 bkcolor)
{
    char    Temp[100];
    INT08U    line;
    INT16U    x, y, i, n, k, num;

    // ɨ���м���
    for (num = GUI_strlen(str), line = 0, i = 0; i < num; i++)
    {
        if (str[i] == '\n')
            line++;
    }

    // ע��,�����һ��,���� "ddd \n tyui1234 \n 45"
    if (line > 0)
        line++;

    // �����Y�����
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

    // ��ʾ������
    if (line == 0)
    {
        // ���뷽ʽ
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

        // ��ʾ
        GUI_PutString(fontType, x, y, str);
    }
    else
    {
        for (num = GUI_strlen(str), n = 0, i = 0; i < line; i++)
        {
            // �����ÿһ��
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
                    ;    // ������
                }
                else
                {
                    Temp[k] = str[n];
                    k++;
                }
            }

            // ���뷽ʽ
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

            // ��ʾ
            GUI_PutString(fontType, x, y, Temp);

            y = y + GUI_GetCharDistY() + GUI_GetCharSpaceY();
        }
    }
}

#endif



