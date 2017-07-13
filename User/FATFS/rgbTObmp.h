#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "image_processing.h"
#include "ff.h"

void rgb_TO_bmp(void);
void TO_SDcard(u8 mode);

//持续写入同一文件（图像）
extern u8 ToOneFile_StartFlag;
extern FIL ToOneFile_f;				//文件指针
void TO_SDcard_OneFile(u8 mode);	//持续写入同一文件模式的函数

//持续写入同一文件（高度）
extern u8 Height_StartFlag;			//开始定时写入FLAG
extern FIL Height_f;				//文件指针
void TO_SDcard_Height(void);		//向SD卡中写入txt数据文件



