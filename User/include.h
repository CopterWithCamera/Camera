#ifndef __INCLUDE_H
#define __INCLUDE_H

#include "stm32f4xx.h"
#include <stdlib.h>

//��ʾ֡�����ݣ�Ĭ�ϲ���ʾ����Ҫ��ʾʱ�����������Ϊ1���ɣ���������΢�������󣡣�
//������Һ����ʾ����ͷ����800*480���أ�֡��Ϊ14.2֡/�롣

//���ñ������嵽��EXRAM�������ĺ�
#define __EXRAM  __attribute__ ((section ("EXRAM")))


//�������ѡ��
#define DEBUG_USART			USART1
#define DATA_OUT_USART		USART2


//************ ���ܿ��� ***************

//֡����ʾ
#define FRAME_RATE_DISPLAY 		1

//============================
//LCD��ʾ
//#define __LCD_DISPLAY

//��ʾ����
//#define __DISPALY_DATA
//#define __DISPALY_GRAPH

//============================
//ͼ����

//#define __USART_DISPLAY
#define __NRF_DISPLAY

//��ʾ���ݣ�ֻ��ѡ��һ������
#define __DISPLAY_IMAGE	//��ʾͼ��
//#define __DISPLAY_MATRIX	//��ʾ����
//#define __DISPALY_WAVE	//��ʾ����

//*************************************

extern uint8_t dispBuf[100];
extern int SD_State;
extern float fps;

#endif /* __INCLUDE_H */
