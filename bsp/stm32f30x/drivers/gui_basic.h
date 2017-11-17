/*****************************************************************************************
��Ȩ����:        --------------  
�汾��:            1.0.0
��������:        2008.08.31
�ļ���:            gui_basic.h
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

#ifndef GUI_BASIC_PRESENT
#define GUI_BASIC_PRESENT

#ifdef  GUI_BASIC_GLOBALS                                                
    #define GUI_BASIC_EXT                                                    
#else
    #define GUI_BASIC_EXT    extern                    
#endif 




/*****************************************************************************************
��Ȩ����:   Ӱ����
�汾��:     1.0.0
��������:   2008.08.31
����˵��:   �����ӿ�
����˵��:    ��
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





