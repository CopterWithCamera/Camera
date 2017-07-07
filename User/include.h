#ifndef __INCLUDE_H
#define __INCLUDE_H

#include "stm32f4xx.h"
#include <stdlib.h>
#include <stdio.h>

//显示帧率数据，默认不显示，需要显示时把这个宏设置为1即可，（会有轻微花屏现象！）
//经测试液晶显示摄像头数据800*480像素，帧率为14.2帧/秒。

//设置变量定义到“EXRAM”节区的宏
#define __EXRAM  __attribute__ ((section ("EXRAM")))

//*************************************

//======== 图像传输 =================

//#define __USART_DISPLAY
#define __NRF_DISPLAY

//显示内容：
#define __DISPLAY_IMAGE		//发送灰度图像
#define __DISPLAY_RESULT	//发送运算结果图像
#define __DISPALY_WAVE		//发送波形
#define __PARAMETER_FPS		//发送FPS
#define __PARAMETER_MODE	//发送MODE参数

//======== 输出串口选择 ==============

#define DEBUG_USART			USART1
#define DATA_OUT_USART		USART2

//======== SD卡存图 ==================

#define __SD_SAVE

//存储内容：

#define __SD_SAVE_ORIGINAL	//保存原始彩色图
#define __SD_SAVE_GRAY		//保存灰度矩阵生成的图像
#define __SD_SAVE_RESULT	//保存运算结果矩阵生成的图像

//存储模式

#define __SD_SAVE_MODE 		1		//0 -- 按照图片格式保存
									//1 -- 持续写入同一文件

//*************************************

extern int SD_State;
extern int NRF24L01_State;

extern float fps;
extern float processing_fps;

extern uint8_t fps_temp;
extern uint8_t processing_fps_temp;

#endif /* __INCLUDE_H */
