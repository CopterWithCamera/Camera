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
#include "include.h"

extern unsigned int Task_Delay[];

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
	
	TimingDelay_Decrement();	//��ʱ�������жϷ���
	
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
extern uint8_t fps;


//�������LCD_DISPLAY��include.h�У����ͱ���LCD����
#ifdef LCD_DISPLAY

//���� --> �Դ�
uint16_t line_num0 =1;
void DMA2_Stream0_IRQHandler(void)
{
	if(  DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0) == SET )    
	{
		/*�м���*/
		line_num0 = line_num0 + 1;

		if(line_num0 >= IMG_HEIGHT * 2 +1)
		{
			/*������һ֡,������λ*/
			line_num0=1;
		}
		else  /*DMA һ��һ�д���*/
		{  
			//FSMC_LCD_ADDRESS �� ����ͷ�ɼ�ͼ��Ļ�������ַ
			DMA_AtoB_Config(FSMC_LCD_ADDRESS + lcd_width*2*(line_num0-1),LCD_FRAME_BUFFER+(1600*(line_num0-1)));
		}

	}
	DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);
}

#endif

//DCMI --> ����
static uint16_t line_num =0;	//��¼�����˶�����
void DMA2_Stream1_IRQHandler(void)
{
  if(  DMA_GetITStatus(DMA2_Stream1,DMA_IT_TCIF1) == SET )    
  {
		/*�м���*/
		line_num++;

		if(line_num==img_height)
		{
			/*������һ֡,������λ*/
			line_num=0;
		}
		
		/*DMA һ��һ�д���*/
		OV5640_DMA_Config(FSMC_LCD_ADDRESS+(lcd_width*2*(lcd_height-line_num-1)),img_width*2/4);

		DMA_ClearITPendingBit(DMA2_Stream1,DMA_IT_TCIF1);
	}
}


//ʹ��֡�ж�����line_num,�ɷ�ֹ��ʱ�����ݵ�ʱ��DMA������������ƫ��
//��ʾ֡���������Ҳ����һ֡ͼ������ɣ�ȫ�������Ѿ�ͨ��DMA���䵽�Դ��У�DCMI->CR�����㣬��ʱ���Ե���DCMI_CaptureCmd(ENABLE)��ȡ��һ֡
void DCMI_IRQHandler(void)	
{

	if(  DCMI_GetITStatus (DCMI_IT_FRAME) == SET )    
	{
		/*������һ֡��������λ*/
		line_num=0;
		
		#if FRAME_RATE_DISPLAY	
		fps++; //֡�ʼ���
		#endif
		
		image_updata_flag = 1;	//��ʾһ֡ͼ���Ѿ�ȫ�����뻺��
		
		DCMI_ClearITPendingBit(DCMI_IT_FRAME); 
	}
}



extern int flag;
//KEY1�ⲿ�ж�
void KEY1_IRQHandler(void)
{
	//ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
		
//		printf("Key1\n");
		
		if(flag != 0)
		{
			Image_Process();
		}

		//����жϱ�־λ
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);
	}  
}

//KEY2�ⲿ�ж�
void KEY2_IRQHandler(void)
{
	//ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
		if(flag == 1)
			flag = 0;
		else
			flag = 1;
			
//		printf("Key2\n");
		
		//����жϱ�־λ
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
