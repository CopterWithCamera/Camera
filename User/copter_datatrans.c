#include "copter_datatrans.h"
#include "./usart/bsp_debug_usart.h"
#include "image_processing.h"

#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)	  ) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )

//�߶����ݣ���λmm
float height_ultra = 0;		//ultra.relative_height*10
float height_LPF = 0;		//sonar.displacement
float height_fusion = 0;	//sonar_fusion.fusion_displacement.out

//��̬���ݣ���λΪ��
float roll = 0;
float pitch = 0;
float yaw = 0;

//�����ݴ�����
unsigned char Tmp_Buffer[20];

void Get_Attitude(void)
{
	roll  = *((float*)(&(Tmp_Buffer[0])));
	pitch    = *((float*)(&(Tmp_Buffer[4])));
	yaw = *((float*)(&(Tmp_Buffer[8])));
}

void Get_Height(void)
{
	height_ultra  = *((float*)(&(Tmp_Buffer[0])));
	height_LPF    = *((float*)(&(Tmp_Buffer[4])));
	height_fusion = *((float*)(&(Tmp_Buffer[8])));
	
//	printf("%.1f  %.1f   %.1f\r\n", height_ultra, height_LPF, height_fusion);
}

u8 counter = 0;
void Camera_Receive_Handle(unsigned char data)
{
	static u8 mode = 0;
	
	switch(mode)
	{
		case 0:
			if(data == 0xAA)
				mode = 1;
			else
				mode = 0;
			break;
			
		case 1:
			if(data == 0xAA)
				mode = 2;
			else
				mode = 0;
			break;
			
		case 2:
			if(data == 0x01)	//���빦����1����
			{
				mode = 10;
				counter = 0;
			}
			else if(data == 0x02)	//���빦����2����
			{
				mode = 11;
				counter = 0;
			}
			else				//û�ж�Ӧ������
			{
				mode = 0;
			}
			break;
		
		case 10:
			Tmp_Buffer[counter] = data;	//3*4�ֽڣ��ܹ�ռ������0-11λ
			counter++;
			if(counter>=12)
			{
				Get_Height();	//�߶����ݻ�ȡ���
				mode = 0;
			}
		break;
			
		case 11:
			Tmp_Buffer[counter] = data;	//3*4�ֽڣ��ܹ�ռ������0-11λ
			counter++;
			if(counter>=12)
			{
				Get_Attitude();	//�߶����ݻ�ȡ���
				mode = 0;
			}
		break;
			
		default:
			mode = 0;
		break;
	}
}

//=======================================================================================================================

unsigned char Data_Buffer[20];

void Send_to_Copter(unsigned char *DataToSend ,u8 data_num)
{
	u16 i;
	
	for(i = 0;i<data_num;i++)
	{
		USART2_Send(DataToSend[i]);
	}
}

//����λ����Ϣ
void Camera_Send_Position(void)
{
	float tmp_f;
	
	u8 cnt = 0;
	
	//֡ͷ
	Data_Buffer[cnt++] = 0xAA;	
	Data_Buffer[cnt++] = 0xAF;
	
	//������
	Data_Buffer[cnt++] = 0x01;
	
	//����
	tmp_f = length;							//ƫ��
	Data_Buffer[cnt++] = BYTE0(tmp_f);
	Data_Buffer[cnt++] = BYTE1(tmp_f);
	Data_Buffer[cnt++] = BYTE2(tmp_f);
	Data_Buffer[cnt++] = BYTE3(tmp_f);
	
	tmp_f = angle;							//�Ƕ�
	Data_Buffer[cnt++] = BYTE0(tmp_f);
	Data_Buffer[cnt++] = BYTE1(tmp_f);
	Data_Buffer[cnt++] = BYTE2(tmp_f);
	Data_Buffer[cnt++] = BYTE3(tmp_f);
	
	tmp_f = speed;							//�ٶ�
	Data_Buffer[cnt++] = BYTE0(tmp_f);
	Data_Buffer[cnt++] = BYTE1(tmp_f);
	Data_Buffer[cnt++] = BYTE2(tmp_f);
	Data_Buffer[cnt++] = BYTE3(tmp_f);
	
	Send_to_Copter(Data_Buffer,cnt);
}

//����״̬��Ϣ
void Camera_Send_Status(void)
{
	float tmp_f;
	
	u8 cnt = 0;
	
	//֡ͷ
	Data_Buffer[cnt++] = 0xAA;	
	Data_Buffer[cnt++] = 0xAF;
	
	//������
	Data_Buffer[cnt++] = 0x02;
	
	//����
	tmp_f = fps;							//��ͼ����
	Data_Buffer[cnt++] = BYTE0(tmp_f);
	Data_Buffer[cnt++] = BYTE1(tmp_f);
	Data_Buffer[cnt++] = BYTE2(tmp_f);
	Data_Buffer[cnt++] = BYTE3(tmp_f);
	
	tmp_f = processing_fps;					//��������
	Data_Buffer[cnt++] = BYTE0(tmp_f);
	Data_Buffer[cnt++] = BYTE1(tmp_f);
	Data_Buffer[cnt++] = BYTE2(tmp_f);
	Data_Buffer[cnt++] = BYTE3(tmp_f);
	
	tmp_f = 0.0f;							//��
	Data_Buffer[cnt++] = BYTE0(tmp_f);
	Data_Buffer[cnt++] = BYTE1(tmp_f);
	Data_Buffer[cnt++] = BYTE2(tmp_f);
	Data_Buffer[cnt++] = BYTE3(tmp_f);
	
	Send_to_Copter(Data_Buffer,cnt);
}

//��ɿط�������
//ÿ����������Զ����ô˺���������Ϣ
void Camera_Data_Send(void)
{
	Camera_Send_Position();		//����λ����Ϣ��ƫ�ơ��Ƕȡ��ٶȣ�
	Camera_Send_Status();
}
