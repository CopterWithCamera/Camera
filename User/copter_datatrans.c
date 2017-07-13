#include "copter_datatrans.h"
#include "./usart/bsp_debug_usart.h"

//高度数据，单位mm
float height_ultra = 0;		//ultra.relative_height*10
float height_LPF = 0;		//sonar.displacement
float height_fusion = 0;	//sonar_fusion.fusion_displacement.out

//数据暂存数组
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
			if(data == 0x01)	//进入功能字1解码
			{
				mode = 10;
				counter = 0;
			}
			else if(data == 0x02)	//进入功能字2解码
			{
				
			}
			else				//没有对应功能字
			{
				mode = 0;
			}
			break;
		
		case 10:
			Tmp_Buffer[counter] = data;	//3*4字节，总共占用数组0-11位
			counter++;
			if(counter>=12)
			{
				Get_Height();	//高度数据获取完成
				mode = 0;
			}
		break;
			
		default:
			mode = 0;
		break;
	}
}


