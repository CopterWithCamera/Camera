/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ledӦ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./buzzer/bsp_buzzer.h"

 /**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void BUZZER_GPIO_Config(void)
{		
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����BUZZER��ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOC, ENABLE); 
												   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;			/*ѡ��Ҫ���Ƶ�GPIO����*/	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   	/*��������ģʽΪ���ģʽ*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/*�������ŵ��������Ϊ�������*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		/*��������Ϊ����ģʽ��Ĭ��LED��*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 	/*������������Ϊ50MHz */
	GPIO_Init(GPIOC, &GPIO_InitStructure);				/*ѡ��Ҫ���Ƶ�GPIO����*/	

	/*ָʾ��Ĭ�Ͽ���*/
	BUZZER(OFF);
		
}
/*********************************************END OF FILE**********************/
