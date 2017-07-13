#include "copter_datatrans.h"
#include "./usart/bsp_debug_usart.h"

//�߶����ݣ���λmm
float height_ultra = 0;		//ultra.relative_height*10
float height_LPF = 0;		//sonar.displacement
float height_fusion = 0;	//sonar_fusion.fusion_displacement.out

//�����ݴ�����
unsigned char Tmp_Buffer[20];

void Get_Height(void)
{
	height_ultra = *((float*)(&(Tmp_Buffer[0])));
	height_LPF = *((float*)(&(Tmp_Buffer[4])));
	height_fusion = *((float*)(&(Tmp_Buffer[8])));
}

u8 counter = 0;
void Copter_Receive_Handle(unsigned char data)
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
			
		default:
			mode = 0;
		break;
	}
}


