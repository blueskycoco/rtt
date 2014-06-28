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
#define BI_RGB	 		0  //没有压缩.RGB 5,5,5.
#define BI_RLE8 		1  //每个象素8比特的RLE压缩编码，压缩格式由2字节组成(重复象素计数和颜色索引)；
#define BI_RLE4 		2  //每个象素4比特的RLE压缩编码，压缩格式由2字节组成
#define BI_BITFIELDS 	3  //每个象素的比特由指定的掩码决定。  
//BMP信息头
typedef  struct
{
    rt_uint32_t biSize ;		   	//说明BITMAPINFOHEADER结构所需要的字数。
    long  biWidth ;		   	//说明图象的宽度，以象素为单位 
    long  biHeight ;	   	//说明图象的高度，以象素为单位 
    rt_uint16_t  biPlanes ;	   		//为目标设备说明位面数，其值将总是被设为1 
    rt_uint16_t  biBitCount ;	   	//说明比特数/象素，其值为1、4、8、16、24、或32
    rt_uint32_t biCompression ;  	//说明图象数据压缩的类型。其值可以是下述值之一：
	//BI_RGB：没有压缩；
	//BI_RLE8：每个象素8比特的RLE压缩编码，压缩格式由2字节组成(重复象素计数和颜色索引)；  
    //BI_RLE4：每个象素4比特的RLE压缩编码，压缩格式由2字节组成
  	//BI_BITFIELDS：每个象素的比特由指定的掩码决定。
    rt_uint32_t biSizeImage ;		//说明图象的大小，以字节为单位。当用BI_RGB格式时，可设置为0  
    long  biXPelsPerMeter ;	//说明水平分辨率，用象素/米表示
    long  biYPelsPerMeter ;	//说明垂直分辨率，用象素/米表示
    rt_uint32_t biClrUsed ;	  	 	//说明位图实际使用的彩色表中的颜色索引数
    rt_uint32_t biClrImportant ; 	//说明对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要。 
}__attribute__((packed)) BITMAPINFOHEADER;
//BMP头文件
typedef struct
{
    rt_uint16_t  bfType ;     //文件标志.只对'BM',用来识别BMP位图类型
    rt_uint32_t  bfSize ;	  //文件大小,占四个字节
    rt_uint16_t  bfReserved1 ;//保留
    rt_uint16_t  bfReserved2 ;//保留
    rt_uint32_t  bfOffBits ;  //从文件开始到位图数据(bitmap data)开始之间的的偏移量
}__attribute__((packed)) BITMAPFILEHEADER ;
//彩色表 
typedef struct 
{
    rt_uint8_t rgbBlue ;    //指定蓝色强度
    rt_uint8_t rgbGreen ;	//指定绿色强度 
    rt_uint8_t rgbRed ;	  	//指定红色强度 
    rt_uint8_t rgbReserved ;//保留，设置为0 
}__attribute__((packed)) RGBQUAD ;
//位图信息头
typedef struct
{ 
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bmiHeader;  
	rt_uint32_t RGB_MASK[3];			//调色板用于存放RGB掩码.
	//RGBQUAD bmiColors[256];  
}__attribute__((packed)) BITMAPINFO; 
typedef RGBQUAD * LPRGBQUAD;//彩色表  
rt_uint8_t bmpreadbuf[BMP_DBUF_SIZE];
typedef struct
{		
	rt_uint16_t lcdwidth;	//LCD的宽度
	rt_uint16_t lcdheight;	//LCD的高度
	rt_uint32_t ImgWidth; 	//图像的实际宽度和高度
	rt_uint32_t ImgHeight;

	rt_uint32_t Div_Fac;  	//缩放系数 (扩大了8192倍的)
	
	rt_uint32_t S_Height; 	//设定的高度和宽度
	rt_uint32_t S_Width;
	
	rt_uint32_t	S_XOFF;	  	//x轴和y轴的偏移量
	rt_uint32_t S_YOFF;

	rt_uint32_t staticx; 	//当前显示到的ｘｙ坐标
	rt_uint32_t staticy;																 	
}_pic_info;
_pic_info picinfo;
void ai_draw_init(long ImgWidth,long ImgHeight)
{
	float temp,temp1;	   
	temp=(float)800/ImgWidth;
	temp1=(float)480/ImgHeight;						 
	if(temp<temp1)temp1=temp;//取较小的那个	 
	if(temp1>1)temp1=1;	  
	//使图片处于所给区域的中间
	picinfo.S_XOFF+=(800-temp1*ImgWidth)/2;
	picinfo.S_YOFF+=(480-temp1*ImgHeight)/2;
	temp1*=8192;//扩大8192倍	 
	picinfo.Div_Fac=temp1;
	picinfo.staticx=0xffff;
	picinfo.staticy=0xffff;//放到一个不可能的值上面			 										    
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
	rt_uint16_t countpix=0;//记录像素 	 

	//x,y的实际坐标	
	rt_uint16_t  realx=0;
	rt_uint16_t realy=0;
	rt_uint8_t  yok=1;  				   
	rt_uint8_t res;


	rt_uint8_t *databuf;    		//数据读取存放地址
 	rt_uint16_t readlen=BMP_DBUF_SIZE;//一次从SD卡读取的字节数长度

	rt_uint8_t *bmpbuf;			  	//数据解码地址
	rt_uint8_t biCompression=0;		//记录压缩方式
	
	rt_uint16_t rowlen;	  		 	//水平方向字节数  
	BITMAPINFO *pbmp;		//临时指针
	
	databuf=bmpreadbuf;
f_bmp=open((const char*)filename, 0, 0);
if(f_bmp<0)
	rt_kprintf("read bmp failed\r\n");
//	res=f_open(f_bmp,(const TCHAR*)filename,FA_READ);//打开文件	 						  
	if(f_bmp>=0)//打开成功.
	{ 
		//f_read(f_bmp,databuf,readlen,(UINT*)&br);	//读出readlen个字节  
		br=read(f_bmp, databuf, readlen*sizeof(char));
		pbmp=(BITMAPINFO*)databuf;					//得到BMP的头部信息   
		count=pbmp->bmfHeader.bfOffBits;        	//数据偏移,得到数据段的开始地址
		color_byte=pbmp->bmiHeader.biBitCount/8;	//彩色位 16/24/32  
		biCompression=pbmp->bmiHeader.biCompression;//压缩方式
		rt_kprintf("count %d,color_byte %d,biCompression %d\r\n",count,color_byte,biCompression);
		//picinfo.ImgHeight=pbmp->bmiHeader.biHeight;	//得到图片高度
		//picinfo.ImgWidth=pbmp->bmiHeader.biWidth;  	//得到图片宽度 
		rt_kprintf("width %d,height %d\r\n",pbmp->bmiHeader.biWidth,pbmp->bmiHeader.biHeight);
		ai_draw_init(pbmp->bmiHeader.biWidth,pbmp->bmiHeader.biHeight);//初始化智能画图			
		//水平像素必须是4的倍数!!
		if((pbmp->bmiHeader.biWidth*color_byte)%4)rowlen=((pbmp->bmiHeader.biWidth*color_byte)/4+1)*4;
		else rowlen=pbmp->bmiHeader.biWidth*color_byte;
		//开始解码BMP   
		color=0;//颜色清空	 													 
		x=0 ;
		y=pbmp->bmiHeader.biHeight;
		rgb=0;      
		//对于尺寸小于等于设定尺寸的图片,进行快速解码
		realy=(y*picinfo.Div_Fac)>>13;
		bmpbuf=databuf;
		while(1)
		{				 
			while(count<readlen)  //读取一簇1024扇区 (SectorsPerClust 每簇扇区数)
		    {
				if(color_byte==3)   //24位颜色图
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
				}else if(color_byte==2)  //16位颜色图
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
				}else if(color_byte==4)//32位颜色图
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
							//alphabend=bmpbuf[count];//不读取  ALPHA通道
							break ;  		  	 
					}	
				}else if(color_byte==1)//8位色,暂时不支持,需要用到颜色表.
				{
				} 
				rgb++;	  
				count++ ;		  
				if(rgb==color_byte) //水平方向读取到1像素数数据后显示
				{	
					if(x<pbmp->bmiHeader.biWidth)	 					 			   
					{	
						realx=(x*picinfo.Div_Fac)>>13;//x轴实际值
						if(is_element_ok(realx,realy,1)&&yok)//符合条件
						{						 				 	  	       
							//pic_phy.draw_point(realx+picinfo.S_XOFF,realy+picinfo.S_YOFF-1,color);//显示图片	
LCD_Fast_DrawPoint(realx+picinfo.S_XOFF,realy+picinfo.S_YOFF-1,color);
							//POINT_COLOR=color;		 
							//LCD_DrawPoint(realx+picinfo.S_XOFF,realy+picinfo.S_YOFF); 
							//SRAMLCD.Draw_Point(realx+picinfo.S_XOFF,realy+picinfo.S_YOFF,color);
						}   									    
					}
					x++;//x轴增加一个像素 
					color=0x00; 
					rgb=0;  		  
				}
				countpix++;//像素累加
				if(countpix>=rowlen)//水平方向像素值到了.换行
				{		 
					y--; 
					if(y==0)break;			 
					realy=(y*picinfo.Div_Fac)>>13;//实际y值改变	 
					if(is_element_ok(realx,realy,0))yok=1;//此处不改变picinfo.staticx,y的值	 
					else yok=0; 
					x=0; 
					countpix=0;
					color=0x00;
					rgb=0;
				}	 
			} 		
			//res=f_read(f_bmp,databuf,readlen,(UINT *)&br);//读出readlen个字节
			br=read(f_bmp, databuf, readlen*sizeof(char));
			if(br!=readlen)readlen=br;	//最后一批数据		  
			if(res||br==0)break;		//读取出错
			bmpbuf=databuf;
	 	 	count=0;
		}  
		close(f_bmp);//关闭文件
	}  	
	return res;		//BMP显示结束.    					   
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
