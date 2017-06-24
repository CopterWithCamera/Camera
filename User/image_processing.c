#include "image_processing.h"
#include "./usart/bsp_debug_usart.h"
#include "math.h"
#include "rgbTObmp.h"
#include "bsp_spi_nrf.h"
#include "image_fix.h"

/*
 * ****** �ܹ�ʹ�õ���Դ *******
 * 
 * ��ʱ #define Delay(ms)  Delay_ms(ms) 
 *
 * ����ͷ�ɼ���ͼ���С����������������鳤�ȣ� extern uint16_t img_width, img_height;
 * 
 */
 
//**************************************************************
	
//ͼ�񻺴�����,��С�����*����*2�ֽ�
uint8_t CAMERA_BUFFER_ARRAY[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//����*���*2���ֽ�

//�Ҷ�ͼ��洢�ռ�
uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//����*���*1�ֽ�

//�������洢�ռ�
uint8_t result_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//����*���*1�ֽ�

//�������
float length;	//ƫ��
float speed;

//���ɻҶȾ���
void Creat_Gray(void)
{
	uint32_t r,g,b;
	
	uint16_t i;
	
	for(i=0;i<IMG_WIDTH*IMG_HEIGHT*2;i=i+2)
	{
		r = (CAMERA_BUFFER_ARRAY[i+1] >> 3) * 8;
		g = (((CAMERA_BUFFER_ARRAY[i+1] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[i] >> 5)) * 4;
		b = (CAMERA_BUFFER_ARRAY[i] & 0x1F) * 8;
		
		gray_array[i/2] = (r * 299 + g * 587 + b * 114 + 500) / 1000;
	}
}

//************************ Ϊ�㷨�ṩ����Դ ************************************

//��ȡ����Gray����ֵ
uint8_t Get_Gray(uint16_t row,uint16_t column)	//��row�У���column��
{
	//�����ķ�ʽ����ȷ����row�У���ȷ���ڱ����еĵ�column����ֵ��
	//�У�row      ��Χ��1 -- IMG_HEIGHT
	//�У�column   ��Χ��1 -- IMG_WIDTH
	
	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	return gray_array[num/2];
}

//�洢�������ݵ�result��
void To_Result(uint16_t row,uint16_t column,uint8_t gray)
{
	//�����ķ�ʽ����ȷ����row�У���ȷ���ڱ����еĵ�column����ֵ��
	//�У�row      ��Χ��1 -- IMG_HEIGHT
	//�У�column   ��Χ��1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	result_array[num/2] = gray;
}

//************** �����Ϣ ************************************************

void Data_Output_Ctrl(u8 num,u8 cmd)	//num��ָ���		cmd��ָ������
{
	switch(num)
	{
		case 1:			//ͼ�������
			if(cmd)
			{
				
			}
			else
			{
				
			}
			break;
		case 2:			//���δ������
			if(cmd)
			{
				
			}
			else
			{
				
			}			
			break;
		case 3:			//��ʾfps
			if(cmd)
			{
				
			}
			else
			{
				
			}
			break;
		default:
			break;
	}
}

//�����ݴ��͵�����˿�
void Data_Output(u8 ch)
{
	#ifdef __USART_DISPLAY

		USART2_Send(ch);
	
	#endif
	
	#ifdef __NRF_DISPLAY
		if(NRF24L01_State)
		{
			NRF_Send(ch);	//NRF����
		}
	#endif
}

//��ʾͼ�����ɽ��๦�ܵ�������
void Display_Image(void)
{
	uint32_t i;
	uint8_t ch;
	
	//���Ͱ�ͷ
	ch = 0x01;
	Data_Output(ch);

	ch = 0xFE;
	Data_Output(ch);

	
	//����ͼ��
	for(i = 0 ; i<IMG_HEIGHT*IMG_WIDTH; i++ )
	{		
		ch = gray_array[i];
		Data_Output(ch);
	}
	
	//���Ͱ�β
	ch = 0xFE;
	Data_Output(ch);
	
	ch = 0x01;
	Data_Output(ch);
	
}

//floatת4��unsigned char
void float_char(float f,unsigned char *s)
{
	unsigned char *p;
 
	p = (unsigned char *)&f;
    *s = *p;
    *(s+1) = *(p+1);
    *(s+2) = *(p+2);
    *(s+3) = *(p+3);
}

//������Σ�length��speed������ɽ��๦�ܵ������ֲ鿴
void Display_Wave(void)
{
	uint8_t ch;
	unsigned char a[4];
	
	//���Ͱ�ͷ
	ch = 0x03;
	Data_Output(ch);
	ch = 0xFC;
	Data_Output(ch);
	
	//��������
	
	//����ͨ��һ
	float_char(length,a);
	ch = a[0];
	Data_Output(ch);
	ch = a[1];
	Data_Output(ch);
	ch = a[2];
	Data_Output(ch);	
	ch = a[3];
	Data_Output(ch);
	
	//����ͨ����
	float_char(speed,a);
	ch = a[0];
	Data_Output(ch);
	ch = a[1];
	Data_Output(ch);
	ch = a[2];
	Data_Output(ch);
	ch = a[3];
	Data_Output(ch);		
	
	//���Ͱ�β
	ch = 0xFC;
	Data_Output(ch);
	ch = 0x03;
	Data_Output(ch);
	
}

void Send_Parameter_Fps(void)
{
	uint8_t ch;
	unsigned char a[4];
	
	ch = 0x04;
	Data_Output(ch);
	ch = 0xFB;
	Data_Output(ch);
	
	//����fps
	float_char(fps,a);
	ch = a[0];
	Data_Output(ch);
	ch = a[1];
	Data_Output(ch);
	ch = a[2];
	Data_Output(ch);	
	ch = a[3];
	Data_Output(ch);	
	
	//���Ͱ�β
	ch = 0xFB;
	Data_Output(ch);
	ch = 0x04;
	Data_Output(ch);
}

void Image_Output(void)
{
	//*******************************************************************
	//�����Ϣ
	
	if(!full_flag)
	{
		#if defined(__DISPLAY_IMAGE)
		
			Display_Image();	//�Ӵ������ͼ�����ɽ��๦�ܵ���������ʾ
		
		#endif
		
			
		#if defined(__DISPALY_WAVE)
		
			Display_Wave();	//�����������
		
		#endif
		
		
		#if defined(__PARAMETER_FPS)
		
			Send_Parameter_Fps();
		
		#endif
	}
	
	//*******************************************************************
	//SD��ͼ
	
	#ifdef __SD_SAVE
	
		if(SD_State)	//���SD�����سɹ�
		{
			TO_SDcard();    //SD��
		}
		
	#endif
}

//*********************** ��ִ�к��� **********************************************

uint8_t image_updata_flag = 0;		//��ͼ��ɼ���ɱ�־  0����ͼû�вɼ����    1����ͼ�ɼ����
void Image_Process(void)
{
	DCMI_CaptureCmd(ENABLE);			//��ȡһ֡ͼ�񵽻���

	//��д��
	//��ʱ1s �� ͼ��ɼ�����ж���λ
	image_updata_flag = 0;
	Task_Delay[9] = 1000;
	while(Task_Delay[9]!=0 && image_updata_flag == 0){}
	
	Creat_Gray();	//���ɻҶȾ�������������ʾ����
	Image_Fix();	//ͼ������
	Image_Output();	//�������

}

