#include "image_fix.h"
#include "image_processing.h"

#define ABS(x) ( (x)>0?(x):-(x) )


/*
      本文件可以使用的资源：
	  1.CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2]					彩色图像数组
	  2.gray_array[IMG_WIDTH*IMG_HEIGHT]    						灰度图像数组
	  3.gray_column_array[IMG_WIDTH*IMG_HEIGHT]						灰度列向量数组
	  4.result_array[IMG_WIDTH*IMG_HEIGHT]							结果图像数组
	  5.result_column_array[IMG_WIDTH*IMG_HEIGHT]					结果列向量数组
	  3.float length;												对外输出参数1（将数值赋值给变量后将自动输出）
	  4.float speed;												对外输出参数2（将数值赋值给变量后将自动输出）
	  5.使用__EXRAM尾缀向外部SDRAM中定义大数组    	例子：int array[1000] __EXRAM;
*/

void Image_Fix(void)	//图像算法
{
	
}

