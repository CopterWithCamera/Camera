#ifndef __IMAGE_PROCESSING_H
#define __IMAGE_PROCESSING_H

#include "stm32f4xx.h"
#include "include.h"

#include "./systick/bsp_SysTick.h"
#include "./camera/bsp_ov5640.h"

#define	IMG_WIDTH						80
#define	IMG_HEIGHT						48
#define CAMERA_BUFFER_OFFSET			IMG_WIDTH*IMG_HEIGHT*2

/* ****************** 对外提供数据接口 ************************ */

//图像存储区
extern uint8_t CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//存储摄像头采集到的RGB565数据和要显示的数据的数组（提供彩色图像）
extern uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//存储灰度图像的矩阵（提供灰度图像）
extern uint8_t result_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//长度*宽度*1字节

//输出参数变量
extern float length;	//偏差
extern float speed;

//数据传输函数
uint8_t Get_Gray(uint16_t row,uint16_t column);				//从Gray区提取数据
void To_Result(uint16_t row,uint16_t column,uint8_t gray);	//向Result区存入数据


/* ********************* 对系统接口 *************************** */

extern uint8_t image_updata_flag;	//新图像采集完成标志  0：新图没有采集完成    1：新图采集完成
void Image_Process(void);



#endif /* __IMAGE_PROCESSING_H */
