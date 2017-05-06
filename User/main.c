/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   OV5640����ͷ��ʾ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� STM32  F429������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"

#include "include.h"

#include "./usart/bsp_debug_usart.h"
#include "./sdram/bsp_sdram.h"
#include "./lcd/bsp_lcd.h"
#include "./camera/bsp_ov5640.h"
#include "./systick/bsp_SysTick.h"
#include "./camera/ov5640_AF.h"
#include "./key/bsp_exti.h"
#include "image_processing.h"
#include "ff.h"
#include "rgbTObmp.h"

/*���������*/
uint32_t Task_Delay[NumOfTask];

uint8_t dispBuf[100];
OV5640_IDTypeDef OV5640_Camera_ID;

uint8_t fps=0;

void My_Camera_Init(void)
{
//�������LCD_DISPLAY��include.h�У����ͱ���LCD����
#ifdef LCD_DISPLAY
	
	printf("\r\nSTM32F429 DCMI ����OV5640����\r\n");
	LCD_DisplayStringLine_EN_CH(LINE(19),(uint8_t*) "OV5640ͼ������ԣ�Ӳ���汾���ɰ�F429");
	
#endif
	
	/* ��ʼ������ͷGPIO��IIC */
	OV5640_HW_Init();   

	/* ��ȡ����ͷоƬID��ȷ������ͷ�������� */
	OV5640_ReadID(&OV5640_Camera_ID);

	if(OV5640_Camera_ID.PIDH  == 0x56)
	{
//		sprintf((char*)dispBuf, "              OV5640 ����ͷ,ID:0x%x", OV5640_Camera_ID.PIDH);
//		LCD_DisplayStringLine_EN_CH(LINE(0),(uint8_t*)dispBuf);
		CAMERA_DEBUG("%x %x\r\n",OV5640_Camera_ID.PIDH ,OV5640_Camera_ID.PIDL);

	}
	else
	{
		//�������LCD_DISPLAY��include.h�У����ͱ���LCD����
		#ifdef LCD_DISPLAY
		
		LCD_SetTextColor(LCD_COLOR_RED);
		LCD_DisplayStringLine_EN_CH(LINE(0),(uint8_t*) "         û�м�⵽OV5640�������¼�����ӡ�");
		CAMERA_DEBUG("û�м�⵽OV5640����ͷ�������¼�����ӡ�\r\n");
		
		#endif
		
		CAMERA_DEBUG("δ��⵽����ͷ\r\n");

		while(1);  
	}


	OV5640_Init();			//DCMI  DMA  INTERRUPT

	OV5640_RGB565Config();
//	OV5640_AUTO_FOCUS();

	//ʹ��DCMI�ɼ�����
	DCMI_Cmd(ENABLE); 
	DCMI_CaptureCmd(ENABLE); 	
}


void My_LCD_Init(void)
{
	/*��ʼ��Һ����*/
	LCD_Init();
	LCD_LayerInit();		//��ʾ�����ã������Դ��ַ����
	LTDC_Cmd(ENABLE);
	
	/*�ѱ�����ˢ��ɫ*/
	LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BLACK);
	
	/*��ʼ����Ĭ��ʹ��ǰ����*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*Ĭ�����ò�͸��	���ú�������Ϊ��͸���ȣ���Χ 0-0xff ��0Ϊȫ͸����0xffΪ��͸��*/
	LCD_SetTransparency(0xFF);
	LCD_Clear(TRANSPARENCY);
	
//	LCD_SetColors(LCD_COLOR_RED,TRANSPARENCY);

//	LCD_ClearLine(LINE(18));
//  LCD_DisplayStringLine_EN_CH(LINE(18),(uint8_t* )" ģʽ:WVGA 800x480");
	
}


////���������SDRAM
//uint32_t testValue __EXRAM =7 ;
////�������鵽SDRAM
//uint8_t testGrup[3] __EXRAM ={1,2,3};

////���������SRAM
//uint32_t testValue2  =7 ;
////�������鵽SRAM
//uint8_t testGrup2[3] ={1,2,3};

void My_RAM_TEST(void)
{
//	uint32_t inerTestValue =10;
//	char ch;

//	printf("\r\nRAM������Գ���\r\n");
	
//	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&inerTestValue,inerTestValue);
	
//	ch = testValue & 0xFF;
//	USART_SendData(DEBUG_USART, (uint8_t) ch);
//	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
//	
//	ch = (testValue>>8) & 0xFF;
//	USART_SendData(DEBUG_USART, (uint8_t) ch);
//	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
//	
//	ch = (testValue>>16) & 0xFF;
//	USART_SendData(DEBUG_USART, (uint8_t) ch);
//	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
//	
//	ch = (testValue>>24) & 0xFF;
//	USART_SendData(DEBUG_USART, (uint8_t) ch);
//	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
//	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&testValue,testValue);
//	printf("��������ĵ�ַΪ��0x,����ֵΪ��%d,%d,%d\r\n",testGrup[0],testGrup[1],testGrup[2]);
//	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&testValue2,testValue2);
//	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d��%d,%d\r\n",(uint32_t)&testGrup2,testGrup2[0],testGrup2[1],testGrup2[2]);
	
}

//����WiFiģ�飨��sd���������wifi��ԭ������
void BL8782_PDN_INIT(void)
{
  /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOG, ENABLE); 							   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);	
  
  GPIO_ResetBits(GPIOG,GPIO_Pin_9);  //����WiFiģ��
}

//SD����ʼ������

u8 SDCard_Init(void)
{
	FATFS fs;					/* Work area (file system object) for logical drives */
	FRESULT res_sd_conf; 
	
	/*����wifiģ��*/
	BL8782_PDN_INIT();
	
	//SD������
	res_sd_conf = f_mount(&fs,"0:",1);  //�����ļ�ϵͳ
	
	//���û���ļ�ϵͳ����Ҫ��ʽ�����ٹ���
	if(res_sd_conf == FR_NO_FILESYSTEM)
	{
		printf("��SD����û���ļ�ϵͳ���������и�ʽ��...\r\n");
		/* ��ʽ�� */
		res_sd_conf=f_mkfs("0:",0,0);							
		
		if(res_sd_conf == FR_OK)
		{
			printf("��SD���ѳɹ���ʽ���ļ�ϵͳ��\r\n");
			/* ��ʽ������ȡ������ */
			res_sd_conf = f_mount(NULL,"0:",1);			
			/* ���¹���	*/			
			res_sd_conf = f_mount(&fs,"0:",1);
		}
		else
		{
			printf("������ʽ��ʧ�ܡ�����\r\n");
			return 0;	//SD������ʧ��
		}
	}
	else
	{
		printf("��SD�����سɹ�\r\n");
	}
	return 1;	//SD�����سɹ�
}


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */

int flag = 0;	//���ڰ����ж�
int SD_State = 0;	//SD��״̬ 0 -- ����ʧ��  1 -- ���سɹ�
int main(void)
{
	/*����ͷ��RGB LED�ƹ������ţ���Ҫͬʱʹ��LED������ͷ*/

	Debug_USART_Config();   
	
	/* ����SysTick Ϊ1ms�ж�һ��,ʱ�䵽�󴥷���ʱ�жϣ�
	*����stm32fxx_it.c�ļ���SysTick_Handler����ͨ�����жϴ�����ʱ
	*/
	
	SysTick_Init();		//������ʱ����
	
	EXTI_Key_Config();	//��ʼ���ⲿ�жϰ���

	//�������LCD_DISPLAY��include.h�У����ͱ���LCD����
	#ifdef LCD_DISPLAY
		
		My_LCD_Init();		//��ʼ��LCD������DMA2D��ʼ����
		
	#endif
	
	My_Camera_Init();	//��ʼ��OV5640
	
	//�������LCD_DISPLAY��include.h�У����ͱ���LCD����
	#ifdef LCD_DISPLAY
			
		DMA2_Stream0_Init();	//DMA2_Stream0��ʼ�� ������ -> �Դ棨�˴�ֻ�ǳ�ʼ������û�п�������
		
	#endif
	
	SD_State = SDCard_Init();	//��ʼ��SD��  0 -- ����ʧ��  1 -- ���سɹ�
	
	/*DMAֱ�Ӵ�������ͷ���ݵ�LCD��Ļ��ʾ*/
	while(1)
	{	
		//ȫ�ٸ���
		if(flag == 0)
		{
			Image_Process();	//ͼ��������������ͼ��д���Դ�
		}
		
		
		//�������LCD_DISPLAY��include.h�У����ͱ���LCD����
		#ifdef LCD_DISPLAY
			
			//��ʾ֡�ʣ�Ĭ�ϲ���ʾ		
			#if FRAME_RATE_DISPLAY	
			
			if(Task_Delay[0]==0)
			{
							
				LCD_SetColors(LCD_COLOR_RED,TRANSPARENCY);

				/*���֡��*/
	//			LCD_ClearLine(LINE(17));
				sprintf((char*)dispBuf, "      ");
				LCD_DisplayStringLine_EN_CH(LINE(17),dispBuf);
				sprintf((char*)dispBuf, " %.1f/s", (float)(fps/5.0));
				LCD_DisplayStringLine_EN_CH(LINE(17),dispBuf);
				
				//����
				fps =0;
				
				Task_Delay[0]=5000; //��ֵÿ1ms���1������0�ſ������½�������
			}
				
			#endif
			
		#endif
		
	}

}




/*********************************************END OF FILE**********************/

//		//��ʱ����ͼ�����ڲ��ԣ�
//		if(Task_Delay[1]==0 && flag == 0)
//		{
//			//DCMI_CaptureCmd(ENABLE);
//			
//			Image_Process();	//ͼ��������������ͼ��д���Դ�
//			
//			Task_Delay[1]=100; //��ֵÿ1ms���1������0�ſ������½�������
//		}
