/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   OV5640摄像头显示例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 STM32  F429开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"

#include "include.h"

#include "./usart/bsp_debug_usart.h"
#include "./sdram/bsp_sdram.h"
#include "./camera/bsp_ov5640.h"
#include "./systick/bsp_SysTick.h"
#include "./camera/ov5640_AF.h"
#include "./key/bsp_exti.h"
#include "image_processing.h"
#include "ff.h"
#include "rgbTObmp.h"
#include "bsp_spi_nrf.h"

/*						*\
		简单任务管理
		
0 采图帧率
1 运算帧率
2 持续写入模式下定时保存SD卡文件
3
4
5
6
7
8
9

\*						*/

uint32_t Task_Delay[NumOfTask];

//帧率计算
uint8_t fps_temp = 0;
float fps = 0;
uint8_t processing_fps_temp = 0;
float processing_fps = 0;

void My_Camera_Init(void)
{
	OV5640_IDTypeDef OV5640_Camera_ID;
	
	/* 初始化摄像头GPIO及IIC */
	OV5640_HW_Init();   		//同F407工程相同，不必更改

	/* 读取摄像头芯片ID，确定摄像头正常连接 */
	OV5640_ReadID(&OV5640_Camera_ID);

	if(OV5640_Camera_ID.PIDH  == 0x56)
	{
		CAMERA_DEBUG("%x %x\r\n",OV5640_Camera_ID.PIDH ,OV5640_Camera_ID.PIDL);
	}
	else
	{
		CAMERA_DEBUG("未检测到摄像头\r\n");
		while(1);  
	}

	OV5640_Init();			//DCMI  DMA  INTERRUPT
	OV5640_RGB565Config();
	OV5640_AUTO_FOCUS();
	
}


//定义变量到SDRAM
uint32_t testValue __EXRAM =7 ;
//定义数组到SDRAM
uint8_t testGrup[4] __EXRAM ={1,2,3,4};

//定义变量到SRAM
uint32_t testValue2  =7 ;
//定义数组到SRAM
uint8_t testGrup2[3] ={1,2,3};

void My_RAM_TEST(void)
{
	uint32_t inerTestValue =10;
	char ch;

	printf("\r\n RAM分配测试程序 \r\n\r\n");
	
	ch = testValue & 0xFF;
	USART_SendData(DEBUG_USART, (uint8_t) ch);
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
	
	ch = (testValue>>8) & 0xFF;
	USART_SendData(DEBUG_USART, (uint8_t) ch);
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
	
	ch = (testValue>>16) & 0xFF;
	USART_SendData(DEBUG_USART, (uint8_t) ch);
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
	
	ch = (testValue>>24) & 0xFF;
	USART_SendData(DEBUG_USART, (uint8_t) ch);
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
	
	printf("0\r\n");
	printf("局部变量：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&inerTestValue,inerTestValue);
	
	printf("1\r\n");
	printf("全局SDRAM：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&testValue,testValue);
	
	printf("2\r\n");
	printf("全局SDRAM数组：它的地址为：0x%x,变量值为：%d,%d,%d\r\n",(uint32_t)testGrup,testGrup[0],testGrup[1],testGrup[2]);
	
	printf("3\r\n");
	printf("全局RAM：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&testValue2,testValue2);
	
	printf("4\r\n");
	printf("全局RAM数组：它的地址为：0x%x,变量值为：%d，%d,%d\r\n",(uint32_t)&testGrup2,testGrup2[0],testGrup2[1],testGrup2[2]);
	
	printf("\r\n");
	printf("全局SDRAM：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&testValue,testValue);
	printf("全局SDRAM数组：它的地址为：0x%x,变量值为：%d,%d,%d\r\n",(uint32_t)testGrup,testGrup[0],testGrup[1],testGrup[2]);

//	printf("全局CAMERA_BUFFER_ARRAY：它的地址为：0x%x\r\n",(uint32_t)&CAMERA_BUFFER_ARRAY);
	
	
//	LCD_FRAME_BUFFER_ARRAY
//	CAMERA_BUFFER_ARRAY	
}

//禁用WiFi模块（用sd卡必须禁用wifi，原因不明）
void BL8782_PDN_INIT(void)
{
  /*定义一个GPIO_InitTypeDef类型的结构体*/
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOG, ENABLE); 							   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);	
  
  GPIO_ResetBits(GPIOG,GPIO_Pin_9);  //禁用WiFi模块
}

//SD卡初始化函数

u8 SDCard_Init(void)
{
	FATFS fs;					/* Work area (file system object) for logical drives */
	FRESULT res_sd_conf; 
	
	/*禁用wifi模块*/
	BL8782_PDN_INIT();
	
	//SD卡挂载
	res_sd_conf = f_mount(&fs,"0:",1);  //挂载文件系统
	
	//如果没有文件系统，则要格式化，再挂载
//	if(res_sd_conf == FR_NO_FILESYSTEM)
	if(res_sd_conf != FR_OK)				//只要不是FR_OK都直接返回SD卡状态错误，不进行错误处理
	{
		printf("SD卡挂载失败\r\n");
		return 0;
		
//		printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
//		/* 格式化 */
//		res_sd_conf=f_mkfs("0:",0,0);							
//		
//		if(res_sd_conf == FR_OK)
//		{
//			printf("》SD卡已成功格式化文件系统。\r\n");
//			res_sd_conf = f_mount(NULL,"0:",1);			/* 格式化后，先取消挂载 */
//			res_sd_conf = f_mount(&fs,"0:",1);		/* 重新挂载	*/	
//		}
//		else
//		{
//			printf("《《格式化失败。》》\r\n");
//			return 0;	//SD卡挂载失败
//		}
	}

	printf("SD卡挂载成功\r\n");
	
	return 1;	//SD卡挂载成功
}


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */

extern  uint32_t CurrentFrameBuffer;

int SD_State = 0;	//SD卡状态 0 -- 挂载失败  1 -- 挂载成功
int main(void)
{
	/*摄像头与RGB LED灯共用引脚，不要同时使用LED和摄像头*/
	
	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);	//设定中断优先级分组
	
	Debug_USART_Config();	//串口1
	USART2_Config();		//串口2
	SysTick_Init();		//用于延时函数
	My_RAM_TEST();		//内存分配情况测试
	EXTI_Key_Config();	//初始化外部中断按键

	My_Camera_Init();	//初始化OV5640
	
	#ifdef __NRF_DISPLAY
	
		NRF24L01_Init();	//NRF24L01
	
	#endif
	
	#ifdef __SD_SAVE
		SD_State = SDCard_Init();	//初始化SD卡  0 -- 挂载失败  1 -- 挂载成功
	#endif
	
	//开启传输
	OV5640_DMA_Config((uint32_t)DCMI_IN_BUFFER_ARRAY,img_height*img_width*2/4);	//开启第一次传输
	DCMI_Cmd(ENABLE);
	DCMI_CaptureCmd(ENABLE);
	
	Mode_Set();	//设置数据传输模式
	
	/*DMA直接传输摄像头数据到LCD屏幕显示*/
	while(1)
	{
		
		Image_Process();	//图像处理函数，包括读图和写入显存
		
		//帧率计算
		if(Task_Delay[0]==0)
		{
			Task_Delay[0]=5000; //此值每1ms会减1，减到0才可以重新进来这里
			
			fps = fps_temp/5.0f;
			fps_temp =0;	//重置
		}
		
		//持续写入SD卡模式下定时保存文件数据
		if(Task_Delay[2]==0)
		{
			Task_Delay[2]=1000; //此值每1ms会减1，减到0才可以重新进来这里
			
			if(ToOneFile_StartFlag)
				f_sync(&ToOneFile_f);
			
			if(Height_StartFlag)
				f_sync(&Height_f);
		}
	}
}

/*********************************************END OF FILE**********************/

