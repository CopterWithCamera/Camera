//#include "stdafx.h"

#include "rgbTObmp.h"
#include "ff.h"

typedef unsigned char  BYTE;
typedef unsigned short WORD;

/*文件系统相关*/
	
FRESULT res_sd; 
UINT fnum;            					/* File R/W count */

//这是一个存放完整的bmp格式文件区域，此文件将来发送至SD卡
BYTE SDIO_BUFFER_ARRAY[54+IMG_WIDTH*IMG_HEIGHT*3]  __EXRAM;
//BYTE SDIO_BUFFER_ARRAY_DEAL[54+IMG_WIDTH*IMG_HEIGHT*3]  __EXRAM;

//图片文件指针
FIL fnew1;
//FIL dealfnew1;

//文件名称数组
char picn[20];

//typedef struct
//{    long imageSize;
//    long blank;
//    long startPosition;
//}BmpHead;

void RGB2BMP(BYTE ARRAY[])
{
	// fwrte(bfType,1,szeof(bfType),fp1);
	ARRAY[0]='B';ARRAY[1]='M';
	// fwrte(&m_BMPHeader.mageSze,1,szeof(m_BMPHeader.mageSze),fp1);
	ARRAY[2]=0x36;
	ARRAY[3]=0x2D;
	ARRAY[4]=0;
	ARRAY[5]=0;

	// fwrte(&m_BMPHeader.blank,1,szeof(m_BMPHeader.blank),fp1);
	ARRAY[6]=0;
	ARRAY[7]=0;
	ARRAY[8]=0;
	ARRAY[9]=0;

	//fwrte(&m_BMPHeader.startPoston,1,szeof(m_BMPHeader.startPoston),fp1);
	ARRAY[10]=0x36;
	ARRAY[11]=0;
	ARRAY[12]=0;
	ARRAY[13]=0;

	//fwrte(&m_BMPnfoHeader.Length,1,szeof(m_BMPnfoHeader.Length),fp1);
	ARRAY[14]=40;
	ARRAY[15]=0;
	ARRAY[16]=0;
	ARRAY[17]=0;

	//fwrte(&m_BMPnfoHeader.wdth,1,szeof(m_BMPnfoHeader.wdth),fp1);
	ARRAY[18]=80;
	ARRAY[19]=0;
	ARRAY[20]=0;
	ARRAY[21]=0;

	//fwrte(&m_BMPnfoHeader.heght,1,szeof(m_BMPnfoHeader.heght),fp1);
	ARRAY[22]=48;
	ARRAY[23]=0;
	ARRAY[24]=0;
	ARRAY[25]=0;

	//fwrte(&m_BMPnfoHeader.colorPlane,1,szeof(m_BMPnfoHeader.colorPlane),fp1);
	ARRAY[26]=0;
	ARRAY[27]=1;

	//fwrte(&m_BMPnfoHeader.btColor,1,szeof(m_BMPnfoHeader.btColor),fp1);
	ARRAY[28]=24;
	ARRAY[29]=0;

	//fwrte(&m_BMPnfoHeader.zpFormat,1,szeof(m_BMPnfoHeader.zpFormat),fp1);
	ARRAY[30]=0;
	ARRAY[31]=0;
	ARRAY[32]=0;
	ARRAY[33]=0;

	//fwrte(&m_BMPnfoHeader.realSze,1,szeof(m_BMPnfoHeader.realSze),fp1);
	ARRAY[34]=0;
	ARRAY[35]=0x2D;
	ARRAY[36]=0;
	ARRAY[37]=0;

	//fwrte(&m_BMPnfoHeader.xPels,1,szeof(m_BMPnfoHeader.xPels),fp1);
	ARRAY[38]=0;
	ARRAY[39]=0;
	ARRAY[40]=0;
	ARRAY[41]=0;
	//fwrte(&m_BMPnfoHeader.yPels,1,szeof(m_BMPnfoHeader.yPels),fp1);
	ARRAY[42]=0;
	ARRAY[43]=0;
	ARRAY[44]=0;
	ARRAY[45]=0;
	//fwrte(&m_BMPnfoHeader.colorUse,1,szeof(m_BMPnfoHeader.colorUse),fp1);
	ARRAY[46]=0;
	ARRAY[47]=0;
	ARRAY[48]=0;
	ARRAY[49]=0;
	//fwrte(&m_BMPnfoHeader.colormportant,1,szeof(m_BMPnfoHeader.colormportant),fp1);
	ARRAY[50]=0;
	ARRAY[51]=0;
	ARRAY[52]=0;
	ARRAY[53]=0; 
	
}

int a=1;
void TO_SDcard(void)
{   
    long i =1, j=1, k=0;
    uint8_t r,g,b;

	
	a++;	//图片编号累加

	RGB2BMP(SDIO_BUFFER_ARRAY);
	for(i=IMG_HEIGHT;i>0;i=i-1)
	{
		for(j=0;j<IMG_WIDTH*2;j=j+2)
		{
			//rgb565 -> rgb888
			r = (CAMERA_BUFFER_ARRAY[(i-1)*IMG_WIDTH*2+j+1] >> 3) * 8;
			g = (((CAMERA_BUFFER_ARRAY[(i-1)*IMG_WIDTH*2+j+1] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[(i-1)*IMG_WIDTH*2+j] >> 5)) * 4;//这个地方应该是总体*4
			b = (CAMERA_BUFFER_ARRAY[(i-1)*IMG_WIDTH*2+j] & 0x1F) * 8;
			
			//rgb888
			
			SDIO_BUFFER_ARRAY[54+k*3+2]=r;
			SDIO_BUFFER_ARRAY[54+k*3+1]=g;
			SDIO_BUFFER_ARRAY[54+k*3+0]=b;
			k++;			
		}
	}
	k=0;

	sprintf(picn,"pic%d.bmp",a);	//生成图片名称数组
	
	
	res_sd = f_open(&fnew1, picn,FA_CREATE_ALWAYS | FA_WRITE );
	res_sd = f_write(&fnew1,SDIO_BUFFER_ARRAY,sizeof(SDIO_BUFFER_ARRAY),&fnum);
	f_close(&fnew1);
}


	
//********************************************************************************************
