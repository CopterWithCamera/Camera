#ifndef __INCLUDE_H
#define __INCLUDE_H

#include "stm32f4xx.h"
#include <stdlib.h>
#include "./math/mymath.h"

//显示帧率数据，默认不显示，需要显示时把这个宏设置为1即可，（会有轻微花屏现象！）
//经测试液晶显示摄像头数据800*480像素，帧率为14.2帧/秒。

#define FRAME_RATE_DISPLAY 	1

//设置变量定义到“EXRAM”节区的宏
#define __EXRAM  __attribute__ ((section ("EXRAM")))

//	#define LCD_DISPLAY

extern uint8_t dispBuf[100];

#endif /* __INCLUDE_H */
