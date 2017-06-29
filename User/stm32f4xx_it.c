/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./camera/bsp_ov5640.h"
#include "./systick/bsp_SysTick.h"
#include "./key/bsp_exti.h"
#include "image_processing.h"
#include "include.h"

/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
	printf("\r\n hardfault!");
	}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

	
extern uint32_t Task_Delay[];
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	uint8_t i;
	
	TimingDelay_Decrement();	//延时函数的中断服务
	
	for(i=0;i<NumOfTask;i++)
	{
		if(Task_Delay[i])
		{
			Task_Delay[i]--;
		}
	}
}



/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

extern uint16_t lcd_width, lcd_height;
extern uint16_t img_width, img_height;

//DCMI --> 缓存
//static uint16_t line_num =0;	//记录传输了多少行
void DMA2_Stream1_IRQHandler(void)								//这个DMA中断已经被关了
{
  if(  DMA_GetITStatus(DMA2_Stream1,DMA_IT_TCIF1) == SET )    
  {
//		/*行计数*/
//		line_num++;

//		if(line_num==img_height)
//		{
//			/*传输完一帧,计数复位*/
//			line_num=0;
//		}
		
		/*DMA 一行一行传输*/
		//OV5640_DMA_Config(FSMC_LCD_ADDRESS+(lcd_width*2*(lcd_height-line_num-1)),img_width*2/4);
		//OV5640_DMA_Config(FSMC_LCD_ADDRESS,img_height*img_width*2/4);	//一次传输整张图，DMA的最大长度是2^16（猜的，长了会有BUG）
	  
		//DMA的数据默认进入DCMI_IN_BUFFER_ARRAY对应区域
		//OV5640_DMA_Config((uint32_t)DCMI_IN_BUFFER_ARRAY,img_height*img_width*2/4);
	  
		DMA_ClearITPendingBit(DMA2_Stream1,DMA_IT_TCIF1);
	}
}


//使用帧中断重置line_num,可防止有时掉数据的时候DMA传送行数出现偏移
//表示帧捕获结束，也就是一帧图像传输完成，全部数据已经通过DMA传输到显存中，DCMI->CR被清零，此时可以调用DCMI_CaptureCmd(ENABLE)读取下一帧
void DCMI_IRQHandler(void)	
{
	uint8_t * tmp;
	
	if(  DCMI_GetITStatus (DCMI_IT_FRAME) == SET )    
	{
		/*传输完一帧，计数复位*/
//		line_num=0;
		
		fps_temp++; //帧率计数
		
		//进到这里了，就表示已经采完了，满足采集完毕条件
		
		//运算已经结束，则对换空间
		if(processing_ready)
		{
			processing_ready = 0;	//清零运算完成标志位，等待下次运算完成
			
			//空间对换
			tmp = DCMI_IN_BUFFER_ARRAY;
			DCMI_IN_BUFFER_ARRAY = CAMERA_BUFFER_ARRAY;
			CAMERA_BUFFER_ARRAY = tmp;
			
			//换完之后新图就位，开启新图完成标志位
			//表示 CAMERA_BUFFER_ARRAY 区域中已经提供了一张新图用于运算
			image_updata_flag = 1;
		}
		
		OV5640_DMA_Config((uint32_t)DCMI_IN_BUFFER_ARRAY,img_height*img_width*2/4);	//空间交换已经完成后，再开启新的DMA（保证DMA的对于区域已经被更新）
		
		DCMI_ClearITPendingBit(DCMI_IT_FRAME); 
	}
}


//KEY1外部中断
void KEY1_IRQHandler(void)	//核心板上的按钮
{
	//确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
//		printf("Key1\n");
		
		Mode_Change();
		
		//清除中断标志位
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);
	}  
}

//KEY2外部中断
void KEY2_IRQHandler(void)
{
	//确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
			
//		printf("Key2\n");
		
		//清除中断标志位
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
}




/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
