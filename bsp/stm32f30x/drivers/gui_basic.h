/*****************************************************************************************
版权所有:        --------------  
版本号:            1.0.0
生成日期:        2008.08.31
文件名:            gui_basic.h
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

#ifndef GUI_BASIC_PRESENT
#define GUI_BASIC_PRESENT

#ifdef  GUI_BASIC_GLOBALS                                                
    #define GUI_BASIC_EXT                                                    
#else
    #define GUI_BASIC_EXT    extern                    
#endif 




/*****************************************************************************************
版权所有:   影舞者
版本号:     1.0.0
生成日期:   2008.08.31
功能说明:   函数接口
其它说明:    无
*****************************************************************************************/
GUI_BASIC_EXT  void  GUI_HLine          (INT16U x0, INT16U y0, INT16U x1, uint16 color);
GUI_BASIC_EXT  void  GUI_VLine          (INT16U x0, INT16U y0, INT16U y1, uint16 color);
GUI_BASIC_EXT  void  GUI_Rectangle      (INT16U x0, INT16U y0, INT16U x1, INT16U y1, uint16 color);
GUI_BASIC_EXT  void  GUI_RectangleFill  (INT16U x0, INT16U y0, INT16U x1, INT16U y1, uint16 color);
GUI_BASIC_EXT  void  GUI_Square         (INT16U x0, INT16U y0, INT16U  with, uint16 color);
GUI_BASIC_EXT  void  GUI_DrawPoint      (INT16U x, INT16U y, uint16 color);
GUI_BASIC_EXT  void  GUI_Line           (INT16U x0, INT16U y0, INT16U x1, INT16U y1, uint16 color);
GUI_BASIC_EXT  void  GUI_LineWith       (INT16U x0, INT16U y0, INT16U x1, INT16U y1, INT16U with, uint16 color);
GUI_BASIC_EXT  void  GUI_LineDashed     (INT16U x0, INT16U y0, INT16U x1, INT16U y1, INT16U dashed1, INT16U dashed2, uint16 color);
GUI_BASIC_EXT  void  GUI_Circle         (INT16U x0, INT16U y0, INT16U r, uint16 color);
GUI_BASIC_EXT  void  GUI_CircleFill     (INT16U x0, INT16U y0, INT16U r, uint16 color);
GUI_BASIC_EXT  void  GUI_Ellipse        (INT16U x0, INT16U x1, INT16U y0, INT16U y1, uint16 color);
GUI_BASIC_EXT  void  GUI_EllipseFill    (INT16U x0, INT16U x1, INT16U y0, INT16U y1, uint16 color);
GUI_BASIC_EXT  void  GUI_Arc4           (INT16U x, INT16U y, INT16U r, INT16U angle, uint16 color);
GUI_BASIC_EXT  void  GUI_Arc            (INT16U x, INT16U y, INT16U r, INT16U stangle, INT16U endangle, uint16 color);
GUI_BASIC_EXT  void  GUI_Pieslice       (INT16U x, INT16U y, INT16U r, INT16U stangle, INT16U endangle, uint16 color);




#endif





