#include "image_fix.h"
#include "image_processing.h"
#include <math.h>
#include <string.h>

#define false	0
#define true	1
	
#define ABS(x) ( (x)>0?(x):-(x) )
#define fabs(x) ( (x)>0?(x):-(x) )

typedef u8 boolean_T;


/*
      ���ļ�����ʹ�õ���Դ��
	  1.CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2]					��ɫͼ������
	  2.gray_array[IMG_WIDTH*IMG_HEIGHT]    						�Ҷ�ͼ������
	  3.gray_column_array[IMG_WIDTH*IMG_HEIGHT]						�Ҷ�����������
	  4.result_array[IMG_WIDTH*IMG_HEIGHT]							���ͼ������
	  5.result_column_array[IMG_WIDTH*IMG_HEIGHT]					�������������
	  3.float length;												�����������1������ֵ��ֵ���������Զ������
	  4.float angle													�����������2
	  4.float speed;												�����������3������ֵ��ֵ���������Զ������
	  5.ʹ�� __EXRAM β׺���ⲿSDRAM�ж��������    	���ӣ�int array[1000] __EXRAM;
*/

void b_abs(const float x[3840], float y[3840])
{
  int k;
  for (k = 0; k < 3840; k++) {
    y[k] = fabs(x[k]);
  }
}

float sum(const float x[46])
{
  float y;
  int k;
  y = x[0];
  for (k = 0; k < 45; k++) {
    y += x[k + 1];
  }

  return y;
}

static unsigned char a[3840] __EXRAM;
short pengzhang[3840] __EXRAM;
void tophat_7_18(const unsigned char gray[3840], float edge1[3840])
{
  int b_max;
  int i;
  int j;
  short b[9];
  int k;
  for (b_max = 0; b_max < 3840; b_max++) {
    a[b_max] = gray[b_max];
    edge1[b_max] = 0.0f;
    pengzhang[b_max] = 0;
  }

  /* ���� */
  for (i = 0; i < 46; i++) {
    for (j = 0; j < 78; j++) {
      b[0] = (short)(a[i + 48 * j] - 1);
      b[1] = (short)(a[i + 48 * (j + 1)] - 2);
      b[2] = (short)(a[i + 48 * (j + 2)] - 3);
      b[3] = (short)(a[(i + 48 * j) + 1] - 4);
      b[4] = (short)(a[(i + 48 * (j + 1)) + 1] - 5);
      b[5] = (short)(a[(i + 48 * (j + 2)) + 1] - 6);
      b[6] = (short)(a[(i + 48 * j) + 2] - 7);
      b[7] = (short)(a[(i + 48 * (j + 1)) + 2] - 8);
      b[8] = (short)(a[(i + 48 * (j + 2)) + 2] - 9);
      b_max = 255;
      for (k = 0; k < 9; k++) {
        if (b[k] < b_max) {
          b_max = b[k];
        }
      }

      pengzhang[(i + 48 * (j + 1)) + 1] = (short)b_max;
    }
  }

  for (i = 0; i < 46; i++) {
    for (j = 0; j < 78; j++) {
      b[0] = (short)(1 + pengzhang[i + 48 * j]);
      b[1] = (short)(2 + pengzhang[i + 48 * (j + 1)]);
      b[2] = (short)(3 + pengzhang[i + 48 * (j + 2)]);
      b[3] = (short)(4 + pengzhang[(i + 48 * j) + 1]);
      b[4] = (short)(5 + pengzhang[(i + 48 * (j + 1)) + 1]);
      b[5] = (short)(6 + pengzhang[(i + 48 * (j + 2)) + 1]);
      b[6] = (short)(7 + pengzhang[(i + 48 * j) + 2]);
      b[7] = (short)(8 + pengzhang[(i + 48 * (j + 1)) + 2]);
      b[8] = (short)(9 + pengzhang[(i + 48 * (j + 2)) + 2]);
      b_max = 0;
      for (k = 0; k < 9; k++) {
        if (b[k] > b_max) {
          b_max = b[k];
        }
      }

      edge1[(i + 48 * (j + 1)) + 1] = fabs((float)(pengzhang[(i + 48 * (j + 1))
        + 1] - b_max));

      /* f(i,j)=max; */
    }
  }
}

float tmp_array[3840];
void Image_Fix(void)	//ͼ���㷨
{
	tophat_7_18(gray_column_array,tmp_array);
}

