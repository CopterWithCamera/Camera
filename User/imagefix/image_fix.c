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
      ±¾ÎÄ¼ş¿ÉÒÔÊ¹ÓÃµÄ×ÊÔ´£º
	  1.CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2]					²ÊÉ«Í¼ÏñÊı×é
	  2.gray_array[IMG_WIDTH*IMG_HEIGHT]    						»Ò¶ÈÍ¼ÏñÊı×é
	  3.gray_column_array[IMG_WIDTH*IMG_HEIGHT]						»Ò¶ÈÁĞÏòÁ¿Êı×é
	  4.result_array[IMG_WIDTH*IMG_HEIGHT]							½á¹ûÍ¼ÏñÊı×é
	  5.result_column_array[IMG_WIDTH*IMG_HEIGHT]					½á¹ûÁĞÏòÁ¿Êı×é
	  3.float length;												¶ÔÍâÊä³ö²ÎÊı1£¨½«ÊıÖµ¸³Öµ¸ø±äÁ¿ºó½«×Ô¶¯Êä³ö£©
	  4.float length_pitch;											¶ÔÍâÊä³ö²ÎÊı2
	  4.float angle;												¶ÔÍâÊä³ö²ÎÊı3£¨½«ÊıÖµ¸³Öµ¸ø±äÁ¿ºó½«×Ô¶¯Êä³ö£©
	  5.Ê¹ÓÃ __EXRAM Î²×ºÏòÍâ²¿SDRAMÖĞ¶¨Òå´óÊı×é    	Àı×Ó£ºint array[1000] __EXRAM;
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

/******µÚÈı°æ********/
  unsigned char b_a[3840] __EXRAM;
  short e[3840] __EXRAM;
  short edge1[3840] __EXRAM;
  float bias_array[80] __EXRAM;
  float bias_array1[80] __EXRAM;
 void test_simple(const unsigned char a[3840], float In_last_bias, float
                 In_last_angle, float In_last_row_bias, float *Out_bias,
                 float *Out_angle, float *Out_row_bias, float *Out_last_bias,
                 float *Out_last_angle, float *Out_last_row_bias)
{

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
  boolean_T exitg4;

  float yubei;
  boolean_T exitg3;
  boolean_T exitg2;
  float bias_aver;
  float biaozhuncha;
  float x;
  float bias;
  float sum_X;
  float sum_Y;
  float sum_XY;
  float sum_Xsquare;
  boolean_T exitg1;
  float yubei1;
  (void)In_last_bias;

  /*  clear all; */
  /* % ???????????????? */
  /*  a=imread('74.bmp'); */
  /*  a=rgb2gray(a); */
  /* % ??-??-???? */
  /* ?? */
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
  /*  title('??') */
  /* % ??? ?????? */
  /* ????????? */
  /* ??????(cicic==0) */
  /* test_flag ?????????????,test_flag=0???,=200???,??test_flag???0 */
  more_value = 0.0f;
  less_value = 0.0f;
  more_counter = 0.0f;
  less_counter = 0.0f;
  flag = false;
  pre_threshold = 20.0f;
  delivery_threshold = 0.0f;
  cicici = 0.0f;
  test_flag = 0;
  exitg4 = false;
  while ((!exitg4) && (!flag)) {
    for (i = 0; i < 46; i++) {
      for (j = 0; j < 78; j++) {
        if (edge1[(i + 48 * (j + 1)) + 1] > pre_threshold) {
          /* %??????????? */
          more_value += (float)edge1[(i + 48 * (j + 1)) + 1];

          /* % ?? */
          more_counter++;

          /* % ??+1 */
        } else {
          less_value += (float)edge1[(i + 48 * (j + 1)) + 1];
          less_counter++;
        }
      }
    }

    /* ??test_flag????? */
    if (less_counter > 3553.0f) {
      test_flag = 200;
      exitg4 = true;
    } else {
      delivery_threshold = (more_value / more_counter + less_value /
                            less_counter) / 2.0f;

      /* %??????? */
      if (fabs(delivery_threshold - pre_threshold) < 0.05f) {
        /* % ??????????? */
        flag = true;
      }

      pre_threshold = delivery_threshold;

      /* ?????? */
      cicici++;
      if (cicici == 5.0f) {
        exitg4 = true;
      } else {
        more_value = 0.0f;
        less_value = 0.0f;
        more_counter = 0.0f;
        less_counter = 0.0f;
      }
    }
  }

  /* % edge2 ? ????????? */
  /* %????? */
  for (i = 0; i < 46; i++) {
    for (j = 0; j < 78; j++) {
      if (edge1[(i + 48 * (j + 1)) + 1] <= delivery_threshold) {
        edge1[(i + 48 * (j + 1)) + 1] = 0;
      } else {
        edge1[(i + 48 * (j + 1)) + 1] = 255;
      }
    }
  }

  /* %????test_flag???0,???0 ???????,?200??????????? */
  if (test_flag == 0) {
    memset(&bias_array[0], 0, 80U * sizeof(float));
    memset(&bias_array1[0], 0, 80U * sizeof(float));
    yubei = 0.0f;
    for (i = 0; i < 46; i++) {
      j = 0;
      exitg3 = false;
      while ((!exitg3) && (j < 78)) {
        if (edge1[(48 * (j + 1) - i) + 46] - edge1[(48 * (2 + j) - i) + 46] ==
            255) {
          bias_array[46 - i] = 2.0f + (float)j;
          exitg3 = true;
        } else {
          j++;
        }
      }

      j = 0;
      exitg2 = false;
      while ((!exitg2) && (j < 78)) {
        if (edge1[(48 * (78 - j) - i) + 46] - edge1[(48 * (77 - j) - i) + 46] ==
            255) {
          bias_array1[46 - i] = 79.0f + -(float)j;
          exitg2 = true;
        } else {
          j++;
        }
      }
    }

    if (fabs(sum(bias_array) - 1903.5f) > fabs(sum(bias_array1) - 1903.5f)) {
      memcpy(&bias_array[0], &bias_array1[0], 80U * sizeof(float));
    } else {
      memcpy(&bias_array1[0], &bias_array[0], 80U * sizeof(float));
    }

    for (i = 0; i < 47; i++) {
      if ((signed char)bias_array[i] == 0) {
        yubei++;
      }
    }

    /* ????????(?????????,?0??,??????) */
    bias_aver = sum(bias_array) / (47.0f - yubei);
    biaozhuncha = 0.0f;
    for (i = 0; i < 46; i++) {
      x = (float)(signed char)bias_array[i + 1] - bias_aver;
      biaozhuncha += x * x;
    }

    biaozhuncha = sqrt(biaozhuncha / (47.0f - yubei));
    for (i = 0; i < 46; i++) {
      if (fabs((float)(signed char)bias_array[i + 1] - bias_aver) > biaozhuncha)
      {
        bias_array[i + 1] = 0.0f;
      }
    }

    /* ??????(??????????-??) */
    yubei = 0.0f;
    for (i = 0; i < 47; i++) {
      if ((signed char)bias_array[i] == 0) {
        yubei++;
      }
    }

    bias = sum(bias_array) / (47.0f - yubei) - 40.5f;

    /* ?????? */
    if (fabs(bias) < 1.0f) {
      bias = 0.0f;
    }

    *Out_last_bias = bias;
    *Out_bias = bias;

    /* ????,?????????????????????,???????? */
    sum_X = 0.0f;
    sum_Y = 0.0f;
    sum_XY = 0.0f;
    sum_Xsquare = 0.0f;
    for (i = 0; i < 47; i++) {
      if ((signed char)bias_array[i] != 0) {
        sum_X += 1.0f + (float)i;
        sum_Y += (float)(signed char)bias_array[i];
        sum_XY += (float)((1 + i) * (signed char)bias_array[i]);
        sum_Xsquare += (float)((1 + i) * (1 + i));
      }
    }

    sum_X /= 47.0f - yubei;
    sum_Y /= 47.0f - yubei;
    sum_XY /= 47.0f - yubei;
    sum_Xsquare /= 47.0f - yubei;
    *Out_angle = atan((sum_XY - sum_X * sum_Y) / (sum_Xsquare - sum_X * sum_X)) *
      180.0f / 3.1416f;

    /* ???????,??????? */
    /* ??+-2°????? */
    if (fabs(*Out_angle) <= 2.0f) {
      *Out_angle = 0.0f;
    }

    *Out_last_angle = *Out_angle;

    /* ???0???30,??????????,?????? */
    if (yubei > 37.0f) {
      if (bias > 20.0f) {
        *Out_bias = -100.0f;

        /* ?????? */
        *Out_angle = In_last_angle;
        *Out_last_angle = In_last_angle;
      } else {
        if (bias < -20.0f) {
          *Out_bias = 100.0f;

          /* ?????? */
          *Out_angle = In_last_angle;
          *Out_last_angle = In_last_angle;
        }
      }
    }

    for (i = 0; i < 46; i++) {
      if (fabs((float)(signed char)bias_array1[i + 1] - bias_aver) > 2.0f *
          biaozhuncha) {
        bias_array1[i + 1] = 0.0f;
      }
    }

    yubei = 0.0f;
    for (i = 0; i < 47; i++) {
      if ((signed char)bias_array1[i] == 0) {
        yubei++;
      }
    }

    /* ???0???5 ?????,???????????,???row_bias */
    if (yubei >= 5.0f) {
      memset(&bias_array[0], 0, 80U * sizeof(float));
      for (j = 0; j < 78; j++) {
        i = 0;
        exitg1 = false;
        while ((!exitg1) && (i < 47)) {
          if (edge1[(i + 48 * (j + 1)) + 1] - edge1[i + 48 * (j + 1)] == 255) {
            bias_array[j + 1] = 2.0f + (float)i;
            exitg1 = true;
          } else {
            i++;
          }
        }
      }

      if (fabs(sum(bias_array) - 1560.0f) > 1560.0f) {
        memset(&bias_array[0], 0, 80U * sizeof(float));
      }

      /* ????????(?????????,?0??,??????) */
      yubei1 = 0.0f;
      for (i = 0; i < 80; i++) {
        if ((signed char)bias_array[i] == 0) {
          yubei1++;
        }
      }

      bias_aver = sum(bias_array) / (78.0f - yubei1);
      biaozhuncha = 0.0f;
      for (i = 0; i < 78; i++) {
        x = (float)(signed char)bias_array[i + 1] - bias_aver;
        biaozhuncha += x * x;
      }

      biaozhuncha = sqrt(biaozhuncha / (78.0f - yubei1));
      for (i = 0; i < 77; i++) {
        if (fabs((float)(signed char)bias_array[i + 1] - bias_aver) >
            biaozhuncha) {
          bias_array[i + 1] = 0.0f;
        }
      }

      /* ??????(??????????-??) */
      yubei1 = 0.0f;
      for (i = 0; i < 80; i++) {
        if ((signed char)bias_array[i] == 0) {
          yubei1++;
        }
      }

      x = sum(bias_array);
      *Out_row_bias = x / (78.0f - yubei1) - 20.0f;
      *Out_last_row_bias = In_last_row_bias;

      /* ?????? */
      if (fabs(x / (78.0f - yubei1) - 20.0f) < 1.0f) {
        *Out_row_bias = 0.0f;
        *Out_last_row_bias = 0.0f;
      }

      /* ????????,??????????last_place,???????????????? */
    } else {
      *Out_row_bias = In_last_row_bias;
      *Out_last_row_bias = In_last_row_bias;
    }
  } else {
    *Out_last_bias = 0.0f;
    *Out_angle = In_last_angle;
    *Out_last_angle = In_last_angle;
    *Out_bias = 0.0f;
    if (In_last_row_bias > 12.0f) {
      *Out_row_bias = 100.0f;
      *Out_last_row_bias = In_last_row_bias;
    } else if (In_last_row_bias < -12.0f) {
      *Out_row_bias = -100.0f;
      *Out_last_row_bias = In_last_row_bias;
    } else {
      *Out_row_bias = In_last_row_bias;
      *Out_last_row_bias = In_last_row_bias;
    }
  }
}



float tmp;
void Image_Fix(void)	//Í¼ÏñËã·¨
{
	static float last_length,last_angle,last_length_pitch,
								To_next_length,To_next_angle,To_next_length_pitch;
	
	test_simple(gray_column_array,last_length,last_angle,last_length_pitch,
							&length,&angle,&length_pitch,
							&To_next_length,&To_next_angle,&To_next_length_pitch);
	last_length = To_next_length;
	last_angle = To_next_angle;
	last_length_pitch = To_next_length_pitch;
	
	
}

