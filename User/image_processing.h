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
//extern uint8_t CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//存储摄像头采集到的RGB565数据和要显示的数据的数组（提供彩色图像）

extern uint8_t CAMERA_BUFFER_ARRAY1[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//长度*宽度*2个字节
extern uint8_t CAMERA_BUFFER_ARRAY2[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//长度*宽度*2个字节

extern uint8_t * CAMERA_BUFFER_ARRAY;	//当前数据指针
extern uint8_t * DCMI_IN_BUFFER_ARRAY;	//当前输入缓存指针

extern uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//存储灰度图像的矩阵（提供灰度图像）
extern uint8_t gray_column_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//灰度列向量矩阵
extern uint8_t result_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//长度*宽度*1字节
extern uint8_t result_column_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//结果列向量矩阵

//输出参数变量
extern float length;	//偏差
extern float speed;

//数据传输函数
uint8_t Get_Gray(uint16_t row,uint16_t column);				//从Gray区提取数据
void To_Result(uint16_t row,uint16_t column,uint8_t gray);	//向Result区存入数据


/* ********************* 对系统接口 *************************** */

extern uint8_t image_updata_flag;	//新图像采集完成标志  0：新图没有采集完成    1：新图采集完成
extern uint8_t processing_ready;

void Image_Process(void);

void Data_Output_Ctrl(unsigned char cmd);	//读取输入控制命令
void Mode_Change(void);						//按键控制切换传输内容
void Mode_Set(void);

#endif /* __IMAGE_PROCESSING_H */
