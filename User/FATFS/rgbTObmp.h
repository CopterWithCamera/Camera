#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "image_processing.h"
#include "ff.h"

void rgb_TO_bmp(void);
void TO_SDcard(u8 mode);

//����д��ͬһ�ļ���ͼ��
extern u8 ToOneFile_StartFlag;
extern FIL ToOneFile_f;				//�ļ�ָ��
void TO_SDcard_OneFile(u8 mode);	//����д��ͬһ�ļ�ģʽ�ĺ���

//����д��ͬһ�ļ����߶ȣ�
extern u8 Height_StartFlag;			//��ʼ��ʱд��FLAG
extern FIL Height_f;				//�ļ�ָ��
void TO_SDcard_Height(void);		//��SD����д��txt�����ļ�



