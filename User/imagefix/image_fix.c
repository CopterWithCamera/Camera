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
      本文件可以使用的资源：
	  1.CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2]					彩色图像数组
	  2.gray_array[IMG_WIDTH*IMG_HEIGHT]    						灰度图像数组
	  3.gray_column_array[IMG_WIDTH*IMG_HEIGHT]						灰度列向量数组
	  4.result_array[IMG_WIDTH*IMG_HEIGHT]							结果图像数组
	  5.result_column_array[IMG_WIDTH*IMG_HEIGHT]					结果列向量数组
	  3.float length;												对外输出参数1（将数值赋值给变量后将自动输出）
	  4.float length_pitch;											对外输出参数2
	  4.float angle;												对外输出参数3（将数值赋值给变量后将自动输出）
	  5.使用 __EXRAM 尾缀向外部SDRAM中定义大数组    	例子：int array[1000] __EXRAM;
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
  /* % 读图做灰度处理并且转化为浮点类型 */
  /*  a=imread('532.bmp'); */
  /*  a=rgb2gray(a); */
  /*  %% 膨胀-腐蚀-边缘检测 */
  /* 膨胀 */
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
  /*  title('边缘') */
  /*     %% 迭代法 求二值化阈值 */
  /* 迭代法判断动态阈值 */
  /* 只能迭代十次（cicic==0） */
  /* test_flag 设置为测试场地全白的标志位，test_flag=0为正常，=200为出界，每次test_flag预先置0 */
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
          /* %与预先设置的阈值做对比 */
          more_value += (float)edge1[(i + 48 * (j + 1)) + 1];

          /* % 求和 */
          more_counter++;

          /* % 数量+1 */
        } else {
          less_value += (float)edge1[(i + 48 * (j + 1)) + 1];
          less_counter++;
        }
      }
    }

    /* 判断test_flag标志位环节 */
    if (less_counter > 3553.0f) {
      test_flag = 200;
      exitg3 = true;
    } else {
      delivery_threshold = (more_value / more_counter + less_value /
                            less_counter) / 2.0f;

      /* %求分割后的阈值 */
      if (fabs(delivery_threshold - pre_threshold) < 0.05f) {
        /* % 与预先设置的阈值做对比 */
        flag = true;
      }

      pre_threshold = delivery_threshold;

      /* 判断次数环节 */
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

  /*     %% edge2 是 盛放二值化后的图像 */
  /*     %%图像二值化 */
  for (i = 0; i < 46; i++) {
    for (j = 0; j < 78; j++) {
      if (edge1[(i + 48 * (j + 1)) + 1] <= delivery_threshold) {
        edge1[(i + 48 * (j + 1)) + 1] = 0;
      } else {
        edge1[(i + 48 * (j + 1)) + 1] = 255;
      }
    }
  }

  /*     %%首先判断test_flag是否为0，假如为0 计算偏差和角度，为200则判断左出界或者右出界 */
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

    /* 进行误差大数调整（误差较大的记为横线，为0剔除，剩下的为竖线） */
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

    /* 计算直线偏移（信任是直线的点的平均-中点） */
    yubei = 0.0;
    for (i = 0; i < 47; i++) {
      if ((signed char)bias[i] == 0) {
        yubei++;
      }
    }

    *quxian = sum(bias) / (47.0f - yubei) - 40.0f;

    /* 设定偏移死区 */
    if (fabs(*quxian) < 1.0f) {
      *quxian = 0.0;
    }

    b_last_quxian = *quxian;

    /* 计算角度，将置信任的竖线坐标进行最小二乘法的直线拟合，利用斜率计算偏角 */
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

    /* 负为飞机头左转，正为飞机头右转 */
    /* 设定+-2°区间为死区 */
    if (fabs(*angle) <= 2.0f) {
      *angle = 0.0f;
    }

    /* 假如置0点多余30，认为只有标志线的边缘，直接开始判断 */
    if (yubei > 30.0f) {
      if (*quxian > 20.0f) {
        *quxian = -100.0;

        /* 从右边出去了 */
      } else {
        if (*quxian < -20.0f) {
          *quxian = 100.0;

          /* 从左边出去了 */
        }
      }
    }

    /*  假如置0点大于5 认为有横线，进入横线的偏差计算模式，输出为row_bias */
    if (b_last_quxian > 20.0f) {
      *quxian = -100.0f;

      /* 从右边出去了 */
    } else {
      if (b_last_quxian < -20.0f) {
        *quxian = 100.0f;

        /* 从左边出去了 */
      }
    }
  }
}

float tmp = 0.0f;
void Image_Fix(void)	//图像算法
{
//	tuxiang_ver1(gray_column_array,&length,&tmp);
//	test_simple(gray_column_array,tmp,&length,&angle);
//	tmp = length;
}

