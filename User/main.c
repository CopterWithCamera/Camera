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
#include "./lcd/bsp_lcd.h"
#include "./camera/bsp_ov5640.h"
#include "./systick/bsp_SysTick.h"
#include "./camera/ov5640_AF.h"
#include "./key/bsp_exti.h"
#include "image_processing.h"


/*简单任务管理*/
uint32_t Task_Delay[NumOfTask];

uint8_t dispBuf[100];
OV5640_IDTypeDef OV5640_Camera_ID;

uint8_t fps=0;

void My_Camera_Init(void)
{
	printf("\r\nSTM32F429 DCMI 驱动OV5640例程\r\n");
	LCD_DisplayStringLine_EN_CH(LINE(19),(uint8_t*) "OV5640");
	
	
	/* 初始化摄像头GPIO及IIC */
	OV5640_HW_Init();   

	/* 读取摄像头芯片ID，确定摄像头正常连接 */
	OV5640_ReadID(&OV5640_Camera_ID);

	if(OV5640_Camera_ID.PIDH  == 0x56)
	{
//		sprintf((char*)dispBuf, "              OV5640 摄像头,ID:0x%x", OV5640_Camera_ID.PIDH);
//		LCD_DisplayStringLine_EN_CH(LINE(0),(uint8_t*)dispBuf);
		CAMERA_DEBUG("%x %x\r\n",OV5640_Camera_ID.PIDH ,OV5640_Camera_ID.PIDL);

	}
	else
	{
		LCD_SetTextColor(LCD_COLOR_RED);
		LCD_DisplayStringLine_EN_CH(LINE(0),(uint8_t*) "         没有检测到OV5640，请重新检查连接。");
		CAMERA_DEBUG("没有检测到OV5640摄像头，请重新检查连接。\r\n");

		while(1);  
	}


	OV5640_Init();			//DCMI  DMA  INTERRUPT

	OV5640_RGB565Config();
//	OV5640_AUTO_FOCUS();

	//使能DCMI采集数据
	DCMI_Cmd(ENABLE); 
	DCMI_CaptureCmd(ENABLE); 	
}


void My_LCD_Init(void)
{
	/*初始化液晶屏*/
	LCD_Init();
	LCD_LayerInit();		//显示层配置，包括显存地址配置
	LTDC_Cmd(ENABLE);
	
	/*把背景层刷黑色*/
	LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BLACK);
	
	/*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
	LCD_SetTransparency(0xFF);
	LCD_Clear(TRANSPARENCY);
	
//	LCD_SetColors(LCD_COLOR_RED,TRANSPARENCY);

//	LCD_ClearLine(LINE(18));
//  LCD_DisplayStringLine_EN_CH(LINE(18),(uint8_t* )" 模式:WVGA 800x480");
	
}


//定义变量到SDRAM
uint32_t testValue __EXRAM =7 ;
//定义数组到SDRAM
uint8_t testGrup[3] __EXRAM ={1,2,3};

//定义变量到SRAM
uint32_t testValue2  =7 ;
//定义数组到SRAM
uint8_t testGrup2[3] ={1,2,3};

void My_RAM_TEST(void)
{
//	uint32_t inerTestValue =10;
//	char ch;

//	printf("\r\nRAM分配测试程序\r\n");
	
//	printf("结果：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&inerTestValue,inerTestValue);
	
//	ch = testValue & 0xFF;
//	USART_SendData(DEBUG_USART, (uint8_t) ch);
//	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
//	
//	ch = (testValue>>8) & 0xFF;
//	USART_SendData(DEBUG_USART, (uint8_t) ch);
//	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
//	
//	ch = (testValue>>16) & 0xFF;
//	USART_SendData(DEBUG_USART, (uint8_t) ch);
//	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
//	
//	ch = (testValue>>24) & 0xFF;
//	USART_SendData(DEBUG_USART, (uint8_t) ch);
//	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
//	printf("结果：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&testValue,testValue);
//	printf("结果：它的地址为：0x,变量值为：%d,%d,%d\r\n",testGrup[0],testGrup[1],testGrup[2]);
//	printf("结果：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&testValue2,testValue2);
//	printf("结果：它的地址为：0x%x,变量值为：%d，%d,%d\r\n",(uint32_t)&testGrup2,testGrup2[0],testGrup2[1],testGrup2[2]);
	
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
  
int flag = 0;	//用于按键中断
int main(void)
{
	/*摄像头与RGB LED灯共用引脚，不要同时使用LED和摄像头*/

	Debug_USART_Config();   
	
	My_RAM_TEST();	//先检查一下SDRAM是否正常,在USART1上有输出
	
	/* 配置SysTick 为1ms中断一次,时间到后触发定时中断，
	*进入stm32fxx_it.c文件的SysTick_Handler处理，通过数中断次数计时
	*/
	
	SysTick_Init();		//用于延时函数
	
	EXTI_Key_Config();	//初始化外部中断按键

	My_LCD_Init();		//初始化LCD
	
	My_Camera_Init();	//初始化OV5640
	
	DMA2_Stream0_Init();	//缓存 -> 显存

	/*DMA直接传输摄像头数据到LCD屏幕显示*/
	while(1)
	{	
//		//定时更新图像（用于测试）
//		if(Task_Delay[1]==0 && flag == 0)
//		{
//			//DCMI_CaptureCmd(ENABLE);
//			
//			Image_Process();	//图像处理函数，包括读图和写入显存
//			
//			Task_Delay[1]=100; //此值每1ms会减1，减到0才可以重新进来这里
//		}
		
		//全速更新
		if(flag == 0)
		{
			Image_Process();	//图像处理函数，包括读图和写入显存
		}
		
		//显示帧率，默认不显示		
		#if FRAME_RATE_DISPLAY	
		
		if(Task_Delay[0]==0)
		{
						
			LCD_SetColors(LCD_COLOR_RED,TRANSPARENCY);

			LCD_ClearLine(LINE(17));
			sprintf((char*)dispBuf, " %.1f/s", (float)(fps/5.0));
			
			/*输出帧率*/
			LCD_DisplayStringLine_EN_CH(LINE(17),dispBuf);
			//重置
			fps =0;
			
			Task_Delay[0]=5000; //此值每1ms会减1，减到0才可以重新进来这里
		}
			
		#endif
		
	}

}
/*********************************************END OF FILE**********************/

