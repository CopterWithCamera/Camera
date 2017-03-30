#include "image_processing.h"
//#include "math.h"

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
	
//图像缓存数组，第一行是原图，第二行是处理后的图
uint8_t CAMERA_BUFFER_ARRAY[2][ IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//长度*宽度*2个字节  *  2块区域
//uint8_t CAMERA_BUFFER_ARRAY[2][ IMG_WIDTH*IMG_HEIGHT*2] ;	//长度*宽度*2个字节  *  2块区域

uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT];	//第一块灰度空间，默认提供灰度数据
uint8_t temp_array[IMG_WIDTH*IMG_HEIGHT];


//所有取数据的函数以Get开头
//所有存数据的函数以To开头

//生成灰度矩阵
void Creat_Gray()
{
	uint32_t r,g,b;
	
	uint16_t i;
	
	for(i=0;i<IMG_WIDTH*IMG_HEIGHT*2;i=i+2)
	{
		r = (CAMERA_BUFFER_ARRAY[0][i+1] >> 3) * 8;
		g = ((CAMERA_BUFFER_ARRAY[0][i+1] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[0][i] >> 5) * 4;
		b = (CAMERA_BUFFER_ARRAY[0][i] & 0x1F) * 8;
		
		gray_array[i/2] = (r * 299 + g * 587 + b * 114 + 500) / 1000;
	}
}

void Creat_LCD(void)
{
	uint32_t i;
	uint8_t rb,g;
	
	//循环选取第i行第j列的值
	for(i = 0 ; i<IMG_HEIGHT*IMG_WIDTH; i++ )	//行扫描
	{
		rb = gray_array[i] / 8;
		g = gray_array[i] / 4;
		
		CAMERA_BUFFER_ARRAY[1][i*2+1] = (rb << 3) + ((g >> 3) & 0x07);	//低八位在后面，高八位在前面
		CAMERA_BUFFER_ARRAY[1][i*2] = (g << 5) + rb;
	}
}

//1.获取单点灰度区数值
uint8_t Get_Gray(uint16_t row,uint16_t column)	//第row行，第column个
{
	//计算点的方式是先确定第row行，再确定在本行中的第column个数值。
	//行：row      范围：1 -- IMG_HEIGHT
	//列：column   范围：1 -- IMG_WIDTH
	
	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	return gray_array[num/2];
}

//2.存储单点数据到暂存区
void To_Temp(uint16_t row,uint16_t column,uint8_t gray)
{
	//计算点的方式是先确定第row行，再确定在本行中的第column个数值。
	//行：row      范围：1 -- IMG_HEIGHT
	//列：column   范围：1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	temp_array[num/2] = gray;
}


//3.获取单点暂存区数值
uint8_t Get_Temp(uint16_t row,uint16_t column)
{
	//计算点的方式是先确定第row行，再确定在本行中的第column个数值。
	//行：row      范围：1 -- IMG_HEIGHT
	//列：column   范围：1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	return temp_array[num/2];
}

//4.存储单点数据到灰度区
void To_Gray(uint16_t row,uint16_t column,uint8_t gray)
{
	//计算点的方式是先确定第row行，再确定在本行中的第column个数值。
	//行：row      范围：1 -- IMG_HEIGHT
	//列：column   范围：1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	gray_array[num/2] = gray;
}



void Image_Fix(void)	//图像算法
{
	uint32_t i,j;
	
	
	
	
		//动态阈值法

//	//这只是简单的图像复制
//	for(i = 0;i<IMG_WIDTH*IMG_HEIGHT*2;i++)
//	{
//		CAMERA_BUFFER_ARRAY[1][i] = CAMERA_BUFFER_ARRAY[0][i];
//		
//		
//	}
	
	//使用8位灰度函数
//	for(i = 1;i<=IMG_HEIGHT;i++)
//	{
//		for(j = 1;j<=IMG_WIDTH;j++)
//		{
//			To_LCD(i,j,		To_Gray(i,j)	,0x80);
//		}
//	}
	
	//灰度 --> 暂存
	for(i = 1;i<=IMG_HEIGHT;i++)
	{
		for(j = 1;j<=IMG_WIDTH;j++)
		{
			To_Temp(i,j,Get_Gray(i,j));
		}
	}
	
	//暂存 --> 灰度
	for(i = 1;i<=IMG_HEIGHT;i++)
	{
		for(j = 1;j<=IMG_WIDTH;j++)
		{
			To_Gray(i,j,Get_Temp(i,j));
		}
	}
	
	
	
//    float value=0;
//	float a,b,c,d,threhold=0;
//	int done=0;
//	i=0;j=0;//int i ,j = 0;
//	for(i = 2;i<IMG_HEIGHT ; i++)
//	{	
//		for (j = 2;j<IMG_WIDTH ; j++)	
//		{
//	    a=(To_Gray(i-1,j-1)+To_Gray(i,j-1)+To_Gray(i+1,j-1))-(To_Gray(i-1,j+1)+To_Gray(i,j-1)+To_Gray(i+1,j+1));
//        a=my_abs(a);
//		b=(To_Gray(i-1,j-1)+To_Gray(i-1,j)+To_Gray(i-1,j+1))-(To_Gray(i+1,j-1)+To_Gray(i+1,j)+To_Gray(i+1,j+1));
//	    b=my_abs(b);
//		 if (a>b)
//		 { c=a;}
//		 else 
//          {c=b;}
//		 To_Gray_LCD(i,j,c);
//		}	
//	}


		
}

//非DMA方式显示
void Camera_Buffer_To_Lcd_Buffer(void)
{
	int i,j;
	
	//原图区
	for(i = 0;i<IMG_HEIGHT;i++)
	{
		for(j = 0;j<IMG_WIDTH*2;j=j+1)
		{
			LCD_FRAME_BUFFER_ARRAY[i*1600+j] = CAMERA_BUFFER_ARRAY[0][i*IMG_WIDTH*2+j];
		}
			
	}
	
	//运算后图区
	for(i = 0;i<IMG_HEIGHT;i++)
	{
		for(j = 0;j<IMG_WIDTH*2;j=j+1)
		{
			LCD_FRAME_BUFFER_ARRAY[1600*140 + i*1600+j] = CAMERA_BUFFER_ARRAY[1][i*IMG_WIDTH*2+j];
		}
			
	}
	
	//800*480
//	for(i=0;i<IMG_WIDTH*IMG_HEIGHT*2;i++)
//	{
//		LCD_FRAME_BUFFER_ARRAY[i] = CAMERA_BUFFER_ARRAY[0][i];
//	}
	
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

uint8_t image_updata_flag = 0;
void Image_Process(void)
{	 
	DCMI_CaptureCmd(ENABLE);			//读取一帧图像到缓存
	
	//新写法
	//延时150ms 或 图像采集完成中断置位
	image_updata_flag = 0;
	Task_Delay[9] = 1000;
	while(Task_Delay[9]!=0 && image_updata_flag == 0){}
	
//	image_updata_flag = 0;
//	Task_Delay[9] = 1000;
//	while(1)
//	{
//		if(Task_Delay[9]==0)
//		{
//			break;
//		}
//			
//		if(Task_Delay[9]<=500)
//		{
//			if(image_updata_flag != 0)
//			{
//				break;
//			}
//			break;
//		}

//	}
	
	//老写法
//	Delay(150);	//加150ms延迟，确保DMA2已经完成从DCMI读到显存中
	
	Creat_Gray();	//生成灰度矩阵
	Image_Fix();	//图像处理函数
	Creat_LCD();	//还原RGB565图像

	//Camera_Buffer_To_Lcd_Buffer();	//图像从缓存搬运到显存
	DMA_AtoB_Config(FSMC_LCD_ADDRESS,LCD_FRAME_BUFFER);

}


//旧的单点计算灰度函数

//uint8_t To_Gray(uint16_t row,uint16_t column)
//{
//	//行：coiumn   范围：1 -- IMG_WIDTH
//	//列：row      范围：1 -- IMG_HEIGHT

//	uint32_t r,g,b;
//	uint32_t num;
//	uint32_t gray; 
//	
//	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
//	
//	r = (CAMERA_BUFFER_ARRAY[0][num] >> 3) * 8;
//	g = ((CAMERA_BUFFER_ARRAY[0][num] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[0][num+1] >> 5) * 4;
//	b = (CAMERA_BUFFER_ARRAY[0][num+1] & 0x1F) * 8;

//	gray = (r * 299 + g * 587 + b * 114 + 500) / 1000;
//	
//	if(gray >= 256)
//		printf("灰度算法错误！\n");
//	
//	return (uint8_t)gray;
//}


//旧的单点转换为RGB565函数

//void To_LCD(uint16_t row,uint16_t column,uint8_t gray,uint8_t threshold)
//{
//	uint32_t num;
//	
//	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
//	
//	if(gray > threshold)
//	{
//		CAMERA_BUFFER_ARRAY[1][num] = 0xFF;
//		CAMERA_BUFFER_ARRAY[1][num+1] = 0xFF;
//	}
//	else
//	{
//		CAMERA_BUFFER_ARRAY[1][num] = 0x00;
//		CAMERA_BUFFER_ARRAY[1][num+1] = 0x00;
//	}
//}

//void To_Gray_LCD(uint16_t row,uint16_t column,uint8_t gray)
//{
//	uint32_t num;
//	uint8_t rb,g;
//	
//	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
//	
//	rb = gray / 8;
//	g = gray / 4;
//	
//	CAMERA_BUFFER_ARRAY[1][num+1] = (rb << 3) + ((g >> 3) & 0x07);	//低八位在后面，高八位在前面
//	CAMERA_BUFFER_ARRAY[1][num] = (g << 5) + rb;
//	
//}
