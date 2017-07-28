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

/******µÚËÄ°æ********/
  unsigned char b_a[3840] __EXRAM;
  short e[3840] __EXRAM;
  short edge1[3840] __EXRAM;
  float bias_array[80] __EXRAM;
  float bias_array1[80] __EXRAM;


 void test_simple(const unsigned char a[3840], float In_last_bias, float
                 In_last_angle, float *Out_bias, float *Out_angle, float
                 *Out_last_bias, float *Out_last_angle)
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
  boolean_T exitg3;

  float yubei;
  boolean_T exitg2;
  float bias_aver;
  boolean_T exitg1;
  float biaozhuncha;
  float c_a;
  float sum_X;
  float sum_Y;
  float sum_XY;
  float sum_Xsquare;
  float gradien;
  float angle;
  float bias;

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
  exitg3 = false;
  while ((!exitg3) && (!flag)) {
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
      exitg3 = true;
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
        exitg3 = true;
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
  if (test_flag == 0) 
	{
    memset(&bias_array[0], 0, 80U * sizeof(float));
    memset(&bias_array1[0], 0, 80U * sizeof(float));
    yubei = 0.0f;
    for (i = 0; i < 46; i++) {
      j = 0;
      exitg2 = false;
      while ((!exitg2) && (j < 78)) {
        if (edge1[(48 * (j + 1) - i) + 46] - edge1[(48 * (2 + j) - i) + 46] ==
            255) {
          bias_array[46 - i] = 2.0f + (float)j;
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
          bias_array1[46 - i] = 79.0f + -(float)j;
          exitg1 = true;
        } else {
          j++;
        }
      }
    }

    if (fabs(sum(bias_array) - 1903.5f) > fabs(sum(bias_array1) - 1903.5f)) {
      memcpy(&bias_array[0], &bias_array1[0], 80U * sizeof(float));
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
      c_a = (float)(signed char)bias_array[i + 1] - bias_aver;
      biaozhuncha += c_a * c_a;
    }

    biaozhuncha = sqrt(biaozhuncha / (47.0f - yubei));
    for (i = 0; i < 46; i++) {
      if (fabs((float)(signed char)bias_array[i + 1] - bias_aver) > 2.0f *
          biaozhuncha) {
        bias_array[i + 1] = 0.0f;
      }
    }

    /*      %??????(??????????-??) */
    /*  */
    /*      yubei=0; */
    /*      for i=1:47 */
    /*          if bias_array(i,1)==0 */
    /*              yubei=yubei+1; */
    /*          end */
    /*      end */
    /*      bias=sum(bias_array)/(47-yubei)-40.5; */
    /* ?????? */
    /*      if abs(bias)<1 */
    /*          bias=0; */
    /*      end */
    /*      Out_last_bias=bias; */
    /*      Out_bias=bias; */
    /*      In_last_bias=Out_last_bias; */
    /* ????,?????????????????????,???????? */
    sum_X = 0.0f;
    sum_Y = 0.0f;
    sum_XY = 0.0f;
    sum_Xsquare = 0.0f;
    for (i = 0; i < 47; i++) 
		{
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
    gradien = (sum_XY - sum_X * sum_Y) / (sum_Xsquare - sum_X * sum_X);
    angle = atan(gradien) * 180.0f / 3.1416f;

    /* ???????,??????? */
    *Out_angle = angle;

    /*      In_last_angle=Out_last_angle; */
    /* ??+-2°????? */
    /*      if abs(Out_angle)<=2 */
    /*          Out_angle=0; */
    /*      end */
    /*      In_last_angle=Out_angle; */
    *Out_last_angle = angle;
    bias = (gradien * 24.5f + (sum_Xsquare * sum_Y - sum_X * sum_XY) /
            (sum_Xsquare - sum_X * sum_X)) - 40.5f;
    *Out_bias = bias;
    *Out_last_bias = bias;

    /*      In_last_bias=Out_last_bias; */
    if (bias >= 40.0f) {
      *Out_bias = 100.0f;
    } 
		else 
		{
      if (bias <= -40.0f) {
        *Out_bias = -100.0f;
      }
    }

    /* ???0???30,??????????,?????? */
    if (yubei > 37.0f) 
		{
      if (In_last_bias > 20.0f) 
			{
        *Out_bias = 100.0f;

        /* ?????? */
        *Out_last_bias = In_last_bias;
        *Out_last_angle = In_last_angle;
        *Out_angle = In_last_angle;
      } 
			else 
			{
        if (In_last_bias < -20.0f) 
				{
          *Out_bias = -100.0f;

          /* ?????? */
          *Out_last_bias = In_last_bias;
          *Out_last_angle = In_last_angle;
          *Out_angle = In_last_angle;
        }
      }
    } 
//		else 
//		{
//      *Out_bias = In_last_bias;

//      /* ?????? */
//      *Out_last_bias = In_last_bias;
//      *Out_last_angle = In_last_angle;
//      *Out_angle = In_last_angle;
//    }

    /*           for i=2:47 */
    /*              if abs(bias_array1(i,1)-bias_aver)>2*biaozhuncha */
    /*                  bias_array1(i,1)=0; */
    /*              end */
    /*           end */
    /*           yubei=0; */
    /*          for i=1:47 */
    /*              if bias_array1(i,1)==0 */
    /*                  yubei=yubei+1; */
    /*              end */
    /*          end */
    /*  ???0???5 ?????,???????????,???row_bias */
    /*          if yubei>=5 */
    /*              bias_array=zeros(80,1); */
    /*              bias_array1=zeros(80,1); */
    /*              for j=2:79 */
    /*                  for i=2:48 */
    /*                      if edge1(i,j)-edge1(i-1,j)==255; */
    /*                          bias_array(j,1)=i; */
    /*                          break; */
    /*                      end */
    /*                  end */
    /*              end */
    /*              for j=2:79 */
    /*                  for i=48:2 */
    /*                      if edge1(i,j)-edge1(i-1,j)==255; */
    /*                          bias_array(j,1)=i; */
    /*                          break; */
    /*                      end */
    /*                  end */
    /*              end */
    /*  */
    /*              if abs(sum(bias_array)-1560)>abs(sum(bias_array1)-1560) */
    /*                  bias_array(:)=bias_array1(:); */
    /*              end */
    /* ????????(?????????,?0??,??????) */
    /*              yubei1=0; */
    /*              for i=1:80 */
    /*                  if bias_array(i,1)==0 */
    /*                      yubei1=yubei1+1; */
    /*                  end */
    /*              end */
    /*              bias_aver=sum(bias_array)/(78-yubei1); */
    /*              biaozhuncha=0; */
    /*              for i=2:79 */
    /*                  biaozhuncha=biaozhuncha+(bias_array(i,1)-bias_aver)^2; */
    /*              end */
    /*              biaozhuncha=sqrt(biaozhuncha/(78-yubei1)); */
    /*              for i=2:78 */
    /*                  if abs(bias_array(i,1)-bias_aver)>biaozhuncha */
    /*                      bias_array(i,1)=0; */
    /*                  end */
    /*              end */
    /* ??????(??????????-??) */
    /*              yubei1=0; */
    /*              for i=1:80 */
    /*                  if bias_array(i,1)==0 */
    /*                      yubei1=yubei1+1; */
    /*                  end */
    /*              end */
    /*              row_bias=sum(bias_array)/(78-yubei1)-20; */
    /* ?????? */
    /*              if abs(row_bias)<1 */
    /*                  row_bias=0; */
    /*              end */
    /* ????????,??????????last_place,???????????????? */
    /*          end */
  } 
	else if (In_last_bias > 20.0f) 
	{
    *Out_bias = 100.0f;

    /* ?????? */
    *Out_last_bias = In_last_bias;
    *Out_angle = In_last_angle;
    *Out_last_angle = In_last_angle;
  } else if (In_last_bias < -20.0f) 
	{
    *Out_bias = -100.0f;

    /* ?????? */
    *Out_last_bias = In_last_bias;
    *Out_angle = In_last_angle;
    *Out_last_angle = In_last_angle;
  } else 
	{
    *Out_last_bias = In_last_bias;
    *Out_angle = In_last_angle;
    *Out_last_angle = In_last_angle;
    *Out_bias = In_last_bias;
  }

  /*  end */
}



float tmp;
void Image_Fix(void)	//Í¼ÏñËã·¨
{
	static float last_length,last_angle,
								To_next_length,To_next_angle;
	
	test_simple(gray_column_array,last_length,last_angle,
							&length,&angle,
							&To_next_length,&To_next_angle);
	last_length = To_next_length;
	last_angle = To_next_angle;
}

