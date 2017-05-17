#ifndef __INCLUDE_H
#define __INCLUDE_H

#include "stm32f4xx.h"
#include <stdlib.h>

//显示帧率数据，默认不显示，需要显示时把这个宏设置为1即可，（会有轻微花屏现象！）
//经测试液晶显示摄像头数据800*480像素，帧率为14.2帧/秒。

//设置变量定义到“EXRAM”节区的宏
#define __EXRAM  __attribute__ ((section ("EXRAM")))


//************ 功能开关 ***************

//帧率显示
#define FRAME_RATE_DISPLAY 		1

//LCD显示
#define LCD_DISPLAY

//图像传输方式选择（只能使能一个）
//#define __USART_DISPLAY_IMAGE	//显示图像
//#define __USART_DISPLAY_MATRIX	//显示矩阵
#define __USART_DISPALY_WAVE	//显示波形

#define __DISPALY_DATA
#define __DISPALY_GRAPH

//数据输出端口选择
#define DATA_OUT_USART		USART2


//*************************************

extern uint8_t dispBuf[100];
extern int SD_State;

#endif /* __INCLUDE_H */
