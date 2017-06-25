#include "image_fix.h"
#include "image_processing.h"

#define ABS(x) ( (x)>0?(x):-(x) )


/*
      本文件可以使用的资源：
	  1.CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2]					彩色图像数组
	  2.gray_array[IMG_WIDTH*IMG_HEIGHT];     						灰度图像数组
	  3.uint8_t Get_Gray(uint16_t row,uint16_t column);				从Gray区提取数据
      4.void To_Result(uint16_t row,uint16_t column,uint8_t gray);	向Result取存入数据
	  5.float length;												对外输出参数1（将数值赋值给变量后将自动输出）
	  6.float speed;												对外输出参数2（将数值赋值给变量后将自动输出）
	  7.使用__EXRAM尾缀向外部SDRAM中定义大数组    	例子：int array[1000] __EXRAM;
*/

uint8_t temp_array[IMG_WIDTH*IMG_HEIGHT];	//第二块灰度空间，作为运算临时存储空间

//存储单点数据到暂存区
void To_Temp(uint16_t row,uint16_t column,uint8_t gray)
{
	//计算点的方式是先确定第row行，再确定在本行中的第column个数值。
	//行：row      范围：1 -- IMG_HEIGHT
	//列：column   范围：1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	temp_array[num/2] = gray;
}


//获取单点暂存区数值
uint8_t Get_Temp(uint16_t row,uint16_t column)
{
	//计算点的方式是先确定第row行，再确定在本行中的第column个数值。
	//行：row      范围：1 -- IMG_HEIGHT
	//列：column   范围：1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	return temp_array[num/2];
}

void Image_Fix(void)	//图像算法
{
	
}

