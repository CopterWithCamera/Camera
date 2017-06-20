#ifndef __IMAGE_PROCESSING_H
#define __IMAGE_PROCESSING_H

#include "stm32f4xx.h"
#include "include.h"

#include "./systick/bsp_SysTick.h"
#include "./camera/bsp_ov5640.h"
#include "./lcd/bsp_lcd.h"

#define	IMG_WIDTH						80
#define	IMG_HEIGHT						48
#define CAMERA_BUFFER_OFFSET			IMG_WIDTH*IMG_HEIGHT*2

extern uint8_t CAMERA_BUFFER_ARRAY[2 * IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//�洢����ͷ�ɼ�����RGB565���ݺ�Ҫ��ʾ�����ݵ�����
extern uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT];	//�洢�Ҷ�ͼ��ľ���

extern uint8_t image_updata_flag;	//��ͼ��ɼ���ɱ�־  0����ͼû�вɼ����    1����ͼ�ɼ����

void Camera_Buffer_To_Lcd_Buffer(void);
void Image_Process(void);

uint8_t Get_Gray(uint16_t row,uint16_t column);				//�ӻҶ����ݴ洢��
void To_Gray(uint16_t row,uint16_t column,uint8_t gray);	//��Ҷ�ȡ��������

void Data_Output(u8 ch);

#endif /* __IMAGE_PROCESSING_H */
