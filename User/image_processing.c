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

uint8_t CAMERA_BUFFER_ARRAY1[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//����*���*2���ֽ�
uint8_t CAMERA_BUFFER_ARRAY2[IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//����*���*2���ֽ�

uint8_t * CAMERA_BUFFER_ARRAY = CAMERA_BUFFER_ARRAY1;	//��ǰ����ָ��
uint8_t * DCMI_IN_BUFFER_ARRAY = CAMERA_BUFFER_ARRAY2;	//��ǰ���뻺��ָ��

//�Ҷ�ͼ��洢�ռ�
uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//����*���*1�ֽ�

//�������洢�ռ�
uint8_t result_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//����*���*1�ֽ�

//�������
float length;	//ƫ��
float speed;

//�������ݵ�ģʽ
unsigned char mode = 0;

//���ƴ����flag
u8 flag_Image = 0;
u8 flag_Result = 0;
u8 flag_Wave = 0;
u8 flag_Sd_original = 0;
u8 flag_Sd_gray = 0;
u8 flag_Sd_result = 0;

//��������flag��һ��Ҫ������
u8 flag_Fps = 1;
u8 flag_Mode = 1;

//���ɻҶȾ���
void Creat_Gray(void)
{
	uint32_t r,g,b;
	
	uint16_t i,j;

 	uint8_t tmp;
	
	//ת�Ҷ�
	for(i=0;i<IMG_WIDTH*IMG_HEIGHT*2;i=i+2)
	{
		r = (CAMERA_BUFFER_ARRAY[i+1] >> 3) * 8;
		g = (((CAMERA_BUFFER_ARRAY[i+1] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[i] >> 5)) * 4;
		b = (CAMERA_BUFFER_ARRAY[i] & 0x1F) * 8;
		
		//gray_array[i/2] = (r * 299 + g * 587 + b * 114 + 500) / 1000;
		gray_array[IMG_WIDTH*IMG_HEIGHT - i/2 - 1] = (r * 299 + g * 587 + b * 114 + 500) / 1000;
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

//�����ݴ��͵�����˿�
void Data_Output(u8 ch)
{	
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

//��ʾͼ�����ɽ��๦�ܵ�������
void Display_Result(void)
{
	uint32_t i;
	uint8_t ch;
	
	//���Ͱ�ͷ
	ch = 0x02;
	Data_Output(ch);

	ch = 0xFD;
	Data_Output(ch);

	
	//����ͼ��
	for(i = 0 ; i<IMG_HEIGHT*IMG_WIDTH; i++ )
	{		
		ch = result_array[i];
		Data_Output(ch);
	}
	
	//���Ͱ�β
	ch = 0xFD;
	Data_Output(ch);
	
	ch = 0x02;
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
	
	//��������fps
	float_char(processing_fps,a);
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

void Send_Parameter_Mode(void)
{
	uint8_t ch;
	
	ch = 0x05;
	Data_Output(ch);
	ch = 0xFA;
	Data_Output(ch);
	
	//����mode
	ch = mode;
	Data_Output(ch);
	
	//���Ͱ�β
	ch = 0xFA;
	Data_Output(ch);
	ch = 0x05;
	Data_Output(ch);
}

void Data_Output_Ctrl(unsigned char cmd)
{
	switch(cmd)
	{
		case 1:
			flag_Image = 1;
			break;
		case 2:
			flag_Image = 0;
			break;
		case 3:
			flag_Result = 1;
			break;
		case 4:
			flag_Result = 0;
			break;
		case 5:
			flag_Wave = 1;
			break;
		case 6:
			flag_Wave = 0;
			break;
		case 7:
			flag_Fps = 1;
			break;
		case 8:
			flag_Fps = 0;
			break;
		case 9:
			flag_Sd_original = 1;
			break;
		case 10:
			flag_Sd_original = 0;
			break;
		default:
			break;
	}
}

void Mode_Set(void)	//ģʽ���ú������ڳ�ʼ�������к��л�ģʽʱ���е���
{
	switch(mode)
	{
		case 0:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_original = 0;
			flag_Sd_gray = 1;
			flag_Sd_result = 0;
		break;
		
		case 1:
			flag_Image = 1;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_original = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
		break;
		
		case 2:
			flag_Image = 0;
			flag_Result = 1;
			flag_Wave = 0;
			flag_Sd_original = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
		break;
		
		case 3:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 1;
			flag_Sd_original = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
		break;
		
		case 4:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_original = 1;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
		break;
		
		case 5:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_original = 0;
			flag_Sd_gray = 1;
			flag_Sd_result = 0;
		break;
		
		case 6:
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_original = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 1;
		break;
		
		default:
			break;
	}
}

void Mode_Change(void)	//�ڰ����ж��е���
{
	mode++;
	if(mode>6)
	{
		mode = 0;
	}
	
	Mode_Set();

}

void Image_Output(u8 mode)	//mode 0--����֮ǰ���ã�1--����֮����ã�ԭͼ�����������ͬʱ���䣬�����ͼֻ��������������䣩
{
	//*******************************************************************
	//�����Ϣ
	
	if(!mode)
	{
		#if defined(__DISPLAY_IMAGE)
			if(flag_Image)
				Display_Image();	//�Ӵ������ͼ�����ɽ��๦�ܵ���������ʾ
		
		#endif
			
		#if defined(__PARAMETER_FPS)
		
			if(flag_Fps)
				Send_Parameter_Fps();
		
		#endif
			
		#if defined(__PARAMETER_MODE)
			
			if(flag_Mode)
				Send_Parameter_Mode();
		
		#endif
	}
	else
	{
		#if defined(__DISPLAY_RESULT)
		
			if(flag_Result)
				Display_Result();	//�Ӵ������ͼ�����ɽ��๦�ܵ���������ʾ
		
		#endif
		
			
		#if defined(__DISPALY_WAVE)
		
			if(flag_Wave)
				Display_Wave();	//�����������
		
		#endif
		
			
		#ifdef __SD_SAVE		//SD��ͼ

			if(SD_State)	//���SD�����سɹ�
			{
				if(flag_Sd_gray)
					TO_SDcard_OneFile(1);		//�Ҷ�ͼ
				
				if(flag_Sd_result)
					TO_SDcard_OneFile(2);		//�Ҷ�ͼ
			}

		#endif
	}

}

//*********************** ��ִ�к��� **********************************************

uint8_t image_updata_flag = 0;		//��ͼ��ɼ���ɱ�־  0����ͼû�вɼ����    1����ͼ�ɼ����
uint8_t processing_ready = 1;	//�״���1����Ϊһ��ʼ��ʱ��û��ͼ�񣬵�һ���޷���������
void Image_Process(void)
{

	//�ȴ���ͼ
	while(!image_updata_flag){}
	
	image_updata_flag = 0;	//��ͼ�Ѿ���ʼ��ʹ�ã���ͼ��־����

	processing_fps_temp++;	//��������֡��
	
	Creat_Gray();	//���ɻҶȾ�������������ʾ����
	
	//�������ʼ���
	if(Task_Delay[1]==0)
	{
		Task_Delay[1]=5000; //��ֵÿ1ms���1������0�ſ������½�������
		
		processing_fps = processing_fps_temp/5.0f;	//���㵱ǰ����֡��
		processing_fps_temp =0;						//����
	}

	Image_Output(0);	//���������ԭʼͼ��������ݣ�
	Image_Fix();		//ͼ������
	Image_Output(1);	//�����������������ݣ�

	processing_ready = 1;	//���������1

}

