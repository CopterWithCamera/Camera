#include "rgbTObmp.h"
#include "ff.h"

typedef unsigned char  BYTE;
typedef unsigned short WORD;

/*�ļ�ϵͳ���*/
	
FRESULT res_sd; 
UINT fnum;            					/* File R/W count */

BYTE SDIO_BUFFER_ARRAY[54+IMG_WIDTH*IMG_HEIGHT*3]  __EXRAM;		//���������bmp��ʽ�ļ�����
FIL fnew1;		//ͼƬ�ļ�ָ��
char picn[20];	//�ļ���������

void WRITE_BMP_HEAD(BYTE ARRAY[])		//��ͼ���ݴ����д��BMP�ļ�ǰ54�ֽ�
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

int a=0;	//�ļ����
void TO_SDcard(u8 mode)	//����Դģʽ   0 -- ����ͷ����   1 -- �ҶȾ���   2 -- �������
{   
    long i,j;
    unsigned char r,g,b;

	a++;	//ͼƬ����ۼӣ���1��ʼ������

	WRITE_BMP_HEAD(SDIO_BUFFER_ARRAY);	//д��BMPǰ54�ֽ��ļ���Ϣ
	
	for(i=IMG_HEIGHT;i>0;i=i-1)
	{
		for(j=0;j<IMG_WIDTH;j++)
		{
			switch(mode)
			{
				case 0:
					//rgb565 -> rgb888��������ͷ����ȡ���ݣ�
					r = (CAMERA_BUFFER_ARRAY[(i-1)*IMG_WIDTH*2+j*2+1] >> 3) * 8;
					g = (((CAMERA_BUFFER_ARRAY[(i-1)*IMG_WIDTH*2+j*2+1] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[(i-1)*IMG_WIDTH*2+j*2] >> 5)) * 4;//����ط�Ӧ��������*4
					b = (CAMERA_BUFFER_ARRAY[(i-1)*IMG_WIDTH*2+j*2] & 0x1F) * 8;
					sprintf(picn,"pic%d_original.bmp",a);	//����ͼƬ��������
				break;
				
				case 1:
					//gray_array
					r = g = b = gray_array[i*IMG_WIDTH+j];
					sprintf(picn,"pic%d_gray.bmp",a);	//����ͼƬ��������
				break;
				
				case 2:
					//result_array
					r = g = b = result_array[i*IMG_WIDTH+j];
					sprintf(picn,"pic%d_result.bmp",a);	//����ͼƬ��������
				break;
				
				default:
				break;
			}
			
			//rgb888���ݴ���ͼ�����
			
			SDIO_BUFFER_ARRAY[54+i*IMG_WIDTH*3+j*3+2]=r;
			SDIO_BUFFER_ARRAY[54+i*IMG_WIDTH*3+j*3+1]=g;
			SDIO_BUFFER_ARRAY[54+i*IMG_WIDTH*3+j*3+0]=b;
		}
	}
	
	res_sd = f_open(&fnew1, picn,FA_CREATE_ALWAYS | FA_WRITE );
	res_sd = f_write(&fnew1,SDIO_BUFFER_ARRAY,sizeof(SDIO_BUFFER_ARRAY),&fnum);
	f_close(&fnew1);
}

//��һ���ļ��г���д�����ݣ����ر��ļ�
u8 ToOneFile_StartFlag = 0;		//��ʼ��ʱд��FLAG
char ToOneFile_FileName[20];	//�ļ���������
FIL ToOneFile_f;				//�ļ�ָ��
BYTE SDIO_ONEFILE_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT]  __EXRAM;		//һ�ŻҶ�ͼƬ������Ϣ���ȵĻ�����
void TO_SDcard_OneFile(u8 mode)	//����Դģʽ   0 -- ����ͷ����   1 -- �ҶȾ���   2 -- �������
{
    long i,j;
    unsigned char r,g,b;
	
	if(!ToOneFile_StartFlag)
	{
		ToOneFile_StartFlag = 1;
		
		sprintf(ToOneFile_FileName,"OneFile.hex");	//�����ļ���
		res_sd = f_open(&ToOneFile_f, ToOneFile_FileName,FA_CREATE_ALWAYS | FA_WRITE );	//���ļ�
	}
	
	for(i=IMG_HEIGHT-1;i>=0;i--)
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
			
			//rgb888���ݴ���ͼ�����
			
			SDIO_ONEFILE_BUFFER_ARRAY[i*IMG_WIDTH+j]=r;
		}
	}
	
	res_sd = f_write(&ToOneFile_f,SDIO_ONEFILE_BUFFER_ARRAY,sizeof(SDIO_ONEFILE_BUFFER_ARRAY),&fnum);	//д�����ݣ����ر��ļ�
}


//********************************************************************************************
