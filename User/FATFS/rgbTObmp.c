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
			r = (CAMERA_BUFFER_ARRAY[0][(i-1)*IMG_WIDTH*2+j+1] >> 3) * 8;
			g = (((CAMERA_BUFFER_ARRAY[0][(i-1)*IMG_WIDTH*2+j+1] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[0][(i-1)*IMG_WIDTH*2+j] >> 5)) * 4;//这个地方应该是总体*4
			b = (CAMERA_BUFFER_ARRAY[0][(i-1)*IMG_WIDTH*2+j] & 0x1F) * 8;
			
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


	
//********************************************************************************************8

//	处理后图像存储代码
//			RGB2BMP(SDIO_BUFFER_ARRAY_DEAL);
//			for(i=IMG_HEIGHT;i>0;i=i-1)
//			{
//				for(j=0;j<IMG_WIDTH*2;j=j+2)
//				{
//					r = (CAMERA_BUFFER_ARRAY[1][(i-1)*IMG_WIDTH*2+j+1] >> 3) * 8;
//					g = (((CAMERA_BUFFER_ARRAY[1][(i-1)*IMG_WIDTH*2+j+1] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[1][(i-1)*IMG_WIDTH*2+j] >> 5)) * 4;//这个地方应该是总体*4
//					b = (CAMERA_BUFFER_ARRAY[1][(i-1)*IMG_WIDTH*2+j] & 0x1F) * 8;
//					
//					//rgb888
//					
//					SDIO_BUFFER_ARRAY_DEAL[54+k*3+2]=r;
//					SDIO_BUFFER_ARRAY_DEAL[54+k*3+1]=g;
//					SDIO_BUFFER_ARRAY_DEAL[54+k*3+0]=b;
//					k++;			
//				}	
//			}	
//				k=0;
//     if(a/4==1)
//		 {
//			res_sd = f_open(&dealfnew1, dealpic1,FA_CREATE_ALWAYS | FA_WRITE );
//			res_sd=f_write(&dealfnew1,SDIO_BUFFER_ARRAY_DEAL,sizeof(SDIO_BUFFER_ARRAY_DEAL),&fnum);   
//			f_close(&dealfnew1);	
//		 }
//			if(a/4==2)
//		 {
//			res_sd = f_open(&dealfnew2, dealpic2,FA_CREATE_ALWAYS | FA_WRITE );
//			res_sd=f_write(&dealfnew2,SDIO_BUFFER_ARRAY_DEAL,sizeof(SDIO_BUFFER_ARRAY_DEAL),&fnum);   
//			f_close(&dealfnew2);	
//		 }
//			if(a/4==3)
//		 {
//			res_sd = f_open(&dealfnew3, dealpic3,FA_CREATE_ALWAYS | FA_WRITE );
//			res_sd=f_write(&dealfnew3,SDIO_BUFFER_ARRAY_DEAL,sizeof(SDIO_BUFFER_ARRAY_DEAL),&fnum);   
//			f_close(&dealfnew3);	
//		 }
//		 if(a/4==4)
//		 {
//			res_sd = f_open(&dealfnew4, dealpic4,FA_CREATE_ALWAYS | FA_WRITE );
//			res_sd=f_write(&dealfnew4,SDIO_BUFFER_ARRAY_DEAL,sizeof(SDIO_BUFFER_ARRAY_DEAL),&fnum);   
//			f_close(&dealfnew4);	
//		 }
//		 if(a/4==5)
//		 {
//			res_sd = f_open(&dealfnew5, dealpic5,FA_CREATE_ALWAYS | FA_WRITE );
//			res_sd=f_write(&dealfnew5,SDIO_BUFFER_ARRAY_DEAL,sizeof(SDIO_BUFFER_ARRAY_DEAL),&fnum);   
//			f_close(&dealfnew5);	
//		 }
//		 if(a/4==6)
//		 {
//			res_sd = f_open(&dealfnew6, dealpic6,FA_CREATE_ALWAYS | FA_WRITE );
//			res_sd=f_write(&dealfnew6,SDIO_BUFFER_ARRAY_DEAL,sizeof(SDIO_BUFFER_ARRAY_DEAL),&fnum);   
//			f_close(&dealfnew6);	
//		 }
//			if(a/4==7)
//		 {
//			res_sd = f_open(&dealfnew7, dealpic7,FA_CREATE_ALWAYS | FA_WRITE );
//			res_sd=f_write(&dealfnew7,SDIO_BUFFER_ARRAY_DEAL,sizeof(SDIO_BUFFER_ARRAY_DEAL),&fnum);   
//			f_close(&dealfnew7);	
//		 }
//			if(a/4==8)
//		 {
//			res_sd = f_open(&dealfnew8, dealpic8,FA_CREATE_ALWAYS | FA_WRITE );
//			res_sd=f_write(&dealfnew8,SDIO_BUFFER_ARRAY_DEAL,sizeof(SDIO_BUFFER_ARRAY_DEAL),&fnum);   
//			f_close(&dealfnew8);	
//		 }
//			if(a/4==9)
//		 {
//			res_sd = f_open(&dealfnew9, dealpic9,FA_CREATE_ALWAYS | FA_WRITE );
//			res_sd=f_write(&dealfnew9,SDIO_BUFFER_ARRAY_DEAL,sizeof(SDIO_BUFFER_ARRAY_DEAL),&fnum);   
//			f_close(&dealfnew9);	
//		 }
//		 if(a/4==10)
//		 {
//			res_sd = f_open(&dealfnew10, dealpic10,FA_CREATE_ALWAYS | FA_WRITE );
//			res_sd=f_write(&dealfnew10,SDIO_BUFFER_ARRAY_DEAL,sizeof(SDIO_BUFFER_ARRAY_DEAL),&fnum);   
//			f_close(&dealfnew10);	
//			 a=0;
//		 }		
			
	//}

//旧的RGB2BMP
//void RGB2BMP(BYTE ARRAY[])
//{
////     BmpHead m_BMPHeader;
////     char bfType[2]={'B','M'};
////     m_BMPHeader.imageSize=3*IMG_WIDTH*IMG_HEIGHT+54;
////     m_BMPHeader.blank=0;
////     m_BMPHeader.startPosition=54;

//		 // fwrte(bfType,1,szeof(bfType),fp1);
//		 ARRAY[0]='B';ARRAY[1]='M';
//		 // fwrte(&m_BMPHeader.mageSze,1,szeof(m_BMPHeader.mageSze),fp1);
//		 ARRAY[2]=0x36;
//		 ARRAY[3]=0x2D;
//		 ARRAY[4]=0;
//		 ARRAY[5]=0;
//    
//		// fwrte(&m_BMPHeader.blank,1,szeof(m_BMPHeader.blank),fp1);
//		 ARRAY[6]=0;
//		 ARRAY[7]=0;
//		 ARRAY[8]=0;
//		 ARRAY[9]=0;

//     //fwrte(&m_BMPHeader.startPoston,1,szeof(m_BMPHeader.startPoston),fp1);
// 		 ARRAY[10]=0x36;
//		 ARRAY[11]=0;
//		 ARRAY[12]=0;
//		 ARRAY[13]=0;
//		 
////     nfoHead  m_BMPnfoHeader;
////     m_BMPnfoHeader.Length=40; 
////     m_BMPnfoHeader.wdth=nWdth;
////     m_BMPnfoHeader.heght=nHeght;
////     m_BMPnfoHeader.colorPlane=1;
////     m_BMPnfoHeader.btColor=24;
////     m_BMPnfoHeader.zpFormat=0;
////     m_BMPnfoHeader.realSze=3*nWdth*nHeght;
////     m_BMPnfoHeader.xPels=0;
////     m_BMPnfoHeader.yPels=0;
////     m_BMPnfoHeader.colorUse=0;
////     m_BMPnfoHeader.colormportant=0;
// 
//     //fwrte(&m_BMPnfoHeader.Length,1,szeof(m_BMPnfoHeader.Length),fp1);
//		 ARRAY[14]=40;
//		 ARRAY[15]=0;
//		 ARRAY[16]=0;
//		 ARRAY[17]=0;
//		 
//     //fwrte(&m_BMPnfoHeader.wdth,1,szeof(m_BMPnfoHeader.wdth),fp1);
//		 ARRAY[18]=80;
//		 ARRAY[19]=0;
//		 ARRAY[20]=0;
//		 ARRAY[21]=0;
//		 
//     //fwrte(&m_BMPnfoHeader.heght,1,szeof(m_BMPnfoHeader.heght),fp1);
//		 ARRAY[22]=48;
//		 ARRAY[23]=0;
//		 ARRAY[24]=0;
//		 ARRAY[25]=0;
//		 
//     //fwrte(&m_BMPnfoHeader.colorPlane,1,szeof(m_BMPnfoHeader.colorPlane),fp1);
//		 ARRAY[26]=0;
//		 ARRAY[27]=1;
//		 
//     //fwrte(&m_BMPnfoHeader.btColor,1,szeof(m_BMPnfoHeader.btColor),fp1);
//		 ARRAY[28]=24;
//		 ARRAY[29]=0;
//		 
//     //fwrte(&m_BMPnfoHeader.zpFormat,1,szeof(m_BMPnfoHeader.zpFormat),fp1);
//		 ARRAY[30]=0;
//		 ARRAY[31]=0;
//		 ARRAY[32]=0;
//		 ARRAY[33]=0;
//		 
//     //fwrte(&m_BMPnfoHeader.realSze,1,szeof(m_BMPnfoHeader.realSze),fp1);
//		 ARRAY[34]=0;
//		 ARRAY[35]=0x2D;
//		 ARRAY[36]=0;
//		 ARRAY[37]=0;
//		 
//     //fwrte(&m_BMPnfoHeader.xPels,1,szeof(m_BMPnfoHeader.xPels),fp1);
//		 ARRAY[38]=0;
//		 ARRAY[39]=0;
//		 ARRAY[40]=0;
//		 ARRAY[41]=0;
//     //fwrte(&m_BMPnfoHeader.yPels,1,szeof(m_BMPnfoHeader.yPels),fp1);
//		 ARRAY[42]=0;
//		 ARRAY[43]=0;
//		 ARRAY[44]=0;
//		 ARRAY[45]=0;
//     //fwrte(&m_BMPnfoHeader.colorUse,1,szeof(m_BMPnfoHeader.colorUse),fp1);
//		 ARRAY[46]=0;
//		 ARRAY[47]=0;
//		 ARRAY[48]=0;
//		 ARRAY[49]=0;
//     //fwrte(&m_BMPnfoHeader.colormportant,1,szeof(m_BMPnfoHeader.colormportant),fp1);
//		 ARRAY[50]=0;
//		 ARRAY[51]=0;
//		 ARRAY[52]=0;
//		 ARRAY[53]=0; 
//	
//}
