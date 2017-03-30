#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

/*简单任务管理*/
#define TASK_ENABLE 0
#define NumOfTask 10

extern uint32_t Task_Delay[NumOfTask];

void SysTick_Init(void);
void Delay_ms(__IO u32 nTime);
void TimingDelay_Decrement(void);

#endif /* __SYSTICK_H */
