#include "image_processing.h"
#include "./usart/bsp_debug_usart.h"
#include "math.h"
#include "rgbTObmp.h"
#include "bsp_spi_nrf.h"
#include "image_fix.h"
#include "copter_datatrans.h"

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

uint8_t CAMERA_BUFFER_ARRAY1[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//长度*宽度*2个字节
uint8_t CAMERA_BUFFER_ARRAY2[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//长度*宽度*2个字节

uint8_t * CAMERA_BUFFER_ARRAY = CAMERA_BUFFER_ARRAY1;	//当前数据指针
uint8_t * DCMI_IN_BUFFER_ARRAY = CAMERA_BUFFER_ARRAY2;	//当前输入缓存指针

//灰度图像存储空间
uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//长度*宽度*1字节
uint8_t gray_column_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//列向量矩阵

//运算结果存储空间
uint8_t result_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//长度*宽度*1字节
uint8_t result_column_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//列向量矩阵

//输出参数
float length;	//偏差
float angle;
float speed;

//传输数据的模式
unsigned char mode = 0;

//控制传输的flag
u8 flag_Image = 0;
u8 flag_Result = 0;
u8 flag_Wave = 0;
u8 flag_Sd_gray = 0;
u8 flag_Sd_result = 0;

//参数传输flag（一般要开启）
u8 flag_Fps = 1;
u8 flag_Mode = 1;

//生成灰度矩阵
void Creat_Gray(void)
{
	uint32_t r,g,b;
	
	uint16_t i;
	
	//转灰度
	for(i=0;i<IMG_WIDTH*IMG_HEIGHT*2;i=i+2)
	{
		r = (CAMERA_BUFFER_ARRAY[i+1] >> 3) * 8;
		g = (((CAMERA_BUFFER_ARRAY[i+1] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[i] >> 5)) * 4;
		b = (CAMERA_BUFFER_ARRAY[i] & 0x1F) * 8;
		
		//gray_array[i/2] = (r * 299 + g * 587 + b * 114 + 500) / 1000;
		gray_array[IMG_WIDTH*IMG_HEIGHT - i/2 - 1] = (r * 299 + g * 587 + b * 114 + 500) / 1000;
	}
	
}

//转换为列向量矩阵
void Creat_Column(void)	//把图翻到对角的位置，生成gray_across_array
{
	uint16_t i,j;
	
	for (i = 0; i < 48; i++)
	{
		for (j = 0; j < 80; j++)
		{
			gray_column_array[j * 48 + i] = gray_array[i * 80 + j];
		}
	}
}

//列向量矩阵恢复为行矩阵
void Column_To_Line(void)	//把图翻到对角的位置，生成gray_across_array
{
	uint16_t i,j;
	
	for (i = 0; i < 80; i++)
	{
		for (j = 0; j < 48; j++)
		{
			result_array[j * 80 + i] = gray_column_array[i * 48 + j];
		}
	}
}

//************** 输出信息 ************************************************

//将数据传送到对外端口
void Data_Output(u8 ch)
{	
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

//显示图像，配合山外多功能调试助手
void Display_Result(void)
{
	uint32_t i;
	uint8_t ch;
	
	//发送包头
	ch = 0x02;
	Data_Output(ch);

	ch = 0xFD;
	Data_Output(ch);

	
	//发送图像
	for(i = 0 ; i<IMG_HEIGHT*IMG_WIDTH; i++ )
	{		
		ch = result_array[i];
		Data_Output(ch);
	}
	
	//发送包尾
	ch = 0xFD;
	Data_Output(ch);
	
	ch = 0x02;
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

//输出波形（length、angle、speed），用山外多功能调试助手查看
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
	float_char(angle,a);
	ch = a[0];
	Data_Output(ch);
	ch = a[1];
	Data_Output(ch);
	ch = a[2];
	Data_Output(ch);
	ch = a[3];
	Data_Output(ch);
	
	//发送通道三
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
	
	//发送运算fps
	float_char(processing_fps,a);
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

void Send_Parameter_Mode(void)
{
	uint8_t ch;
	
	ch = 0x05;
	Data_Output(ch);
	ch = 0xFA;
	Data_Output(ch);
	
	//发送mode
	ch = mode;
	Data_Output(ch);
	
	//发送包尾
	ch = 0xFA;
	Data_Output(ch);
	ch = 0x05;
	Data_Output(ch);
}

void Mode_Set(void)	//模式设置函数，在初始化过程中和切换模式时均有调用
{
	switch(mode)
	{
		case 0:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_gray = 1;
			flag_Sd_result = 0;
		break;
		
		case 1:
			flag_Image = 1;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
		break;
		
		case 2:
			flag_Image = 0;
			flag_Result = 1;
			flag_Wave = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
		break;
		
		case 3:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 1;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
		break;
		
		case 4:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
		break;
		
		case 5:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_gray = 1;
			flag_Sd_result = 0;
		break;
		
		case 6:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 1;
		break;
		
		default:
			break;
	}
}

void Mode_Change(void)	//在按键中断中调用
{
	mode++;
	if(mode>6)
	{
		mode = 0;
	}
	
	Mode_Set();

}

void Image_Output(u8 mode)	//mode 0--运算之前调用；1--运算之后调用（原图可以在运算的同时传输，运算后图只能在运算结束后传输）
{
	//*******************************************************************
	//输出信息
	
	if(!mode)	
	{
		//运算开始前发送内容（不需要运算的内容）
		
		//向飞控发送内容
		
		
		//向上位机发送
		#if defined(__DISPLAY_IMAGE)
			if(flag_Image)
				Display_Image();	//从串口输出图像，配合山外多功能调试助手显示
		
		#endif
			
		#if defined(__PARAMETER_FPS)
		
			if(flag_Fps)
				Send_Parameter_Fps();
		
		#endif
			
		#if defined(__PARAMETER_MODE)
			
			if(flag_Mode)
				Send_Parameter_Mode();
		
		#endif
	}
	else
	{
		//运算后发送的内容
		
		//向飞控发送内容
		
		Camera_Data_Send();	//发送运算结果数据
	
		//向上位机发送内容
		
		#if defined(__DISPLAY_RESULT)
		
			if(flag_Result)
				Display_Result();	//从串口输出图像，配合山外多功能调试助手显示
		
		#endif
		
			
		#if defined(__DISPALY_WAVE)
		
			if(flag_Wave)
				Display_Wave();	//串口输出波形
		
		#endif
		
			
		#ifdef __SD_SAVE		//SD存图

			if(SD_State)	//如果SD卡挂载成功
			{
				if(flag_Sd_gray)
				{
					TO_SDcard_OneFile(1);		//灰度图
					TO_SDcard_Height();			//记录高度数据
				}
				
				if(flag_Sd_result)
				{
					TO_SDcard_OneFile(2);		//灰度图
					TO_SDcard_Height();			//记录高度数据
				}
			}

		#endif
	}

}

//*********************** 总执行函数 **********************************************

uint8_t image_updata_flag = 0;		//新图像采集完成标志  0：新图没有采集完成    1：新图采集完成
uint8_t processing_ready = 1;	//首次置1，因为一开始的时候没有图像，第一次无法启动运算
void Image_Process(void)
{

	//等待新图
	while(!image_updata_flag){}
	
	image_updata_flag = 0;	//新图已经开始被使用，新图标志清零

	processing_fps_temp++;	//计算运算帧率
	
	Creat_Gray();	//生成灰度矩阵，数据来自显示缓冲
	Creat_Column();	//转换为列向量矩阵
	
	//处理速率计算
	if(Task_Delay[1]==0)
	{
		Task_Delay[1]=5000; //此值每1ms会减1，减到0才可以重新进来这里
		
		processing_fps = processing_fps_temp/5.0f;	//计算当前计算帧率
		processing_fps_temp =0;						//重置
	}

	Image_Output(0);	//数据输出（原始图像相关内容）
	Image_Fix();		//图像处理函数
	Column_To_Line();	//从列向量矩阵恢复为行矩阵
	Image_Output(1);	//数据输出（运算后内容）

	processing_ready = 1;	//运算结束置1

}

