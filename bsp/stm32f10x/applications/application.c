/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2013-07-12     aozima       update for auto initial.
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <board.h>
#include <rtthread.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#ifdef RT_USING_DFS
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif
#include <finsh.h>
#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#include <rtgui/calibration.h>
#endif

#include "led.h"
void uart2_tx(char *buf,int len);
void uart2_init();
rt_device_t uart2_dev = RT_NULL;
char g_rx_buf[256];
int g_rx_len=0;
#define BMP_DBUF_SIZE		2048
#define BI_RGB	 		0  //û��ѹ��.RGB 5,5,5.
#define BI_RLE8 		1  //ÿ������8���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����(�ظ����ؼ�������ɫ����)��
#define BI_RLE4 		2  //ÿ������4���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����
#define BI_BITFIELDS 	3  //ÿ�����صı�����ָ�������������  
//BMP��Ϣͷ
typedef  struct
{
    rt_uint32_t biSize ;		   	//˵��BITMAPINFOHEADER�ṹ����Ҫ��������
    long  biWidth ;		   	//˵��ͼ��Ŀ�ȣ�������Ϊ��λ 
    long  biHeight ;	   	//˵��ͼ��ĸ߶ȣ�������Ϊ��λ 
    rt_uint16_t  biPlanes ;	   		//ΪĿ���豸˵��λ��������ֵ�����Ǳ���Ϊ1 
    rt_uint16_t  biBitCount ;	   	//˵��������/���أ���ֵΪ1��4��8��16��24����32
    rt_uint32_t biCompression ;  	//˵��ͼ������ѹ�������͡���ֵ����������ֵ֮һ��
	//BI_RGB��û��ѹ����
	//BI_RLE8��ÿ������8���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����(�ظ����ؼ�������ɫ����)��  
    //BI_RLE4��ÿ������4���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����
  	//BI_BITFIELDS��ÿ�����صı�����ָ�������������
    rt_uint32_t biSizeImage ;		//˵��ͼ��Ĵ�С�����ֽ�Ϊ��λ������BI_RGB��ʽʱ��������Ϊ0  
    long  biXPelsPerMeter ;	//˵��ˮƽ�ֱ��ʣ�������/�ױ�ʾ
    long  biYPelsPerMeter ;	//˵����ֱ�ֱ��ʣ�������/�ױ�ʾ
    rt_uint32_t biClrUsed ;	  	 	//˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ������
    rt_uint32_t biClrImportant ; 	//˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ�� 
}__attribute__((packed)) BITMAPINFOHEADER;
//BMPͷ�ļ�
typedef struct
{
    rt_uint16_t  bfType ;     //�ļ���־.ֻ��'BM',����ʶ��BMPλͼ����
    rt_uint32_t  bfSize ;	  //�ļ���С,ռ�ĸ��ֽ�
    rt_uint16_t  bfReserved1 ;//����
    rt_uint16_t  bfReserved2 ;//����
    rt_uint32_t  bfOffBits ;  //���ļ���ʼ��λͼ����(bitmap data)��ʼ֮��ĵ�ƫ����
}__attribute__((packed)) BITMAPFILEHEADER ;
//��ɫ�� 
typedef struct 
{
    rt_uint8_t rgbBlue ;    //ָ����ɫǿ��
    rt_uint8_t rgbGreen ;	//ָ����ɫǿ�� 
    rt_uint8_t rgbRed ;	  	//ָ����ɫǿ�� 
    rt_uint8_t rgbReserved ;//����������Ϊ0 
}__attribute__((packed)) RGBQUAD ;
//λͼ��Ϣͷ
typedef struct
{ 
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bmiHeader;  
	rt_uint32_t RGB_MASK[3];			//��ɫ�����ڴ��RGB����.
	//RGBQUAD bmiColors[256];  
}__attribute__((packed)) BITMAPINFO; 
typedef RGBQUAD * LPRGBQUAD;//��ɫ��  
rt_uint8_t bmpreadbuf[BMP_DBUF_SIZE];
typedef struct
{		
	rt_uint16_t lcdwidth;	//LCD�Ŀ��
	rt_uint16_t lcdheight;	//LCD�ĸ߶�
	rt_uint32_t ImgWidth; 	//ͼ���ʵ�ʿ�Ⱥ͸߶�
	rt_uint32_t ImgHeight;

	rt_uint32_t Div_Fac;  	//����ϵ�� (������8192����)
	
	rt_uint32_t S_Height; 	//�趨�ĸ߶ȺͿ��
	rt_uint32_t S_Width;
	
	rt_uint32_t	S_XOFF;	  	//x���y���ƫ����
	rt_uint32_t S_YOFF;

	rt_uint32_t staticx; 	//��ǰ��ʾ���ģ�������
	rt_uint32_t staticy;																 	
}_pic_info;
_pic_info picinfo;
void ai_draw_init(long ImgWidth,long ImgHeight)
{
	float temp,temp1;	   
	temp=(float)800/ImgWidth;
	temp1=(float)480/ImgHeight;						 
	if(temp<temp1)temp1=temp;//ȡ��С���Ǹ�	 
	if(temp1>1)temp1=1;	  
	//ʹͼƬ��������������м�
	picinfo.S_XOFF+=(800-temp1*ImgWidth)/2;
	picinfo.S_YOFF+=(480-temp1*ImgHeight)/2;
	temp1*=8192;//����8192��	 
	picinfo.Div_Fac=temp1;
	picinfo.staticx=0xffff;
	picinfo.staticy=0xffff;//�ŵ�һ�������ܵ�ֵ����			 										    
}   
rt_uint8_t is_element_ok(rt_uint16_t x,rt_uint16_t y,rt_uint8_t chg)
{				  
	if(x!=picinfo.staticx||y!=picinfo.staticy)
	{
		if(chg==1)
		{
			picinfo.staticx=x;
			picinfo.staticy=y;
		} 
		return 1;
	}else return 0;
}
rt_uint8_t stdbmp_decode(const rt_uint8_t *filename) 
{
	int f_bmp;
    rt_uint16_t br;

    rt_uint16_t count;		    	   
	rt_uint8_t  rgb ,color_byte;
	rt_uint16_t x ,y,color;	  
	rt_uint16_t countpix=0;//��¼���� 	 

	//x,y��ʵ������	
	rt_uint16_t  realx=0;
	rt_uint16_t realy=0;
	rt_uint8_t  yok=1;  				   
	rt_uint8_t res;


	rt_uint8_t *databuf;    		//���ݶ�ȡ��ŵ�ַ
 	rt_uint16_t readlen=BMP_DBUF_SIZE;//һ�δ�SD����ȡ���ֽ�������

	rt_uint8_t *bmpbuf;			  	//���ݽ����ַ
	rt_uint8_t biCompression=0;		//��¼ѹ����ʽ
	
	rt_uint16_t rowlen;	  		 	//ˮƽ�����ֽ���  
	BITMAPINFO *pbmp;		//��ʱָ��
	
	databuf=bmpreadbuf;
f_bmp=open((const char*)filename, 0, 0);
if(f_bmp<0)
	rt_kprintf("read bmp failed\r\n");
//	res=f_open(f_bmp,(const TCHAR*)filename,FA_READ);//���ļ�	 						  
	if(f_bmp>=0)//�򿪳ɹ�.
	{ 
		//f_read(f_bmp,databuf,readlen,(UINT*)&br);	//����readlen���ֽ�  
		br=read(f_bmp, databuf, readlen*sizeof(char));
		pbmp=(BITMAPINFO*)databuf;					//�õ�BMP��ͷ����Ϣ   
		count=pbmp->bmfHeader.bfOffBits;        	//����ƫ��,�õ����ݶεĿ�ʼ��ַ
		color_byte=pbmp->bmiHeader.biBitCount/8;	//��ɫλ 16/24/32  
		biCompression=pbmp->bmiHeader.biCompression;//ѹ����ʽ
		rt_kprintf("count %d,color_byte %d,biCompression %d\r\n",count,color_byte,biCompression);
		//picinfo.ImgHeight=pbmp->bmiHeader.biHeight;	//�õ�ͼƬ�߶�
		//picinfo.ImgWidth=pbmp->bmiHeader.biWidth;  	//�õ�ͼƬ��� 
		rt_kprintf("width %d,height %d\r\n",pbmp->bmiHeader.biWidth,pbmp->bmiHeader.biHeight);
		ai_draw_init(pbmp->bmiHeader.biWidth,pbmp->bmiHeader.biHeight);//��ʼ�����ܻ�ͼ			
		//ˮƽ���ر�����4�ı���!!
		if((pbmp->bmiHeader.biWidth*color_byte)%4)rowlen=((pbmp->bmiHeader.biWidth*color_byte)/4+1)*4;
		else rowlen=pbmp->bmiHeader.biWidth*color_byte;
		//��ʼ����BMP   
		color=0;//��ɫ���	 													 
		x=0 ;
		y=pbmp->bmiHeader.biHeight;
		rgb=0;      
		//���ڳߴ�С�ڵ����趨�ߴ��ͼƬ,���п��ٽ���
		realy=(y*picinfo.Div_Fac)>>13;
		bmpbuf=databuf;
		while(1)
		{				 
			while(count<readlen)  //��ȡһ��1024���� (SectorsPerClust ÿ��������)
		    {
				if(color_byte==3)   //24λ��ɫͼ
				{
					switch (rgb) 
					{
						case 0:				  
							color=bmpbuf[count]>>3; //B
							break ;	   
						case 1: 	 
							color+=((rt_uint16_t)bmpbuf[count]<<3)&0X07E0;//G
							break;	  
						case 2 : 
							color+=((rt_uint16_t)bmpbuf[count]<<8)&0XF800;//R	  
							break ;			
					}   
				}else if(color_byte==2)  //16λ��ɫͼ
				{
					switch(rgb)
					{
						case 0 : 
							if(biCompression==BI_RGB)//RGB:5,5,5
							{
								color=((rt_uint16_t)bmpbuf[count]&0X1F);	 	//R
								color+=(((rt_uint16_t)bmpbuf[count])&0XE0)<<1; //G
							}else		//RGB:5,6,5
							{
								color=bmpbuf[count];  			//G,B
							}  
							break ;   
						case 1 : 			  			 
							if(biCompression==BI_RGB)//RGB:5,5,5
							{
								color+=(rt_uint16_t)bmpbuf[count]<<9;  //R,G
							}else  		//RGB:5,6,5
							{
								color+=(rt_uint16_t)bmpbuf[count]<<8;	//R,G
							}  									 
							break ;	 
					}		     
				}else if(color_byte==4)//32λ��ɫͼ
				{
					switch (rgb)
					{
						case 0:				  
							color=bmpbuf[count]>>3; //B
							break ;	   
						case 1: 	 
							color+=((rt_uint16_t)bmpbuf[count]<<3)&0X07E0;//G
							break;	  
						case 2 : 
							color+=((rt_uint16_t)bmpbuf[count]<<8)&0XF800;//R	  
							break ;			
						case 3 :
							//alphabend=bmpbuf[count];//����ȡ  ALPHAͨ��
							break ;  		  	 
					}	
				}else if(color_byte==1)//8λɫ,��ʱ��֧��,��Ҫ�õ���ɫ��.
				{
				} 
				rgb++;	  
				count++ ;		  
				if(rgb==color_byte) //ˮƽ�����ȡ��1���������ݺ���ʾ
				{	
					if(x<pbmp->bmiHeader.biWidth)	 					 			   
					{	
						realx=(x*picinfo.Div_Fac)>>13;//x��ʵ��ֵ
						if(is_element_ok(realx,realy,1)&&yok)//��������
						{						 				 	  	       
							//pic_phy.draw_point(realx+picinfo.S_XOFF,realy+picinfo.S_YOFF-1,color);//��ʾͼƬ	
LCD_Fast_DrawPoint(realx+picinfo.S_XOFF,realy+picinfo.S_YOFF-1,color);
							//POINT_COLOR=color;		 
							//LCD_DrawPoint(realx+picinfo.S_XOFF,realy+picinfo.S_YOFF); 
							//SRAMLCD.Draw_Point(realx+picinfo.S_XOFF,realy+picinfo.S_YOFF,color);
						}   									    
					}
					x++;//x������һ������ 
					color=0x00; 
					rgb=0;  		  
				}
				countpix++;//�����ۼ�
				if(countpix>=rowlen)//ˮƽ��������ֵ����.����
				{		 
					y--; 
					if(y==0)break;			 
					realy=(y*picinfo.Div_Fac)>>13;//ʵ��yֵ�ı�	 
					if(is_element_ok(realx,realy,0))yok=1;//�˴����ı�picinfo.staticx,y��ֵ	 
					else yok=0; 
					x=0; 
					countpix=0;
					color=0x00;
					rgb=0;
				}	 
			} 		
			//res=f_read(f_bmp,databuf,readlen,(UINT *)&br);//����readlen���ֽ�
			br=read(f_bmp, databuf, readlen*sizeof(char));
			if(br!=readlen)readlen=br;	//���һ������		  
			if(res||br==0)break;		//��ȡ����
			bmpbuf=databuf;
	 	 	count=0;
		}  
		close(f_bmp);//�ر��ļ�
	}  	
	return res;		//BMP��ʾ����.    					   
}		 
ALIGN(RT_ALIGN_SIZE)
  static rt_uint8_t led_stack[ 2048 ];
  static struct rt_thread led_thread;
static void led_thread_entry(void* parameter)
{
  unsigned int count=0;
  int i,j;
  int fd;
  int index, length;
  char ch;

  rt_hw_led_init();
  LCD_Init();
  ls("/");	
  /*fd = open("/FIGURE1.BIN", 0, 0);
  if (fd < 0)
  {
	rt_kprintf("check: open file for read failed\n");
	return;
  }


  for(i=0;i<800;i++)
  {
	for(j=0;j<240;j++)
	{
	  read(fd, &ch, sizeof(char));
	//  rt_kprintf("%x ",ch);
	  LCD_Fast_DrawPoint(i,j,ch);
	}
	//rt_kprintf("\r\n");
  }
  close(fd);*/
  stdbmp_decode("/FIGURE1.BMP");
  rt_kprintf("\r\nshow over\r\n");
  //LCD_ShowString(60,50,200,16,16,"Mini STM32");
  while (1)
  {
	/* led1 on */
#ifndef RT_USING_FINSH
	rt_kprintf("led on, count : %d\r\n",count);
#endif
	//LCD_Clear(0XF81F);
	//LCD_ShowString(120,240,24,24,24,"Led onn");
	//uart2_tx("123456\r\n",rt_strlen("123456\r\n"));
	count++;
	rt_hw_led_on(0);
	rt_hw_led_on(1);
	rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */

	/* led1 off */
#ifndef RT_USING_FINSH
	rt_kprintf("led off\r\n");
#endif
	//LCD_Clear(0xF800);
	//LCD_ShowString(120,240,24,24,24,"Led off");
	//uart2_tx("654321\r\n",rt_strlen("654321\r\n"));
	rt_hw_led_off(0);
	rt_hw_led_off(1);
	rt_thread_delay( RT_TICK_PER_SECOND/2 );
  }
}
static rt_err_t uart2_rx_ind(rt_device_t dev, rt_size_t size)
{
  char ch;
  int i=0;	
  while (rt_device_read(dev, 0, &ch, 1) == 1)
  {
	g_rx_buf[i]=ch;
	i++;
  }
  g_rx_len=i;
  uart2_tx(g_rx_buf,g_rx_len);
}
void uart2_tx(char *buf,int len)
{
  rt_uint16_t old_flag = uart2_dev->flag;

  uart2_dev->flag |= RT_DEVICE_FLAG_STREAM;
  rt_device_write(uart2_dev, 0, buf, len);
  uart2_dev->flag = old_flag;	  
}
void uart2_init()
{


  uart2_dev = rt_device_find("uart1");
  if (uart2_dev == RT_NULL)
  {
	rt_kprintf("finsh: can not find device: uart2\n");
	return;
  }

  /* open this device and set the new device in finsh shell */
  if (rt_device_open(uart2_dev, RT_DEVICE_OFLAG_RDWR) == RT_EOK)
  {
	rt_device_set_rx_indicate(uart2_dev, uart2_rx_ind);
  }
}
#ifdef RT_USING_RTGUI
rt_bool_t cali_setup(void)
{
  rt_kprintf("cali setup entered\n");
  return RT_FALSE;
}

void cali_store(struct calibration_data *data)
{
  rt_kprintf("cali finished (%d, %d), (%d, %d)\n",
	  data->min_x,
	  data->max_x,
	  data->min_y,
	  data->max_y);
}
#endif /* RT_USING_RTGUI */

void rt_init_thread_entry(void* parameter)
{

#ifdef RT_USING_COMPONENTS_INIT
  /* initialization RT-Thread Components */
  rt_components_init();
#endif

#ifdef  RT_USING_FINSH
  finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif  /* RT_USING_FINSH */

  /* Filesystem Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
  /* mount sd card fat partition 1 as root directory */
  if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
  {
	rt_kprintf("File System initialized!\n");
  }
  else
	rt_kprintf("File System initialzation failed!\n");
#endif  /* RT_USING_DFS */
#ifdef RT_USING_RTGUI
  {
	extern void rt_hw_lcd_init();
	extern void rtgui_touch_hw_init(void);

	rt_device_t lcd;

	/* init lcd */
	rt_hw_lcd_init();

	/* init touch panel */
	rtgui_touch_hw_init();

	/* re-init device driver */
	rt_device_init_all();

	/* find lcd device */
	lcd = rt_device_find("lcd");

	/* set lcd device as rtgui graphic driver */
	rtgui_graphic_set_device(lcd);

#ifndef RT_USING_COMPONENTS_INIT
	/* init rtgui system server */
	rtgui_system_server_init();
#endif

	calibration_set_restore(cali_setup);
	calibration_set_after(cali_store);
	calibration_init();
  }
#endif /* #ifdef RT_USING_RTGUI */
}

int rt_application_init(void)
{
  rt_thread_t init_thread;

  rt_err_t result;
  //uart2_init();

  /* init led thread */
  result = rt_thread_init(&led_thread,
	  "led",
	  led_thread_entry,
	  RT_NULL,
	  (rt_uint8_t*)&led_stack[0],
	  sizeof(led_stack),
	  20,
	  5);
  if (result == RT_EOK)
  {
	rt_thread_startup(&led_thread);
  }

#if (RT_THREAD_PRIORITY_MAX == 32)
  init_thread = rt_thread_create("init",
	  rt_init_thread_entry, RT_NULL,
	  2048, 8, 20);
#else
  init_thread = rt_thread_create("init",
	  rt_init_thread_entry, RT_NULL,
	  2048, 80, 20);
#endif

  if (init_thread != RT_NULL)
	rt_thread_startup(init_thread);

  return 0;
}

/*@}*/
