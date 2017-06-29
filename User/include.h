#ifndef __INCLUDE_H
#define __INCLUDE_H

#include "stm32f4xx.h"
#include <stdlib.h>
#include <stdio.h>

//��ʾ֡�����ݣ�Ĭ�ϲ���ʾ����Ҫ��ʾʱ�����������Ϊ1���ɣ���������΢�������󣡣�
//������Һ����ʾ����ͷ����800*480���أ�֡��Ϊ14.2֡/�롣

//���ñ������嵽��EXRAM�������ĺ�
#define __EXRAM  __attribute__ ((section ("EXRAM")))

//*************************************

//======== ͼ���� =================

//#define __USART_DISPLAY
#define __NRF_DISPLAY

//��ʾ���ݣ�
#define __DISPLAY_IMAGE		//��ʾͼ��
#define __DISPLAY_RESULT	//������
#define __DISPALY_WAVE		//��ʾ����
#define __PARAMETER_FPS		//����FPS 
#define __PARAMETER_MODE	//����MODE

//======== �������ѡ�� ==============

#define DEBUG_USART			USART1
#define DATA_OUT_USART		USART2

//======== SD����ͼ ==================

#define __SD_SAVE

//*************************************

extern int SD_State;
extern int NRF24L01_State;
extern float fps;
extern float processing_fps;

extern uint8_t fps_temp;
extern uint8_t processing_fps_temp;

#endif /* __INCLUDE_H */
