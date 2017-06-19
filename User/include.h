#ifndef __INCLUDE_H
#define __INCLUDE_H

#include "stm32f4xx.h"
#include <stdlib.h>

//��ʾ֡�����ݣ�Ĭ�ϲ���ʾ����Ҫ��ʾʱ�����������Ϊ1���ɣ���������΢�������󣡣�
//������Һ����ʾ����ͷ����800*480���أ�֡��Ϊ14.2֡/�롣

//���ñ������嵽��EXRAM�������ĺ�
#define __EXRAM  __attribute__ ((section ("EXRAM")))

//*************************************

//======== LCD��ʾ =================

//#define __LCD_DISPLAY

//��ʾ����
//#define __DISPALY_DATA
//#define __DISPALY_GRAPH
//#define __DISPLAY_FRAME_RATE

//======== ͼ���� =================

//#define __USART_DISPLAY
#define __NRF_DISPLAY

//��ʾ���ݣ�ֻ��ѡ��һ������
//#define __DISPLAY_IMAGE	//��ʾͼ��
//#define __DISPLAY_MATRIX	//��ʾ����
#define __DISPALY_WAVE	//��ʾ����

//======== �������ѡ�� ==============

#define DEBUG_USART			USART1
#define DATA_OUT_USART		USART2

//======== SD����ͼ ==================

//#define __SD_SAVE

//*************************************

extern uint8_t dispBuf[100];
extern int SD_State;
extern int NRF24L01_State;
extern float fps;

#endif /* __INCLUDE_H */
