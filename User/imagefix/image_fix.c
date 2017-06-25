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

uint8_t temp_array[IMG_WIDTH*IMG_HEIGHT];	//�ڶ���Ҷȿռ䣬��Ϊ������ʱ�洢�ռ�

//�洢�������ݵ��ݴ���
void To_Temp(uint16_t row,uint16_t column,uint8_t gray)
{
	//�����ķ�ʽ����ȷ����row�У���ȷ���ڱ����еĵ�column����ֵ��
	//�У�row      ��Χ��1 -- IMG_HEIGHT
	//�У�column   ��Χ��1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	temp_array[num/2] = gray;
}


//��ȡ�����ݴ�����ֵ
uint8_t Get_Temp(uint16_t row,uint16_t column)
{
	//�����ķ�ʽ����ȷ����row�У���ȷ���ڱ����еĵ�column����ֵ��
	//�У�row      ��Χ��1 -- IMG_HEIGHT
	//�У�column   ��Χ��1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	return temp_array[num/2];
}

void Image_Fix(void)	//ͼ���㷨
{
	
}

