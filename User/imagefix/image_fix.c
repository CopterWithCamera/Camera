#include "image_fix.h"
#include "image_processing.h"

#define ABS(x) ( (x)>0?(x):-(x) )


/*
      ���ļ�����ʹ�õ���Դ��
	  1.CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2]					��ɫͼ������
	  2.gray_array[IMG_WIDTH*IMG_HEIGHT];     						�Ҷ�ͼ������
	  3.uint8_t Get_Gray(uint16_t row,uint16_t column);				��Gray����ȡ����
      4.void To_Result(uint16_t row,uint16_t column,uint8_t gray);	��Resultȡ��������
	  5.float length;												�����������1������ֵ��ֵ���������Զ������
	  6.float speed;												�����������2������ֵ��ֵ���������Զ������
	  7.ʹ��__EXRAMβ׺���ⲿSDRAM�ж��������    	���ӣ�int array[1000] __EXRAM;
*/

void Image_Fix(void)	//ͼ���㷨
{
	
}

