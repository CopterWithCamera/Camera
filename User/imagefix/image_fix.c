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
	  4.float length_pitch;											�����������2
	  4.float angle;												�����������3������ֵ��ֵ���������Զ������
	  5.float tracking_state										����״̬ 0��ʧ��  1���ɹ�
	  6.ʹ�� __EXRAM β׺���ⲿSDRAM�ж��������    	���ӣ�int array[1000] __EXRAM;
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

void test_simple(const unsigned char a[3840], float last_quxian, float *quxian,
                 float *angle)
{
  unsigned char b_a[3840];
  short e[3840];
  short edge1[3840];
  int i0;
  int i;
  int j;
  short b[9];
  float more_value;
  float less_value;
  float more_counter;
  int b_min;
  float less_counter;
  int k;
  boolean_T flag;
  int b_max;
  float pre_threshold;
  float delivery_threshold;
  float cicici;
  int test_flag;
  boolean_T exitg3;
  float bias[80];
  float bias1[80];
  float yubei;
  boolean_T exitg2;
  float bias_aver;
  boolean_T exitg1;
  float biaozhuncha;
  float c_a;
  float b_last_quxian;
  float sum_X;
  float sum_Y;
  float sum_XY;
  float sum_Xsquare;
  (void)last_quxian;
  *quxian = 0.0;
  *angle = 0.0;

  /*  clear all; */
  /* % ��ͼ���Ҷȴ�����ת��Ϊ�������� */
  /*  a=imread('532.bmp'); */
  /*  a=rgb2gray(a); */
  /*  %% ����-��ʴ-��Ե��� */
  /* ���� */
  for (i0 = 0; i0 < 3840; i0++) {
    b_a[i0] = a[i0];
    e[i0] = 0;
    edge1[i0] = 0;
  }

  for (i = 0; i < 46; i++) {
    for (j = 0; j < 78; j++) {
      b[0] = (short)(b_a[i + 48 * j] - 1);
      b[1] = (short)(b_a[i + 48 * (j + 1)] - 2);
      b[2] = (short)(b_a[i + 48 * (2 + j)] - 3);
      b[3] = (short)(b_a[(i + 48 * j) + 1] - 4);
      b[4] = (short)(b_a[(i + 48 * (j + 1)) + 1] - 5);
      b[5] = (short)(b_a[(i + 48 * (2 + j)) + 1] - 6);
      b[6] = (short)(b_a[(i + 48 * j) + 2] - 7);
      b[7] = (short)(b_a[(i + 48 * (j + 1)) + 2] - 8);
      b[8] = (short)(b_a[(i + 48 * (2 + j)) + 2] - 9);
      b_min = 255;
      for (k = 0; k < 9; k++) {
        if (b[k] < b_min) {
          b_min = b[k];
        }
      }

      e[(i + 48 * (j + 1)) + 1] = (short)b_min;
    }
  }

  for (i = 0; i < 46; i++) {
    for (j = 0; j < 78; j++) {
      b[0] = (short)(1 + e[i + 48 * j]);
      b[1] = (short)(2 + e[i + 48 * (j + 1)]);
      b[2] = (short)(3 + e[i + 48 * (2 + j)]);
      b[3] = (short)(4 + e[(i + 48 * j) + 1]);
      b[4] = (short)(5 + e[(i + 48 * (j + 1)) + 1]);
      b[5] = (short)(6 + e[(i + 48 * (2 + j)) + 1]);
      b[6] = (short)(7 + e[(i + 48 * j) + 2]);
      b[7] = (short)(8 + e[(i + 48 * (j + 1)) + 2]);
      b[8] = (short)(9 + e[(i + 48 * (2 + j)) + 2]);
      b_max = 0;
      for (k = 0; k < 9; k++) {
        if (b[k] > b_max) {
          b_max = b[k];
        }
      }

      edge1[(i + 48 * (j + 1)) + 1] = (short)fabs(b_max - e[(i + 48 * (j + 1)) +
        1]);
    }
  }

  /*  figure; */
  /*  imshow(edge1); */
  /*  title('��Ե') */
  /*     %% ������ ���ֵ����ֵ */
  /* �������ж϶�̬��ֵ */
  /* ֻ�ܵ���ʮ�Σ�cicic==0�� */
  /* test_flag ����Ϊ���Գ���ȫ�׵ı�־λ��test_flag=0Ϊ������=200Ϊ���磬ÿ��test_flagԤ����0 */
  more_value = 0.0;
  less_value = 0.0;
  more_counter = 0.0;
  less_counter = 0.0;
  flag = false;
  pre_threshold = 20.0;
  delivery_threshold = 0.0;
  cicici = 0.0;
  test_flag = 0;
  exitg3 = false;
  while ((!exitg3) && (!flag)) {
    for (i = 0; i < 46; i++) {
      for (j = 0; j < 78; j++) {
        if (edge1[(i + 48 * (j + 1)) + 1] > pre_threshold) {
          /* %��Ԥ�����õ���ֵ���Ա� */
          more_value += (float)edge1[(i + 48 * (j + 1)) + 1];

          /* % ��� */
          more_counter++;

          /* % ����+1 */
        } else {
          less_value += (float)edge1[(i + 48 * (j + 1)) + 1];
          less_counter++;
        }
      }
    }

    /* �ж�test_flag��־λ���� */
    if (less_counter > 3553.0f) {
      test_flag = 200;
      exitg3 = true;
    } else {
      delivery_threshold = (more_value / more_counter + less_value /
                            less_counter) / 2.0f;

      /* %��ָ�����ֵ */
      if (fabs(delivery_threshold - pre_threshold) < 0.05f) {
        /* % ��Ԥ�����õ���ֵ���Ա� */
        flag = true;
      }

      pre_threshold = delivery_threshold;

      /* �жϴ������� */
      cicici++;
      if (cicici == 5.0f) {
        exitg3 = true;
      } else {
        more_value = 0.0;
        less_value = 0.0;
        more_counter = 0.0;
        less_counter = 0.0;
      }
    }
  }

  /*     %% edge2 �� ʢ�Ŷ�ֵ�����ͼ�� */
  /*     %%ͼ���ֵ�� */
  for (i = 0; i < 46; i++) {
    for (j = 0; j < 78; j++) {
      if (edge1[(i + 48 * (j + 1)) + 1] <= delivery_threshold) {
        edge1[(i + 48 * (j + 1)) + 1] = 0;
      } else {
        edge1[(i + 48 * (j + 1)) + 1] = 255;
      }
    }
  }

  /*     %%�����ж�test_flag�Ƿ�Ϊ0������Ϊ0 ����ƫ��ͽǶȣ�Ϊ200���ж����������ҳ��� */
  if (test_flag == 0) {
    memset(&bias[0], 0, 80U * sizeof(float));
    memset(&bias1[0], 0, 80U * sizeof(float));
    yubei = 0.0;
    for (i = 0; i < 46; i++) {
      j = 0;
      exitg2 = false;
      while ((!exitg2) && (j < 78)) {
        if (edge1[(48 * (j + 1) - i) + 46] - edge1[(48 * (2 + j) - i) + 46] ==
            255) {
          bias[46 - i] = 2.0f + (float)j;
          exitg2 = true;
        } else {
          j++;
        }
      }

      j = 0;
      exitg1 = false;
      while ((!exitg1) && (j < 78)) {
        if (edge1[(48 * (78 - j) - i) + 46] - edge1[(48 * (77 - j) - i) + 46] ==
            255) {
          bias1[46 - i] = 79.0f + -(float)j;
          exitg1 = true;
        } else {
          j++;
        }
      }
    }

    if (fabs(sum(bias) - 1863.0f) > fabs(sum(bias1) - 1863.0f)) {
      memcpy(&bias[0], &bias1[0], 80U * sizeof(float));
    }

    for (i = 0; i < 47; i++) {
      if ((signed char)bias[i] == 0) {
        yubei++;
      }
    }

    /* �������������������ϴ�ļ�Ϊ���ߣ�Ϊ0�޳���ʣ�µ�Ϊ���ߣ� */
    bias_aver = sum(bias) / (46.0f - yubei);
    biaozhuncha = 0.0;
    for (i = 0; i < 46; i++) {
      c_a = (float)(signed char)bias[i + 1] - bias_aver;
      biaozhuncha += c_a * c_a;
    }

    biaozhuncha = sqrt(biaozhuncha / (47.0f - yubei));
    for (i = 0; i < 46; i++) {
      if (fabs((float)(signed char)bias[i + 1] - bias_aver) > biaozhuncha) {
        bias[i + 1] = 0.0;
      }
    }

    /* ����ֱ��ƫ�ƣ�������ֱ�ߵĵ��ƽ��-�е㣩 */
    yubei = 0.0;
    for (i = 0; i < 47; i++) {
      if ((signed char)bias[i] == 0) {
        yubei++;
      }
    }

    *quxian = sum(bias) / (47.0f - yubei) - 40.0f;

    /* �趨ƫ������ */
    if (fabs(*quxian) < 1.0f) {
      *quxian = 0.0;
    }

    b_last_quxian = *quxian;

    /* ����Ƕȣ��������ε��������������С���˷���ֱ����ϣ�����б�ʼ���ƫ�� */
    sum_X = 0.0;
    sum_Y = 0.0;
    sum_XY = 0.0;
    sum_Xsquare = 0.0;
    for (i = 0; i < 47; i++) {
      if ((signed char)bias[i] != 0) {
        sum_X += 1.0f + (float)i;
        sum_Y += (float)(signed char)bias[i];
        sum_XY += (float)((1 + i) * (signed char)bias[i]);
        sum_Xsquare += (float)((1 + i) * (1 + i));
      }
    }

    sum_X /= 47.0f - yubei;
    sum_Y /= 47.0f - yubei;
    sum_XY /= 47.0f - yubei;
    sum_Xsquare /= 47.0f - yubei;
    *angle = atan((sum_XY - sum_X * sum_Y) / (sum_Xsquare - sum_X * sum_X)) *
      180.0 / 3.1415926535897931;

    /* ��Ϊ�ɻ�ͷ��ת����Ϊ�ɻ�ͷ��ת */
    /* �趨+-2������Ϊ���� */
    if (fabs(*angle) <= 2.0f) {
      *angle = 0.0f;
    }

    /* ������0�����30����Ϊֻ�б�־�ߵı�Ե��ֱ�ӿ�ʼ�ж� */
    if (yubei > 30.0f) {
      if (*quxian > 20.0f) {
        *quxian = -100.0;

        /* ���ұ߳�ȥ�� */
      } else {
        if (*quxian < -20.0f) {
          *quxian = 100.0;

          /* ����߳�ȥ�� */
        }
      }
    }

    /*  ������0�����5 ��Ϊ�к��ߣ�������ߵ�ƫ�����ģʽ�����Ϊrow_bias */
    if (b_last_quxian > 20.0f) {
      *quxian = -100.0f;

      /* ���ұ߳�ȥ�� */
    } else {
      if (b_last_quxian < -20.0f) {
        *quxian = 100.0f;

        /* ����߳�ȥ�� */
      }
    }
  }
}

float tmp = 0.0f;
void Image_Fix(void)	//ͼ���㷨
{
//	tuxiang_ver1(gray_column_array,&length,&tmp);
//	test_simple(gray_column_array,tmp,&length,&angle);
//	tmp = length;
}

