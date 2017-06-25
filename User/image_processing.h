#ifndef __IMAGE_PROCESSING_H
#define __IMAGE_PROCESSING_H

#include "stm32f4xx.h"
#include "include.h"

#include "./systick/bsp_SysTick.h"
#include "./camera/bsp_ov5640.h"

#define	IMG_WIDTH						80
#define	IMG_HEIGHT						48
#define CAMERA_BUFFER_OFFSET			IMG_WIDTH*IMG_HEIGHT*2

/* ****************** �����ṩ���ݽӿ� ************************ */

//ͼ��洢��
//extern uint8_t CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//�洢����ͷ�ɼ�����RGB565���ݺ�Ҫ��ʾ�����ݵ����飨�ṩ��ɫͼ��

extern uint8_t CAMERA_BUFFER_ARRAY1[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//����*���*2���ֽ�
extern uint8_t CAMERA_BUFFER_ARRAY2[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//����*���*2���ֽ�

extern uint8_t * CAMERA_BUFFER_ARRAY;	//��ǰ����ָ��
extern uint8_t * DCMI_IN_BUFFER_ARRAY;	//��ǰ���뻺��ָ��

extern uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//�洢�Ҷ�ͼ��ľ����ṩ�Ҷ�ͼ��
extern uint8_t result_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//����*���*1�ֽ�

//�����������
extern float length;	//ƫ��
extern float speed;

//���ݴ��亯��
uint8_t Get_Gray(uint16_t row,uint16_t column);				//��Gray����ȡ����
void To_Result(uint16_t row,uint16_t column,uint8_t gray);	//��Result����������


/* ********************* ��ϵͳ�ӿ� *************************** */

extern uint8_t image_updata_flag;	//��ͼ��ɼ���ɱ�־  0����ͼû�вɼ����    1����ͼ�ɼ����
extern uint8_t processing_ready;

void Image_Process(void);

void Data_Output_Ctrl(unsigned char cmd);	//��ȡ�����������
void Mode_Change(void);						//���������л���������
void Mode_Set(void);

#endif /* __IMAGE_PROCESSING_H */
