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

float b_a[3840] __EXRAM;
float e[3840] __EXRAM;
short f[3840] __EXRAM;
float b_e[3840] __EXRAM;
float weight_matric[1380] __EXRAM;
void tuxiang_ver1(const unsigned char a[3840], float *quxian, float *place)
{

  int i0;
  int i;
  int j;

  short b[9];
  float more_value;
  float less_value;
  float more_counter;
  float less_counter;
  boolean_T flag;
  int b_min;
  float pre_threshold;
  int k;
  float delivery_threshold;
  float b_max;
  float shuzu[61];
  float summ;
  int cishu;
  signed char bias[46];
  float yubei;
  boolean_T exitg2;
  boolean_T exitg1;
  int high;
  int low;
  int b_j;
  float thod;
  float bias1[46];

  /* ���� */
  for (i0 = 0; i0 < 3840; i0++) {
    b_a[i0] = a[i0];
    e[i0] = 0.0;
    f[i0] = 0;
  }

  for (i = 0; i < 46; i++) {
    for (j = 0; j < 78; j++) {
      b[0] = (short)((int)b_a[i + 48 * j] - 1);
      b[1] = (short)((int)b_a[i + 48 * (j + 1)] - 2);
      b[2] = (short)((int)b_a[i + 48 * (2 + j)] - 3);
      b[3] = (short)((int)b_a[(i + 48 * j) + 1] - 4);
      b[4] = (short)((int)b_a[(i + 48 * (j + 1)) + 1] - 5);
      b[5] = (short)((int)b_a[(i + 48 * (2 + j)) + 1] - 6);
      b[6] = (short)((int)b_a[(i + 48 * j) + 2] - 7);
      b[7] = (short)((int)b_a[(i + 48 * (j + 1)) + 2] - 8);
      b[8] = (short)((int)b_a[(i + 48 * (2 + j)) + 2] - 9);
      b_min = 255;
      for (k = 0; k < 9; k++) {
        if (b[k] < b_min) {
          b_min = b[k];
        }
      }

      e[(i + 48 * (j + 1)) + 1] = b_min;
    }
  }

  for (i = 0; i < 46; i++) {
    for (j = 0; j < 78; j++) {
      b[0] = (short)(1 + (int)e[i + 48 * j]);
      b[1] = (short)(2 + (int)e[i + 48 * (j + 1)]);
      b[2] = (short)(3 + (int)e[i + 48 * (2 + j)]);
      b[3] = (short)(4 + (int)e[(i + 48 * j) + 1]);
      b[4] = (short)(5 + (int)e[(i + 48 * (j + 1)) + 1]);
      b[5] = (short)(6 + (int)e[(i + 48 * (2 + j)) + 1]);
      b[6] = (short)(7 + (int)e[(i + 48 * j) + 2]);
      b[7] = (short)(8 + (int)e[(i + 48 * (j + 1)) + 2]);
      b[8] = (short)(9 + (int)e[(i + 48 * (2 + j)) + 2]);
      b_max = 0.0;
      for (k = 0; k < 9; k++) {
        if (b[k] > (int)b_max) {
          b_max = b[k];
        }
      }

      f[(i + 48 * (j + 1)) + 1] = (short)b_max;
    }
  }

  for (i0 = 0; i0 < 3840; i0++) {
    b_e[i0] = e[i0] - (float)f[i0];
  }

  b_abs(b_e, b_a);

  /* % ������� ���ֵ����ֵ */
  more_value = 0.0;
  less_value = 0.0;
  more_counter = 0.0;
  less_counter = 0.0;
  flag = false;
  pre_threshold = 20.0;
  delivery_threshold = 0.0;
  while (!flag) {
    for (i = 0; i < 46; i++) {
      for (j = 0; j < 78; j++) {
        if (b_a[(i + 48 * (j + 1)) + 1] > pre_threshold) {
          /* %��Ԥ�����õ���ֵ���Ա� */
          more_value += b_a[(i + 48 * (j + 1)) + 1];

          /* % ��� */
          more_counter++;

          /* % ����+1 */
        } else {
          less_value += b_a[(i + 48 * (j + 1)) + 1];
          less_counter++;
        }
      }
    }

    delivery_threshold = (more_value / more_counter + less_value / less_counter)
      / 2.0f;

    /* %��ָ�����ֵ */
    if (fabs(delivery_threshold - pre_threshold) < 0.05f) {
      /* % ��Ԥ�����õ���ֵ���Ա� */
      flag = true;
    }

    pre_threshold = delivery_threshold;
    more_value = 0.0;
    less_value = 0.0;
    more_counter = 0.0;
    less_counter = 0.0;
  }

  /*  %% edge2 �� ʢ�Ŷ�ֵ�����ͼ��  */
  /*  %%ͼ���ֵ�� */
  memset(&e[0], 0, 3840U * sizeof(float));

  /* % ����ģ��Ѱ�ҵ�· */
  /* %Ȩֵ���� �����ѭ����ÿ�����Ȩֵ���㷽�� */
  for (i = 0; i < 46; i++) {
    for (j = 0; j < 78; j++) {
      if (b_a[(i + 48 * (j + 1)) + 1] < delivery_threshold) {
        e[(i + 48 * (j + 1)) + 1] = 0.0;
      } else {
        e[(i + 48 * (j + 1)) + 1] = 255.0;
      }
    }

    for (j = 0; j < 30; j++) {
      weight_matric[i + 46 * j] = 1.0f - sqrt((24.5f - (1.0f + (float)i)) * (24.5f
        - (1.0f + (float)i)) + (15.5f - (1.0f + (float)j)) * (15.5f - (1.0f +
        (float)j))) / 25.93260495977988f;
    }
  }

  memset(&shuzu[0], 0, 61U * sizeof(float));

  /* %�洢ÿһ��ģ���������ƽ���Ҷ�ֵ */
  summ = 0.0;

  /* %ÿһ��ģ��ĻҶ�ֵ��ֵ */
  for (cishu = 0; cishu < 51; cishu++) {
    /* % һ��48*20��ģ��ɨ�� 48*80��ͼ����Ҫ��61�� */
    for (i = 0; i < 46; i++) {
      for (j = 0; j < 30; j++) {
        summ += e[i + 48 * (cishu + j)] * weight_matric[i + 46 * j];

        /* %��Ӧλ�õ�ͼ��ֵ����ģ���Ӧλ�õ�Ȩֵ */
      }
    }

    shuzu[cishu] = summ / 920.0f;

    /* %�洢ƽ���Ҷ�ֵ */
    summ = 0.0;

    /* %��0 */
  }

  /* %Ѱ�����Ҷ�ֵ λ�ô洢��place������  ����ʼ�� */
  if (shuzu[0] > shuzu[1]) {
    b_max = shuzu[0];
    *place = 1.0;
  } else {
    b_max = shuzu[1];
    *place = 2.0;
  }

  for (i = 0; i < 58; i++) {
    if (b_max < shuzu[i + 2]) {
      *place = 3.0f + (float)i;
      b_max = shuzu[3 + i];
    }
  }

  /*   figure; */
  /*   imshow(edge2(:,place:place+29)); %%������ʾ */
  for (i = 0; i < 46; i++) {
    bias[i] = 0;
  }

  if (*place > 30.0f) {
    for (i = 0; i < 45; i++) {
      i0 = (int)(((float)*place + 29.0F) + (1.0F - (float)*place));
      j = 0;
      exitg2 = false;
      while ((!exitg2) && (j <= i0 - 1)) {
        b_j = (int)*place + j;
        if (e[(48 * (b_j - 1) - i) + 45] - e[(48 * (b_j - 2) - i) + 45] == 255.0f)
        {
          bias[44 - i] = (signed char)b_j;
          exitg2 = true;
        } else {
          j++;
        }
      }
    }
  } else if (*place < 25.0f) {
    for (i = 0; i < 45; i++) {
      i0 = (int)-((float)*place + (-1.0F - ((float)*place + 29.0F)));
      j = 0;
      exitg1 = false;
      while ((!exitg1) && (j <= i0 - 1)) {
        b_j = ((int)*place - j) + 29;
        if (e[(48 * (b_j - 1) - i) + 45] - e[(48 * b_j - i) + 45] == 255.0f) {
          bias[44 - i] = (signed char)b_j;
          exitg1 = true;
        } else {
          j++;
        }
      }
    }
  } else {
    for (i = 0; i < 46; i++) {
      bias[i] = 0;
    }
  }

  yubei = 0.0;
  if (bias[45] > bias[44]) {
    high = bias[45];
    low = bias[44];
  } else {
    high = bias[44];
    low = bias[45];
  }

  thod = (float)(high - low) / 2.0f;
  for (i = 0; i < 46; i++) {
    bias1[i] = bias[i];
  }

  for (i = 0; i < 44; i++) {
    if (fabs(bias[45 - i] - bias[44 - i]) > thod) {
      if (fabs(bias[44 - i] - bias[43 - i]) > thod) {
        bias1[44 - i] = bias[45 - i];
      } else {
        bias1[44 - i] = (float)(bias[45 - i] + bias[43 - i]) / 2.0f;
      }
    }
  }

  if (fabs(bias[0] - bias[1]) > thod) {
    bias1[0] = bias[1];
  }

  for (i = 0; i < 46; i++) {
    if (bias1[45 - i] == 0.0f) {
      yubei++;
    }
  }

  *quxian = sum(bias1) / (46.0f - yubei) - 40.0f;
  if ((*place >= 25.0f) && (*place <= 30.0f)) {
    *quxian = 0.0;
  }
}

float tmp;
void Image_Fix(void)	//ͼ���㷨
{
	tuxiang_ver1(gray_column_array,&length,&tmp);
}

