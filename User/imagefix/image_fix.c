#include "image_fix.h"
#include "image_processing.h"
#include "math.h"

#define ABS(x) ( (x)>0?(x):-(x) )
#define fabs(x) ( (x)>0?(x):-(x) )


/*
      ���ļ�����ʹ�õ���Դ��
	  1.CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2]					��ɫͼ������
	  2.gray_array[IMG_WIDTH*IMG_HEIGHT]    						�Ҷ�ͼ������
	  3.gray_column_array[IMG_WIDTH*IMG_HEIGHT]						�Ҷ�����������
	  4.result_array[IMG_WIDTH*IMG_HEIGHT]							���ͼ������
	  5.result_column_array[IMG_WIDTH*IMG_HEIGHT]					�������������
	  3.float length;												�����������1������ֵ��ֵ���������Զ������
	  4.float speed;												�����������2������ֵ��ֵ���������Զ������
	  5.ʹ��__EXRAMβ׺���ⲿSDRAM�ж��������    	���ӣ�int array[1000] __EXRAM;
*/

void Image_Fix(void)	//ͼ���㷨
{
	
}

