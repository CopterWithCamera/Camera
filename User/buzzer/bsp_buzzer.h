#ifndef __BUZZER_H
#define	__BUZZER_H

#include "stm32f4xx.h"

//引脚定义
/*******************************************************/

//小指示灯
#define BUZZER_PIN                  GPIO_Pin_1              
#define BUZZER_GPIO_PORT            GPIOC                
#define BUZZER_GPIO_CLK             RCC_AHB1Periph_GPIOC
/************************************************************/


/** 控制LED灯亮灭的宏，
	* LED低电平亮，设置ON=0，OFF=1
	* 若LED高电平亮，把宏设置成ON=1 ，OFF=0 即可
	*/
#define ON  0
#define OFF 1

/* 带参宏，可以像内联函数一样使用 */

#define BUZZER(a)	if (a)	\
					GPIO_SetBits(BUZZER_GPIO_PORT,BUZZER_PIN);\
					else		\
					GPIO_ResetBits(BUZZER_GPIO_PORT,BUZZER_PIN);

/* 基本混色，后面高级用法使用PWM可混出全彩颜色,且效果更好 */

void BUZZER_GPIO_Config(void);

#endif /* __LED_H */
