#include "rgbTObmp.h"
#include "ff.h"

typedef unsigned char  BYTE;
typedef unsigned short WORD;

/*文件系统相关*/
	
FRESULT res_sd; 
UINT fnum;            					/* File R/W count */

BYTE SDIO_BUFFER_ARRAY[54+IMG_WIDTH*IMG_HEIGHT*3]  __EXRAM;		//存放完整的bmp格式文件区域
FIL fnew1;		//图片文件指针
char picn[20];	//文件名称数组

void WRITE_BMP_HEAD(BYTE ARRAY[])		//向图像暂存矩阵写入BMP文件前54字节
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

int a=0;	//文件编号
void TO_SDcard(u8 mode)	//数据源模式   0 -- 摄像头缓存   1 -- 灰度矩阵   2 -- 结果矩阵
{   
    long i,j;
    unsigned char r,g,b;

	a++;	//图片编号累加（从1开始计数）

	WRITE_BMP_HEAD(SDIO_BUFFER_ARRAY);	//写入BMP前54字节文件信息
	
	for(i=IMG_HEIGHT;i>0;i=i-1)
	{
		for(j=0;j<IMG_WIDTH;j++)
		{
			switch(mode)
			{
				case 0:
					//rgb565 -> rgb888（从摄像头缓存取数据）
					r = (CAMERA_BUFFER_ARRAY[(i-1)*IMG_WIDTH*2+j*2+1] >> 3) * 8;
					g = (((CAMERA_BUFFER_ARRAY[(i-1)*IMG_WIDTH*2+j*2+1] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[(i-1)*IMG_WIDTH*2+j*2] >> 5)) * 4;//这个地方应该是总体*4
					b = (CAMERA_BUFFER_ARRAY[(i-1)*IMG_WIDTH*2+j*2] & 0x1F) * 8;
					sprintf(picn,"pic%d_original.bmp",a);	//生成图片名称数组
				break;
				
				case 1:
					//gray_array
					r = g = b = gray_array[i*IMG_WIDTH+j];
					sprintf(picn,"pic%d_gray.bmp",a);	//生成图片名称数组
				break;
				
				case 2:
					//result_array
					r = g = b = result_array[i*IMG_WIDTH+j];
					sprintf(picn,"pic%d_result.bmp",a);	//生成图片名称数组
				break;
				
				default:
				break;
			}
			
			//rgb888数据存入图像矩阵
			
			SDIO_BUFFER_ARRAY[54+i*IMG_WIDTH*3+j*3+2]=r;
			SDIO_BUFFER_ARRAY[54+i*IMG_WIDTH*3+j*3+1]=g;
			SDIO_BUFFER_ARRAY[54+i*IMG_WIDTH*3+j*3+0]=b;
		}
	}
	
	res_sd = f_open(&fnew1, picn,FA_CREATE_ALWAYS | FA_WRITE );
	res_sd = f_write(&fnew1,SDIO_BUFFER_ARRAY,sizeof(SDIO_BUFFER_ARRAY),&fnum);
	f_close(&fnew1);
}

//向一个文件中持续写入数据，不关闭文件
u8 ToOneFile_StartFlag = 0;		//开始定时写入FLAG
char ToOneFile_FileName[20];	//文件名称数组
FIL ToOneFile_f;				//文件指针
BYTE SDIO_ONEFILE_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT]  __EXRAM;		//一张灰度图片像素信息长度的缓存区
void TO_SDcard_OneFile(u8 mode)	//数据源模式   0 -- 摄像头缓存   1 -- 灰度矩阵   2 -- 结果矩阵
{
    long i,j;
    unsigned char r,g,b;
	
	if(!ToOneFile_StartFlag)
	{
		ToOneFile_StartFlag = 1;
		
		sprintf(ToOneFile_FileName,"OneFile.hex");	//生成文件名
		res_sd = f_open(&ToOneFile_f, ToOneFile_FileName,FA_CREATE_ALWAYS | FA_WRITE );	//打开文件
	}
	
	for(i=IMG_HEIGHT;i>0;i=i-1)
	{
		for(j=0;j<IMG_WIDTH;j++)
		{
			switch(mode)
			{	
				case 1:
					//gray_array
					r = gray_array[i*IMG_WIDTH+j];
				break;
				
				case 2:
					//result_array
					r = result_array[i*IMG_WIDTH+j];
				break;
				
				default:
				break;
			}
			
			//rgb888数据存入图像矩阵
			
			SDIO_ONEFILE_BUFFER_ARRAY[i*IMG_WIDTH+j]=r;
		}
	}
	
	res_sd = f_write(&ToOneFile_f,SDIO_ONEFILE_BUFFER_ARRAY,sizeof(SDIO_ONEFILE_BUFFER_ARRAY),&fnum);	//写入数据，不关闭文件
}


//********************************************************************************************
