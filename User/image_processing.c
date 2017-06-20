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
 * 显存地址： LCD_FRAME_BUFFER
 *
 * 摄像头DMA2配置方式：
 * #define FSMC_LCD_ADDRESS      LCD_FRAME_BUFFER
 * 把 FSMC_LCD_ADDRESS 的定义配成希望摄像头数据存的地方就行了
 * 缓存大小一定要用 img_width 和 img_height 计算
 * 
 */
 
//**************************************************************
 
uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT];	//第一块灰度空间，默认提供灰度数据
	
//**************************************************************	
	
//图像缓存数组，第一行是原图，第二行是处理后的图
uint8_t CAMERA_BUFFER_ARRAY[2 * IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//长度*宽度*2个字节  *  2块区域

float length;	//偏差
float speed;

//所有取数据的函数以Get开头
//所有存数据的函数以To开头

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

void Creat_LCD(void)
{
	uint32_t i;
	uint8_t rb,g;
	
	for(i = 0 ; i<IMG_HEIGHT*IMG_WIDTH; i++ )
	{
		rb = gray_array[i] / 8;
		g = gray_array[i] / 4;
		
		CAMERA_BUFFER_ARRAY[IMG_HEIGHT*IMG_WIDTH*2 + i*2+1] = (rb << 3) + ((g >> 3) & 0x07);	//低八位在后面，高八位在前面
		CAMERA_BUFFER_ARRAY[IMG_HEIGHT*IMG_WIDTH*2 + i*2] = (g << 5) + rb;
	}
}

//获取单点灰度区数值
uint8_t Get_Gray(uint16_t row,uint16_t column)	//第row行，第column个
{
	//计算点的方式是先确定第row行，再确定在本行中的第column个数值。
	//行：row      范围：1 -- IMG_HEIGHT
	//列：column   范围：1 -- IMG_WIDTH
	
	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	return gray_array[num/2];
}

//存储单点数据到灰度区
void To_Gray(uint16_t row,uint16_t column,uint8_t gray)
{
	//计算点的方式是先确定第row行，再确定在本行中的第column个数值。
	//行：row      范围：1 -- IMG_HEIGHT
	//列：column   范围：1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	gray_array[num/2] = gray;
}



//************** 输出信息 ************************************************

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

//显示矩阵，直接用串口调试助手查看
void Display_Matrix(void)
{
	uint32_t i,j;
	uint8_t ch,tmp;
	
	//发送图像
	for(i = 0 ; i<IMG_HEIGHT; i++ )	//行扫描
	{
		for(j = 0;j<IMG_WIDTH;j++)	//列扫描
		{
			ch = gray_array[i];
		
			tmp = ch/100;
			tmp = tmp + 0x30;	//转ASCII码
			Data_Output(tmp);
			
			tmp = ch/10;
			tmp = tmp%10;
			tmp = tmp + 0x30;	//转ASCII码
			Data_Output(tmp);
			
			tmp = ch%10;
			tmp = tmp%10;
			tmp = tmp + 0x30;	//转ASCII码
			Data_Output(tmp);
			
			tmp = ',';
			Data_Output(tmp);
			
		}
		ch = '\r';Data_Output(ch);
		ch = '\n';Data_Output(ch);
	}
	
	ch = '\r';Data_Output(ch);
	ch = '\n';Data_Output(ch);
	
	ch = '\r';Data_Output(ch);
	ch = '\n';Data_Output(ch);
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


//******************** LCD显示内容 *********************************************************************

//如果定义__LCD_DISPLAY（include.h中），就编译LCD代码
#ifdef __LCD_DISPLAY

//非DMA方式显示
void Camera_Buffer_To_Lcd_Buffer(void)
{
	int i,j;
	
	//原图+运算后图（（IMG_HEIGHT*2） * IMG_WIDTH）
	for(i = 0;i<IMG_HEIGHT*2;i++)
	{
		for(j = 0;j<IMG_WIDTH*2;j=j+1)
		{
			LCD_FRAME_BUFFER_ARRAY[i*1600+j] = CAMERA_BUFFER_ARRAY[i*IMG_WIDTH*2+j];
		}
			
	}
	
}


void DMA_AtoB_Config(uint32_t DMA_Memory_A_Addr,uint32_t DMA_Memory_B_Addr)
{
	DMA_InitTypeDef  DMA_InitStructure;

	/* 使能DMA时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	/* 复位初始化DMA数据流 */
	DMA_DeInit(DMA2_Stream0);
	/* 确保DMA数据流复位完成 */
	while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){}

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = DMA_Memory_A_Addr;
	DMA_InitStructure.DMA_Memory0BaseAddr = DMA_Memory_B_Addr;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
	DMA_InitStructure.DMA_BufferSize = IMG_WIDTH;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh ;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;     
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC8;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_Init(DMA2_Stream0, &DMA_InitStructure); 
		
	DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);
		
	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE); 
		
	DMA_Cmd(DMA2_Stream0, ENABLE);
	while(DMA_GetCmdStatus(DMA2_Stream1) != ENABLE){}
  
}

void DMA2_Stream0_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	/* 配置中断 */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* 配置中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn ;//DMA数据流中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE); 	
	
}


//********************* 在LCD上绘图 *****************************************************************

//显示偏移距离和水平速度
void Display_data(void)
{	
	LCD_SetColors(LCD_COLOR_WHITE,TRANSPARENCY);
//	LCD_ClearLine(LINE(11));
//	LCD_ClearLine(LINE(14));
	sprintf((char*)dispBuf, "            ");
	LCD_DisplayStringLine_EN_CH(LINE(11),dispBuf);
	LCD_DisplayStringLine_EN_CH(LINE(14),dispBuf);
	
	sprintf((char*)dispBuf, "%.2f", length);
	LCD_DisplayStringLine_EN_CH(LINE(10),(uint8_t*)"偏移距离：");
	LCD_DisplayStringLine_EN_CH(LINE(11),dispBuf);
	sprintf((char*)dispBuf, "%.2f", speed);
	LCD_DisplayStringLine_EN_CH(LINE(13),(uint8_t*)"水平速度：");
	LCD_DisplayStringLine_EN_CH(LINE(14),dispBuf);
}



//绘制图形曲线
void Draw_Graph()
{
	/*
	
	x轴长度540
	y轴长度200（±100）
	
	每行画90个点，每个点间隔6个像素
	
	*/
	
	static int x = 0;
	int y1,y2 = 0;

	if(x == 0)
	{
		x = 0;
		
		//清屏
		LCD_SetColors(LCD_COLOR_BLACK, TRANSPARENCY);
		LCD_DrawFullRect(235,0,545,445);
		LCD_SetColors(LCD_COLOR_WHITE, TRANSPARENCY);
		
		//显示图例
		LCD_DisplayStringLine_EN_CH(LINE(1),(uint8_t*)"                length");
		LCD_DisplayStringLine_EN_CH(LINE(10),(uint8_t*)"                speed");
		
		//画坐标系
		
		//横轴
		LCD_DrawLine(240, 109, 540, 0);
		LCD_DrawLine(240, 110, 540, 0);
		LCD_DrawLine(240, 111, 540, 0);
		
		LCD_DrawLine(240, 329, 540, 0);
		LCD_DrawLine(240, 330, 540, 0);
		LCD_DrawLine(240, 331, 540, 0);
		
		//纵轴
		LCD_DrawLine(239, 10, 200, 1);
		LCD_DrawLine(240, 10, 200, 1);
		LCD_DrawLine(241, 10, 200, 1);
		
		LCD_DrawLine(239, 230, 200, 1);
		LCD_DrawLine(240, 230, 200, 1);
		LCD_DrawLine(241, 230, 200, 1);
		

	}
	
	//横轴累加
	x++;
	
	
	
	//位移
	y1 = 110 + length;
	LCD_SetColors(LCD_COLOR_BLUE2,TRANSPARENCY);
	LCD_DrawFullCircle(240 + x*6,(uint16_t)y1 ,2);
	
	//速度
	y2 = 330 + speed;
	LCD_SetColors(LCD_COLOR_BLUE2,TRANSPARENCY);
	LCD_DrawFullCircle(240 + x*6,(uint16_t)y2 ,2);
	
	if(x>=89)
	{
		x = 0;
	}
}

#endif

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

void Image_Output(void)
{
	//*******************************************************************
	//输出信息
	
	if(!full_flag)
	{
		#if defined(__DISPLAY_IMAGE)
		
			Display_Image();	//从串口输出图像，配合山外多功能调试助手显示
			
		#elif defined(__DISPLAY_MATRIX)
		
			Display_Matrix();	//从串口输出矩阵，直接在串口调试助手上查看
			
		#elif defined(__DISPALY_WAVE)
		
			Display_Wave();	//串口输出波形
		
		#endif
	}
	

	
	//*******************************************************************
	//LCD显示
	#ifdef __LCD_DISPLAY
	
		#if defined(__DISPALY_DATA)
		
			Display_data();	//显示偏移距离和水平速度
		
		#endif
		
		#if defined(__DISPALY_GRAPH)
		
			Draw_Graph();	//绘制图形曲线
		
		#endif
	
		Creat_LCD();	//还原RGB565图像，存入显示缓冲
		DMA_AtoB_Config(FSMC_LCD_ADDRESS,LCD_FRAME_BUFFER);		//用DMA把图像从缓存搬运到显存
	
	#endif
	
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

