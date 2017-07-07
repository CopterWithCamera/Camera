#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "image_processing.h"
#include "ff.h"

void rgb_TO_bmp(void);
void TO_SDcard(u8 mode);

//持续写入同一文件模式
extern u8 ToOneFile_StartFlag;
extern FIL ToOneFile_f;				//文件指针
void TO_SDcard_OneFile(u8 mode);	//持续写入同一文件模式的函数



