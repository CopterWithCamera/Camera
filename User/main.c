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
#include "./camera/bsp_ov5640.h"
#include "./systick/bsp_SysTick.h"
#include "./camera/ov5640_AF.h"
#include "./key/bsp_exti.h"
#include "image_processing.h"
#include "ff.h"
#include "rgbTObmp.h"
#include "bsp_spi_nrf.h"
#include "copter_datatrans.h"

/*						*\
		���������
		
0 ��ͼ֡��
1 ����֡��
2 ����д��ģʽ�¶�ʱ����SD���ļ�
3
4
5
6
7
8
9

\*						*/

uint32_t Task_Delay[NumOfTask];

//֡�ʼ���
uint8_t fps_temp = 0;
float fps = 0;
uint8_t processing_fps_temp = 0;
float processing_fps = 0;

void My_Camera_Init(void)
{
	OV5640_IDTypeDef OV5640_Camera_ID;
	
	/* ��ʼ������ͷGPIO��IIC */
	OV5640_HW_Init();   		//ͬF407������ͬ�����ظ���

	/* ��ȡ����ͷоƬID��ȷ������ͷ�������� */
	OV5640_ReadID(&OV5640_Camera_ID);

	if(OV5640_Camera_ID.PIDH  == 0x56)
	{
		CAMERA_DEBUG("%x %x\r\n",OV5640_Camera_ID.PIDH ,OV5640_Camera_ID.PIDL);
	}
	else
	{
		CAMERA_DEBUG("δ��⵽����ͷ\r\n");
		while(1);  
	}

	OV5640_Init();			//DCMI  DMA  INTERRUPT
	OV5640_RGB565Config();
	OV5640_AUTO_FOCUS();
}


//���������SDRAM
uint32_t testValue __EXRAM =7 ;
//�������鵽SDRAM
uint8_t testGrup[4] __EXRAM ={1,2,3,4};

//���������SRAM
uint32_t testValue2  =7 ;
//�������鵽SRAM
uint8_t testGrup2[3] ={1,2,3};

void My_RAM_TEST(void)
{
	uint32_t inerTestValue =10;
	char ch;

	printf("\r\n RAM������Գ��� \r\n\r\n");
	
	ch = testValue & 0xFF;
	USART_SendData(DEBUG_USART, (uint8_t) ch);
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
	
	ch = (testValue>>8) & 0xFF;
	USART_SendData(DEBUG_USART, (uint8_t) ch);
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
	
	ch = (testValue>>16) & 0xFF;
	USART_SendData(DEBUG_USART, (uint8_t) ch);
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
	
	ch = (testValue>>24) & 0xFF;
	USART_SendData(DEBUG_USART, (uint8_t) ch);
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
	
	printf("0\r\n");
	printf("�ֲ����������ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&inerTestValue,inerTestValue);
	
	printf("1\r\n");
	printf("ȫ��SDRAM�����ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&testValue,testValue);
	
	printf("2\r\n");
	printf("ȫ��SDRAM���飺���ĵ�ַΪ��0x%x,����ֵΪ��%d,%d,%d\r\n",(uint32_t)testGrup,testGrup[0],testGrup[1],testGrup[2]);
	
	printf("3\r\n");
	printf("ȫ��RAM�����ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&testValue2,testValue2);
	
	printf("4\r\n");
	printf("ȫ��RAM���飺���ĵ�ַΪ��0x%x,����ֵΪ��%d��%d,%d\r\n",(uint32_t)&testGrup2,testGrup2[0],testGrup2[1],testGrup2[2]);
	
	printf("\r\n");
	printf("ȫ��SDRAM�����ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&testValue,testValue);
	printf("ȫ��SDRAM���飺���ĵ�ַΪ��0x%x,����ֵΪ��%d,%d,%d\r\n",(uint32_t)testGrup,testGrup[0],testGrup[1],testGrup[2]);

	printf("ȫ��CAMERA_BUFFER_ARRAY1�����ĵ�ַΪ��0x%x\r\n",(uint32_t)&CAMERA_BUFFER_ARRAY1);
	printf("ȫ��CAMERA_BUFFER_ARRAY2�����ĵ�ַΪ��0x%x\r\n",(uint32_t)&CAMERA_BUFFER_ARRAY2);
	
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
//	if(res_sd_conf == FR_NO_FILESYSTEM)
	if(res_sd_conf != FR_OK)				//ֻҪ����FR_OK��ֱ�ӷ���SD��״̬���󣬲����д�����
	{
		printf("SD������ʧ��\r\n");
		return 0;
	}

	printf("SD�����سɹ�\r\n");
	
	return 1;	//SD�����سɹ�
}


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */

extern  uint32_t CurrentFrameBuffer;

int SD_State = 0;	//SD��״̬ 0 -- ����ʧ��  1 -- ���سɹ�
int main(void)
{
	/*����ͷ��RGB LED�ƹ������ţ���Ҫͬʱʹ��LED������ͷ*/
	
	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);	//�趨�ж����ȼ�����
	
	Debug_USART_Config();	//����1
	USART2_Config();		//����2
	SysTick_Init();		//������ʱ����
	My_RAM_TEST();		//�ڴ�����������
	EXTI_Key_Config();	//��ʼ���ⲿ�жϰ���

	My_Camera_Init();	//��ʼ��OV5640
	
	#ifdef __NRF_DISPLAY
		NRF24L01_Init();	//NRF24L01
	#endif
	
	#ifdef __SD_SAVE
		SD_State = SDCard_Init();	//��ʼ��SD��  0 -- ����ʧ��  1 -- ���سɹ�
	#endif
	
	//��������
	OV5640_DMA_Config((uint32_t)DCMI_IN_BUFFER_ARRAY,img_height*img_width*2/4);	//������һ�δ���
	DCMI_Cmd(ENABLE);
	DCMI_CaptureCmd(ENABLE);
	
	Mode_Set();	//�������ݴ���ģʽ
	
	/*DMAֱ�Ӵ�������ͷ���ݵ�LCD��Ļ��ʾ*/
	while(1)
	{
		Image_Process();	//ͼ��������������ͼ��д���Դ�
		
		//֡�ʼ���
		if(Task_Delay[0]==0)
		{
			Task_Delay[0]=5000; //��ֵÿ1ms���1������0�ſ������½�������
			
			fps = fps_temp/5.0f;
			fps_temp =0;	//����
		}
		
		//����д��SD��ģʽ�¶�ʱ�����ļ�����
		if(Task_Delay[2]==0)
		{
			Task_Delay[2]=1000; //��ֵÿ1ms���1������0�ſ������½�������
			
			if(ToOneFile_StartFlag)
				f_sync(&ToOneFile_f);
			
			if(Height_StartFlag)
				f_sync(&Height_f);
		}
	}
}

/*********************************************END OF FILE**********************/

