#include "image_processing.h"
#include "./usart/bsp_debug_usart.h"
#include "math.h"
#include "rgbTObmp.h"
#include "bsp_spi_nrf.h"
#include "image_fix.h"
#include "copter_datatrans.h"
#include "./buzzer/bsp_buzzer.h"

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
uint8_t gray_column_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//����������

//�������洢�ռ�
uint8_t result_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//����*���*1�ֽ�
uint8_t result_column_array[IMG_WIDTH*IMG_HEIGHT] __EXRAM;	//����������

//�������
float length;	//ƫ��
float angle;
float length_pitch;

//ƥ����
float tracking_state = 0;	//����״̬	0 -- ʧ��	1 -- �ɹ�

//����ģʽ
unsigned char mode = 0;		//mode���������

//���ƴ����flag
u8 flag_Image = 0;
u8 flag_Result = 0;
u8 flag_Wave = 0;
u8 flag_Sd_gray = 0;
u8 flag_Sd_result = 0;

//��������flag
u8 flag_Fps = 0;
u8 flag_Mode = 0;

//���ɻҶȾ���
void Creat_Gray(void)
{
	uint32_t r,g,b;
	
	uint16_t i;
	
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

//ת��Ϊ����������
void Creat_Column(void)	//��ͼ�����Խǵ�λ�ã�����gray_across_array
{
	uint16_t i,j;
	
	for (i = 0; i < 48; i++)
	{
		for (j = 0; j < 80; j++)
		{
			gray_column_array[j * 48 + i] = gray_array[i * 80 + j];
		}
	}
}

//����������ָ�Ϊ�о���
void Column_To_Line(void)	//��ͼ�����Խǵ�λ�ã�����gray_across_array
{
	uint16_t i,j;
	
	for (i = 0; i < 80; i++)
	{
		for (j = 0; j < 48; j++)
		{
			result_array[j * 80 + i] = gray_column_array[i * 48 + j];
		}
	}
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

//������Σ�length��length_pitch��angle��������ɽ��๦�ܵ������ֲ鿴
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
	float_char(length_pitch,a);
	ch = a[0];
	Data_Output(ch);
	ch = a[1];
	Data_Output(ch);
	ch = a[2];
	Data_Output(ch);
	ch = a[3];
	Data_Output(ch);
	
	//����ͨ����
	float_char(angle,a);
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

void Mode_Set(void)	//ģʽ���ú������ڳ�ʼ�������к��л�ģʽʱ���е���
{
	switch(mode)
	{
		case 0:
			//����ģʽ������������������
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
			flag_Fps = 0;
			flag_Mode = 0;
		break;
		
		case 1:
			//���ģʽ��ֻ��������״̬
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
			flag_Fps = 1;
			flag_Mode = 1;
		break;
		
		case 2:
			//ԭͼ
			flag_Image = 1;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
			flag_Fps = 1;
			flag_Mode = 1;
		break;
		
		case 3:
			//��Ҷ�ͼ
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_gray = 1;
			flag_Sd_result = 0;
			flag_Fps = 1;
			flag_Mode = 1;
		break;
		
		case 4:
			//����
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 1;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
			flag_Fps = 1;
			flag_Mode = 1;
		break;
		
		case 5:
			//�����ͼ
			flag_Image = 0;
			flag_Result = 1;
			flag_Wave = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 0;
			flag_Fps = 1;
			flag_Mode = 1;
		break;
		
		case 6:
			//����ͼ
			flag_Image = 0;
			flag_Result = 0;
			flag_Wave = 0;
			flag_Sd_gray = 0;
			flag_Sd_result = 1;
			flag_Fps = 1;
			flag_Mode = 1;
		break;
		
		default:
			break;
	}
}

//�ڰ����ж��е���
void Mode_Change(void)	
{
	mode++;
	if(mode>6)
	{
		mode = 0;
	}
	
	Mode_Set();
}

void Image_Output(u8 sendmode)	//mode 0--����֮ǰ���ã�1--����֮����ã�ԭͼ�����������ͬʱ���䣬�����ͼֻ��������������䣩
{
	//*******************************************************************
	//�����Ϣ
	
	if(!sendmode)
	{
		//���㿪ʼǰ�������ݣ�����Ҫ��������ݣ�
		
		//��ɿط�������
		
		
		//����λ������
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
		
		#ifdef __SD_SAVE		//SD��ͼ
			
			#if defined(__SD_SAVE_GRAY)
			
				if(SD_State)	//���SD�����سɹ�
				{
					if(flag_Sd_gray)
					{
						TO_SDcard_OneFile(1);		//�Ҷ�ͼ
						TO_SDcard_Height();			//��¼�߶�����
					}
				}
			
			#endif
			
		#endif
	}
	else
	{
		//������͵�����
	
		//����λ����������
		
		#if defined(__DISPLAY_RESULT)
		
			if(flag_Result)
				Display_Result();	//�Ӵ������ͼ�����ɽ��๦�ܵ���������ʾ
		
		#endif
		
			
		#if defined(__DISPALY_WAVE)
		
			if(flag_Wave)
				Display_Wave();	//�����������
		
		#endif
		
			
		#ifdef __SD_SAVE		//SD��ͼ

			#if defined(__SD_SAVE_RESULT)
			
				if(SD_State)	//���SD�����سɹ�
				{
					if(flag_Sd_result)
					{
						TO_SDcard_OneFile(2);		//�Ҷ�ͼ
						TO_SDcard_Height();			//��¼�߶�����
					}
				}
			
			#endif

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

	//�������ʼ���
	processing_fps_temp++;	//��������֡��	
	if(Task_Delay[1]==0)
	{
		Task_Delay[1]=5000; //��ֵÿ1ms���1������0�ſ������½�������
		
		processing_fps = processing_fps_temp/5.0f;	//���㵱ǰ����֡��
		processing_fps_temp =0;						//����
	}
	
	//����ͼ������
	Creat_Gray();	//���ɻҶȾ�������������ʾ����
	Creat_Column();	//ת��Ϊ����������

	Image_Output(0);	//���������ԭʼͼ��������ݣ�
	
//	if(mode == 0)
//	{
		//ֻ��ģʽ0�Ż���ɻ���������
		Image_Fix();		//ͼ������
		Camera_Data_Send();	//��ɿط�������          //��������������
		if(tracking_state)	//���ٳɹ�ʱ������������ʾ
		{
			BUZZER(ON);
		}
//	}
	
//	Column_To_Line();	//������������ָ�Ϊ�о���
//	Image_Output(1);	//�����������������ݣ�

	processing_ready = 1;	//���������1

}

