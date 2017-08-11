#ifndef __BUZZER_H
#define	__BUZZER_H

#include "stm32f4xx.h"

//���Ŷ���
/*******************************************************/

//Сָʾ��
#define BUZZER_PIN                  GPIO_Pin_1              
#define BUZZER_GPIO_PORT            GPIOC                
#define BUZZER_GPIO_CLK             RCC_AHB1Periph_GPIOC
/************************************************************/


/** ����LED������ĺ꣬
	* LED�͵�ƽ��������ON=0��OFF=1
	* ��LED�ߵ�ƽ�����Ѻ����ó�ON=1 ��OFF=0 ����
	*/
#define ON  0
#define OFF 1

/* ���κ꣬��������������һ��ʹ�� */

#define BUZZER(a)	if (a)	\
					GPIO_SetBits(BUZZER_GPIO_PORT,BUZZER_PIN);\
					else		\
					GPIO_ResetBits(BUZZER_GPIO_PORT,BUZZER_PIN);

/* ������ɫ������߼��÷�ʹ��PWM�ɻ��ȫ����ɫ,��Ч������ */

void BUZZER_GPIO_Config(void);

#endif /* __LED_H */
