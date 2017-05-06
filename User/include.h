#ifndef __INCLUDE_H
#define __INCLUDE_H

#include "stm32f4xx.h"
#include <stdlib.h>

//��ʾ֡�����ݣ�Ĭ�ϲ���ʾ����Ҫ��ʾʱ�����������Ϊ1���ɣ���������΢�������󣡣�
//������Һ����ʾ����ͷ����800*480���أ�֡��Ϊ14.2֡/�롣

//���ñ������嵽��EXRAM�������ĺ�
#define __EXRAM  __attribute__ ((section ("EXRAM")))


//************ ���ܿ��� ***************

//֡����ʾ
#define FRAME_RATE_DISPLAY 	1

//LCD��ʾ
#define LCD_DISPLAY


//*************************************

extern uint8_t dispBuf[100];
extern int SD_State;

#endif /* __INCLUDE_H */
