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

uint8_t CAMERA_BUFFER_ARRAY1[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//长度*宽度*2个字节
uint8_t CAMERA_BUFFER_ARRAY2[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//长度*宽度*2个字节

uint8_t * CAMERA_BUFFER_ARRAY = CAMERA_BUFFER_ARRAY1;	//当前数据指针
uint8_t * DCMI_IN_BUFFER_ARRAY = CAMERA_BUFFER_ARRAY2;	//当前输入缓存指针

//灰度图像存储空间
uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//长度*宽度*1字节

//运算结果存储空间
uint8_t result_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//长度*宽度*1字节

//输出参数
float length;	//偏差
float speed;

//传输数据的模式
unsigned char mode = 0;

//控制传输的flag
u8 flag_Image = 0;
u8 flag_Result = 0;
u8 flag_Wave = 0;
u8 flag_Sd_original = 0;
u8 flag_Sd_gray = 0;
u8 flag_Sd_result = 0;

//参数传输flag（一般要开启）
u8 flag_Fps = 1;
u8 flag_Mode = 1;

//生成灰度矩阵
void Creat_Gray(void)
{
	uint32_t r,g,b;
	
	uint16_t i,j;

 	uint8_t tmp;
	
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

void Data_Output_Ctrl(unsigned char cmd)
{
	switch(cmd)
	{
		case 1:
			flag_Image = 1;
			break;
		case 2:
			flag_Image = 0;
			break;
		case 3:
			flag_Result = 1;
			break;
		case 4:
			flag_Result = 0;
			break;
		case 5:
			flag_Wave = 1;
			break;
		case 6:
			flag_Wave = 0;
			break;
		case 7:
			flag_Fps = 1;
			break;
		case 8:
			flag_Fps = 0;
			break;
		case 9:
			flag_Sd_original = 1;
			break;
		case 10:
			flag_Sd_original = 0;
			break;
		default:
			break;
	}
}

void Mode_Set(void)	//模式设置函数，在初始化过程中和切换模式时均有调用
{
	switch(mode)
	{
		case 0:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_original = 0;
			flag_Sd_gray = 1;
			flag_Sd_result = 0;
		break;
		
		case 1:
			flag_Image = 1;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_original = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
		break;
		
		case 2:
			flag_Image = 0;
			flag_Result = 1;
			flag_Wave = 0;
			flag_Sd_original = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
		break;
		
		case 3:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 1;
			flag_Sd_original = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
		break;
		
		case 4:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_original = 1;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
		break;
		
		case 5:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_original = 0;
			flag_Sd_gray = 1;
			flag_Sd_result = 0;
		break;
		
		case 6:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_original = 0;
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
	
	if(!full_flag)
	{
		if(!mode)
		{
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
			#if defined(__DISPLAY_RESULT)
			
				if(flag_Result)
					Display_Result();	//从串口输出图像，配合山外多功能调试助手显示
			
			#endif
			
				
			#if defined(__DISPALY_WAVE)
			
				if(flag_Wave)
					Display_Wave();	//串口输出波形
			
			#endif
				
			//SD存图	
				
			#ifdef __SD_SAVE

				//保存完整图片
				#if (__SD_SAVE_MODE == 0)
				
					if(SD_State)	//如果SD卡挂载成功
					{
						//原始彩图
						#if defined(__SD_SAVE_ORIGINAL)

							if(flag_Sd_original)
								TO_SDcard(0);    
						
						#endif
						
						//灰度图片
						#if defined(__SD_SAVE_GRAY)
						
							if(flag_Sd_gray)
								TO_SDcard(1);    //
						
						#endif
						
						//Result矩阵图
						#if defined(__SD_SAVE_RESULT)
							
							if(flag_Sd_result)
								TO_SDcard(2);    
						
						#endif
					}
				
				//持续写入图片
				#elif (__SD_SAVE_MODE == 1)
					
					if(flag_Sd_gray)
						TO_SDcard_OneFile(1);		//灰度图
					
				#endif
	
			#endif
		}
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
	
	//处理速率计算
	if(Task_Delay[1]==0)
	{
		Task_Delay[1]=5000; //此值每1ms会减1，减到0才可以重新进来这里
		
		processing_fps = processing_fps_temp/5.0f;	//计算当前计算帧率
		processing_fps_temp =0;						//重置
	}
		
	Image_Output(0);	//数据输出（原始图像相关内容）
	Image_Fix();	//图像处理函数
	Image_Output(1);	//数据输出（运算后内容）

	processing_ready = 1;	//运算结束置1

}

