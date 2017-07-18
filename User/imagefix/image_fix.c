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
	  4.float angle													对外输出参数2
	  4.float speed;												对外输出参数3（将数值赋值给变量后将自动输出）
	  5.使用__EXRAM尾缀向外部SDRAM中定义大数组    	例子：int array[1000] __EXRAM;
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
u32 counterrr = 10;
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

  /* 膨胀 */
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

  /* % 最大类间差 求二值化阈值 */
  more_value = 0.0;
  less_value = 0.0;
  more_counter = 0.0;
  less_counter = 0.0;
  flag = false;
  pre_threshold = 30.0;
  delivery_threshold = 0.0;
  
  counterrr = 10;
  
  while (!flag) {
	  counterrr --;
	  if(counterrr == 0)
		  break;
    for (i = 0; i < 46; i++) {
      for (j = 0; j < 78; j++) {
        if (b_a[(i + 48 * (j + 1)) + 1] > pre_threshold) {
          /* %与预先设置的阈值做对比 */
          more_value += b_a[(i + 48 * (j + 1)) + 1];

          /* % 求和 */
          more_counter++;

          /* % 数量+1 */
        } else {
          less_value += b_a[(i + 48 * (j + 1)) + 1];
          less_counter++;
        }
      }
    }

    delivery_threshold = (more_value / more_counter + less_value / less_counter)
      / 2.0f;

    /* %求分割后的阈值 */
    if (fabs(delivery_threshold - pre_threshold) < 0.05f) {
      /* % 与预先设置的阈值做对比 */
      flag = true;
    }

    pre_threshold = delivery_threshold;
    more_value = 0.0;
    less_value = 0.0;
    more_counter = 0.0;
    less_counter = 0.0;
  }

  /*  %% edge2 是 盛放二值化后的图像  */
  /*  %%图像二值化 */
  memset(&e[0], 0, 3840U * sizeof(float));

  /* % 利用模板寻找道路 */
  /* %权值矩阵 下面的循环是每个点的权值计算方法 */
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

  /* %存储每一个模板算出来的平均灰度值 */
  summ = 0.0;

  /* %每一个模板的灰度值总值 */
  for (cishu = 0; cishu < 51; cishu++) {
    /* % 一个48*20的模板扫过 48*80的图像需要走61次 */
    for (i = 0; i < 46; i++) {
      for (j = 0; j < 30; j++) {
        summ += e[i + 48 * (cishu + j)] * weight_matric[i + 46 * j];

        /* %对应位置的图像值乘以模板对应位置的权值 */
      }
    }

    shuzu[cishu] = summ / 920.0f;

    /* %存储平均灰度值 */
    summ = 0.0;

    /* %归0 */
  }

  /* %寻找最大灰度值 位置存储在place变量中  即起始列 */
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
  /*   imshow(edge2(:,place:place+29)); %%测试显示 */
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

void Image_Fix(void)	//图像算法
{
	tuxiang_ver1(gray_column_array,&length,&speed);
}

