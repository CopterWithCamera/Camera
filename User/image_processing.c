#include "image_processing.h"
#include "./usart/bsp_debug_usart.h"
#include "math.h"
#include "rgbTObmp.h"
#include "bsp_spi_nrf.h"
#include "image_fix.h"

/*
 * ****** 能够使用的资源 *******
 * 
 * 延时 #define Delay(ms)  Delay_ms(ms) 
 *
 * 摄像头采集的图像大小（可以用这个算数组长度） extern uint16_t img_width, img_height;
 * 
 */
 
//**************************************************************
	
//图像缓存数组,大小：宽度*长度*2字节
uint8_t CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//长度*宽度*2个字节

//灰度图像存储空间
uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//长度*宽度*1字节

//运算结果存储空间
uint8_t result_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//长度*宽度*1字节

//输出参数
float length;	//偏差
float speed;

//生成灰度矩阵
void Creat_Gray(void)
{
	uint32_t r,g,b;
	
	uint16_t i;
	
	for(i=0;i<IMG_WIDTH*IMG_HEIGHT*2;i=i+2)
	{
		r = (CAMERA_BUFFER_ARRAY[i+1] >> 3) * 8;
		g = (((CAMERA_BUFFER_ARRAY[i+1] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[i] >> 5)) * 4;
		b = (CAMERA_BUFFER_ARRAY[i] & 0x1F) * 8;
		
		gray_array[i/2] = (r * 299 + g * 587 + b * 114 + 500) / 1000;
	}
}

//************************ 为算法提供数据源 ************************************

//获取单点Gray区数值
uint8_t Get_Gray(uint16_t row,uint16_t column)	//第row行，第column个
{
	//计算点的方式是先确定第row行，再确定在本行中的第column个数值。
	//行：row      范围：1 -- IMG_HEIGHT
	//列：column   范围：1 -- IMG_WIDTH
	
	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	return gray_array[num/2];
}

//存储单点数据到result区
void To_Result(uint16_t row,uint16_t column,uint8_t gray)
{
	//计算点的方式是先确定第row行，再确定在本行中的第column个数值。
	//行：row      范围：1 -- IMG_HEIGHT
	//列：column   范围：1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	result_array[num/2] = gray;
}

//************** 输出信息 ************************************************

void Data_Output_Ctrl(u8 num,u8 cmd)	//num：指令号		cmd：指令内容
{
	switch(num)
	{
		case 1:			//图像传输控制
			if(cmd)
			{
				
			}
			else
			{
				
			}
			break;
		case 2:			//波形传输控制
			if(cmd)
			{
				
			}
			else
			{
				
			}			
			break;
		case 3:			//显示fps
			if(cmd)
			{
				
			}
			else
			{
				
			}
			break;
		default:
			break;
	}
}

//将数据传送到对外端口
void Data_Output(u8 ch)
{
	#ifdef __USART_DISPLAY

		USART2_Send(ch);
	
	#endif
	
	#ifdef __NRF_DISPLAY
		if(NRF24L01_State)
		{
			NRF_Send(ch);	//NRF发送
		}
	#endif
}

//显示图像，配合山外多功能调试助手
void Display_Image(void)
{
	uint32_t i;
	uint8_t ch;
	
	//发送包头
	ch = 0x01;
	Data_Output(ch);

	ch = 0xFE;
	Data_Output(ch);

	
	//发送图像
	for(i = 0 ; i<IMG_HEIGHT*IMG_WIDTH; i++ )
	{		
		ch = gray_array[i];
		Data_Output(ch);
	}
	
	//发送包尾
	ch = 0xFE;
	Data_Output(ch);
	
	ch = 0x01;
	Data_Output(ch);
	
}

//float转4个unsigned char
void float_char(float f,unsigned char *s)
{
	unsigned char *p;
 
	p = (unsigned char *)&f;
    *s = *p;
    *(s+1) = *(p+1);
    *(s+2) = *(p+2);
    *(s+3) = *(p+3);
}

//输出波形（length和speed），用山外多功能调试助手查看
void Display_Wave(void)
{
	uint8_t ch;
	unsigned char a[4];
	
	//发送包头
	ch = 0x03;
	Data_Output(ch);
	ch = 0xFC;
	Data_Output(ch);
	
	//发送内容
	
	//发送通道一
	float_char(length,a);
	ch = a[0];
	Data_Output(ch);
	ch = a[1];
	Data_Output(ch);
	ch = a[2];
	Data_Output(ch);	
	ch = a[3];
	Data_Output(ch);
	
	//发送通道二
	float_char(speed,a);
	ch = a[0];
	Data_Output(ch);
	ch = a[1];
	Data_Output(ch);
	ch = a[2];
	Data_Output(ch);
	ch = a[3];
	Data_Output(ch);		
	
	//发送包尾
	ch = 0xFC;
	Data_Output(ch);
	ch = 0x03;
	Data_Output(ch);
	
}

void Send_Parameter_Fps(void)
{
	uint8_t ch;
	unsigned char a[4];
	
	ch = 0x04;
	Data_Output(ch);
	ch = 0xFB;
	Data_Output(ch);
	
	//发送fps
	float_char(fps,a);
	ch = a[0];
	Data_Output(ch);
	ch = a[1];
	Data_Output(ch);
	ch = a[2];
	Data_Output(ch);	
	ch = a[3];
	Data_Output(ch);	
	
	//发送包尾
	ch = 0xFB;
	Data_Output(ch);
	ch = 0x04;
	Data_Output(ch);
}

void Image_Output(void)
{
	//*******************************************************************
	//输出信息
	
	if(!full_flag)
	{
		#if defined(__DISPLAY_IMAGE)
		
			Display_Image();	//从串口输出图像，配合山外多功能调试助手显示
		
		#endif
		
			
		#if defined(__DISPALY_WAVE)
		
			Display_Wave();	//串口输出波形
		
		#endif
		
		
		#if defined(__PARAMETER_FPS)
		
			Send_Parameter_Fps();
		
		#endif
	}
	
	//*******************************************************************
	//SD存图
	
	#ifdef __SD_SAVE
	
		if(SD_State)	//如果SD卡挂载成功
		{
			TO_SDcard();    //SD卡
		}
		
	#endif
}

//*********************** 总执行函数 **********************************************

uint8_t image_updata_flag = 0;		//新图像采集完成标志  0：新图没有采集完成    1：新图采集完成
void Image_Process(void)
{
	DCMI_CaptureCmd(ENABLE);			//读取一帧图像到缓存

	//新写法
	//延时1s 或 图像采集完成中断置位
	image_updata_flag = 0;
	Task_Delay[9] = 1000;
	while(Task_Delay[9]!=0 && image_updata_flag == 0){}
	
	Creat_Gray();	//生成灰度矩阵，数据来自显示缓冲
	Image_Fix();	//图像处理函数
	Image_Output();	//数据输出

}

