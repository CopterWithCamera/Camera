#ifndef __IMAGE_PROCESSING_H
#define __IMAGE_PROCESSING_H

#include "stm32f4xx.h"
#include "include.h"

#include "./systick/bsp_SysTick.h"
#include "./camera/bsp_ov5640.h"

#define	IMG_WIDTH						80
#define	IMG_HEIGHT						48
#define CAMERA_BUFFER_OFFSET			IMG_WIDTH*IMG_HEIGHT*2

extern uint8_t CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//存储摄像头采集到的RGB565数据和要显示的数据的数组
extern uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT];	//存储灰度图像的矩阵

extern uint8_t image_updata_flag;	//新图像采集完成标志  0：新图没有采集完成    1：新图采集完成

void Camera_Buffer_To_Lcd_Buffer(void);
void Image_Process(void);

uint8_t Get_Gray(uint16_t row,uint16_t column);				//从灰度数据存储区
void To_Gray(uint16_t row,uint16_t column,uint8_t gray);	//向灰度取存入数据

void Data_Output(u8 ch);

#endif /* __IMAGE_PROCESSING_H */
