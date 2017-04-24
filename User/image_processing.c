#include "image_processing.h"
#include "./usart/bsp_debug_usart.h"
#include "math.h"

/*
 * ****** �ܹ�ʹ�õ���Դ *******
 * 
 * ��ʱ #define Delay(ms)  Delay_ms(ms) 
 *
 * ����ͷ�ɼ���ͼ���С����������������鳤�ȣ� extern uint16_t img_width, img_height;
 * 
 * �Դ��ַ�� LCD_FRAME_BUFFER
 *
 * ����ͷDMA2���÷�ʽ��
 * #define FSMC_LCD_ADD  RESS      LCD_FRAME_BUFFER
 * �� FSMC_LCD_ADDRESS �Ķ������ϣ������ͷ���ݴ�ĵط�������
 * �����Сһ��Ҫ�� img_width �� img_height ����
 * 
 */
 
//**************************************************************
 
//	ͼ���䷽ʽѡ��ֻ��ʹ��һ����
//	#define __USART_DISPLAY_IMAGE	//��ʾͼ��
//	#define __USART_DISPLAY_MATRIX	//��ʾ����
	#define __USART_DISPALY_WAVE	//��ʾ����
	
	#define __DISPALY_DATA
	#define __DISPALY_GRAPH
	
//**************************************************************	
	
//ͼ�񻺴����飬��һ����ԭͼ���ڶ����Ǵ�����ͼ
uint8_t CAMERA_BUFFER_ARRAY[2][ IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;	//����*���*2���ֽ�  *  2������

uint8_t gray_array[IMG_WIDTH*IMG_HEIGHT];	//��һ��Ҷȿռ䣬Ĭ���ṩ�Ҷ�����
uint8_t temp_array[IMG_WIDTH*IMG_HEIGHT];	//�ڶ���Ҷȿռ䣬��Ϊ������ʱ�洢�ռ�

float length;	//ƫ��
float speed;


//����ȡ���ݵĺ�����Get��ͷ
//���д����ݵĺ�����To��ͷ

//���ɻҶȾ���
void Creat_Gray()
{
	uint32_t r,g,b;
	
	uint16_t i;
	
	for(i=0;i<IMG_WIDTH*IMG_HEIGHT*2;i=i+2)
	{
		r = (CAMERA_BUFFER_ARRAY[0][i+1] >> 3) * 8;
		g = ((CAMERA_BUFFER_ARRAY[0][i+1] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[0][i] >> 5) * 4;
		b = (CAMERA_BUFFER_ARRAY[0][i] & 0x1F) * 8;
		
		gray_array[i/2] = (r * 299 + g * 587 + b * 114 + 500) / 1000;
	}
}

void Creat_LCD(void)
{
	uint32_t i;
	uint8_t rb,g;
	
	for(i = 0 ; i<IMG_HEIGHT*IMG_WIDTH; i++ )
	{
		rb = gray_array[i] / 8;
		g = gray_array[i] / 4;
		
		CAMERA_BUFFER_ARRAY[1][i*2+1] = (rb << 3) + ((g >> 3) & 0x07);	//�Ͱ�λ�ں��棬�߰�λ��ǰ��
		CAMERA_BUFFER_ARRAY[1][i*2] = (g << 5) + rb;
	}
}

//1.��ȡ����Ҷ�����ֵ
uint8_t Get_Gray(uint16_t row,uint16_t column)	//��row�У���column��
{
	//�����ķ�ʽ����ȷ����row�У���ȷ���ڱ����еĵ�column����ֵ��
	//�У�row      ��Χ��1 -- IMG_HEIGHT
	//�У�column   ��Χ��1 -- IMG_WIDTH
	
	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	return gray_array[num/2];
}

//2.�洢�������ݵ��ݴ���
void To_Temp(uint16_t row,uint16_t column,uint8_t gray)
{
	//�����ķ�ʽ����ȷ����row�У���ȷ���ڱ����еĵ�column����ֵ��
	//�У�row      ��Χ��1 -- IMG_HEIGHT
	//�У�column   ��Χ��1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	temp_array[num/2] = gray;
}


//3.��ȡ�����ݴ�����ֵ
uint8_t Get_Temp(uint16_t row,uint16_t column)
{
	//�����ķ�ʽ����ȷ����row�У���ȷ���ڱ����еĵ�column����ֵ��
	//�У�row      ��Χ��1 -- IMG_HEIGHT
	//�У�column   ��Χ��1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	return temp_array[num/2];
}

//4.�洢�������ݵ��Ҷ���
void To_Gray(uint16_t row,uint16_t column,uint8_t gray)
{
	//�����ķ�ʽ����ȷ����row�У���ȷ���ڱ����еĵ�column����ֵ��
	//�У�row      ��Χ��1 -- IMG_HEIGHT
	//�У�column   ��Χ��1 -- IMG_WIDTH

	uint32_t num;
	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
	gray_array[num/2] = gray;
}



void Image_Fix(void)	//ͼ���㷨
{
	uint32_t i,j;
	static float k = 0;

	//�Ҷ� --> �ݴ�
	for(i = 1;i<=IMG_HEIGHT;i++)
	{
		for(j = 1;j<=IMG_WIDTH;j++)
		{
			To_Temp(i,j,Get_Gray(i,j));
		}
	}
	
	//�ݴ� --> �Ҷ�
	for(i = 1;i<=IMG_HEIGHT;i++)
	{
		for(j = 1;j<=IMG_WIDTH;j++)
		{
			To_Gray(i,j,Get_Temp(i,j));
		}
	}
	
	k = k + 0.1f;
	if(k >= 314)
		k = 0;
	
	length = (float)sin((double)k) * 50.0f;
	speed = (float)cos((double)k) * 50.0f;
	

	
}

//�Ӵ�����ʾͼ�����ɽ��๦�ܵ�������
void Usart_Display_Image(void)
{
	uint32_t i;
	uint8_t ch;
	
	//���Ͱ�ͷ
	ch = 0x01;
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */
	ch = 0xFE;
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */	
	
	//����ͼ��
	for(i = 0 ; i<IMG_HEIGHT*IMG_WIDTH; i++ )
	{		
		ch = gray_array[i];
		
		//�Ӵ��ڷ���1byte
		USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */
	}
	
	//���Ͱ�β
	ch = 0xFE;
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */
	ch = 0x01;
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */
	
}


//�Ӵ�����ʾ����ֱ���ô��ڵ������ֲ鿴
void Usart_Display_Matrix(void)
{
	uint32_t i,j;
	uint8_t ch;
	
	//����ͼ��
	for(i = 0 ; i<IMG_HEIGHT; i++ )	//��ɨ��
	{
		for(j = 0;j<IMG_WIDTH;j++)	//��ɨ��
		{
			ch = gray_array[i];
		
			printf("%03d ",ch);	//ǿ����ʾ��λ����ǰ�油0
		}
		printf("\r\n");	//ÿ�н����س�
	}
	printf("\r\n\r\n\r\n\r\n");	//ȫ�����ͽ������4��
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

//����������Σ�length��speed������ɽ��๦�ܵ������ֲ鿴
void Usart_Display_Wave(void)
{
	uint8_t ch;
	unsigned char a[4];
	
	//���Ͱ�ͷ
	ch = 0x03;
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */
	ch = 0xFC;
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */
	
	//��������
	
	//����ͨ��һ
	float_char(length,a);
	ch = a[0];
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */
	ch = a[1];
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */	
	ch = a[2];
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */	
	ch = a[3];
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */	
	
	//����ͨ����
	float_char(speed,a);
	ch = a[0];
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */
	ch = a[1];
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */	
	ch = a[2];
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */	
	ch = a[3];
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */		
	
	//���Ͱ�β
	ch = 0xFC;
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */
	ch = 0x03;
	USART_SendData(DEBUG_USART, ch);		/* ����һ���ֽ����ݵ�����DEBUG_USART */
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	/* �ȴ�������� */
	
	
}


//�������LCD_DISPLAY��include.h�У����ͱ���LCD����
#ifdef LCD_DISPLAY

//��DMA��ʽ��ʾ
void Camera_Buffer_To_Lcd_Buffer(void)
{
	int i,j;
	
	//ԭͼ��
	for(i = 0;i<IMG_HEIGHT;i++)
	{
		for(j = 0;j<IMG_WIDTH*2;j=j+1)
		{
			LCD_FRAME_BUFFER_ARRAY[i*1600+j] = CAMERA_BUFFER_ARRAY[0][i*IMG_WIDTH*2+j];
		}
			
	}
	
	//�����ͼ��
	for(i = 0;i<IMG_HEIGHT;i++)
	{
		for(j = 0;j<IMG_WIDTH*2;j=j+1)
		{
			LCD_FRAME_BUFFER_ARRAY[1600*140 + i*1600+j] = CAMERA_BUFFER_ARRAY[1][i*IMG_WIDTH*2+j];
		}
			
	}
	
	
	//800*480
//	for(i=0;i<IMG_WIDTH*IMG_HEIGHT*2;i++)
//	{
//		LCD_FRAME_BUFFER_ARRAY[i] = CAMERA_BUFFER_ARRAY[0][i];
//	}
	
}


void DMA_AtoB_Config(uint32_t DMA_Memory_A_Addr,uint32_t DMA_Memory_B_Addr)
{
  DMA_InitTypeDef  DMA_InitStructure;
  
  /* ʹ��DMAʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  /* ��λ��ʼ��DMA������ */
  DMA_DeInit(DMA2_Stream0);
  /* ȷ��DMA��������λ��� */
  while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){}

  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = DMA_Memory_A_Addr;
  DMA_InitStructure.DMA_Memory0BaseAddr = DMA_Memory_B_Addr;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
  DMA_InitStructure.DMA_BufferSize = IMG_WIDTH;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh ;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;     
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC8;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(DMA2_Stream0, &DMA_InitStructure); 
		
  DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);
		
  DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE); 
		
  DMA_Cmd(DMA2_Stream0, ENABLE);
  while(DMA_GetCmdStatus(DMA2_Stream1) != ENABLE){}
  
}

void DMA2_Stream0_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	/* �����ж� */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* �����ж�Դ */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn ;//DMA�������ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE); 	
	
}

//��ʾƫ�ƾ����ˮƽ�ٶ�
void Display_data(void)
{	
	LCD_SetColors(LCD_COLOR_WHITE,TRANSPARENCY);
//	LCD_ClearLine(LINE(11));
//	LCD_ClearLine(LINE(14));
	sprintf((char*)dispBuf, "            ");
	LCD_DisplayStringLine_EN_CH(LINE(11),dispBuf);
	LCD_DisplayStringLine_EN_CH(LINE(14),dispBuf);
	
	sprintf((char*)dispBuf, "%.2f", length);
	LCD_DisplayStringLine_EN_CH(LINE(10),(uint8_t*)"ƫ�ƾ��룺");
	LCD_DisplayStringLine_EN_CH(LINE(11),dispBuf);
	sprintf((char*)dispBuf, "%.2f", speed);
	LCD_DisplayStringLine_EN_CH(LINE(13),(uint8_t*)"ˮƽ�ٶȣ�");
	LCD_DisplayStringLine_EN_CH(LINE(14),dispBuf);
}



//����ͼ������
void Draw_Graph()
{
	/*
	
	x�᳤��540
	y�᳤��200����100��
	
	ÿ�л�90���㣬ÿ������6������
	
	*/
	
	static int x = 0;
	int y1,y2 = 0;

	if(x == 0)
	{
		x = 0;
		
		//����
		LCD_SetColors(LCD_COLOR_BLACK, TRANSPARENCY);
		LCD_DrawFullRect(235,0,545,445);
		LCD_SetColors(LCD_COLOR_WHITE, TRANSPARENCY);
		
		//��ʾͼ��
		LCD_DisplayStringLine_EN_CH(LINE(1),(uint8_t*)"                length");
		LCD_DisplayStringLine_EN_CH(LINE(10),(uint8_t*)"                speed");
		
		//������ϵ
		
		//����
		LCD_DrawLine(240, 109, 540, 0);
		LCD_DrawLine(240, 110, 540, 0);
		LCD_DrawLine(240, 111, 540, 0);
		
		LCD_DrawLine(240, 329, 540, 0);
		LCD_DrawLine(240, 330, 540, 0);
		LCD_DrawLine(240, 331, 540, 0);
		
		//����
		LCD_DrawLine(239, 10, 200, 1);
		LCD_DrawLine(240, 10, 200, 1);
		LCD_DrawLine(241, 10, 200, 1);
		
		LCD_DrawLine(239, 230, 200, 1);
		LCD_DrawLine(240, 230, 200, 1);
		LCD_DrawLine(241, 230, 200, 1);
		

	}
	
	//�����ۼ�
	x++;
	
	
	
	//λ��
	y1 = 110 + length;
	LCD_SetColors(LCD_COLOR_BLUE2,TRANSPARENCY);
	LCD_DrawFullCircle(240 + x*6,(uint16_t)y1 ,2);
	
	//�ٶ�
	y2 = 330 + speed;
	LCD_SetColors(LCD_COLOR_BLUE2,TRANSPARENCY);
	LCD_DrawFullCircle(240 + x*6,(uint16_t)y2 ,2);
	
	if(x>=89)
	{
		x = 0;
	}
}

#endif

uint8_t image_updata_flag = 0;
void Image_Process(void)
{
	DCMI_CaptureCmd(ENABLE);			//��ȡһ֡ͼ�񵽻���
	
	//��д��
	//��ʱ1s �� ͼ��ɼ�����ж���λ
	image_updata_flag = 0;
	Task_Delay[9] = 1000;
	while(Task_Delay[9]!=0 && image_updata_flag == 0){}
	
//	//��д��
//	Delay(150);	//��150ms�ӳ٣�ȷ��DMA2�Ѿ���ɴ�DCMI�����Դ���
	
	Creat_Gray();	//���ɻҶȾ�������������ʾ����
	Image_Fix();	//ͼ������
	Creat_LCD();	//��ԭRGB565ͼ�񣬴�����ʾ����
		
	#if defined(__USART_DISPLAY_IMAGE)
		
	Usart_Display_Image();	//�Ӵ������ͼ�����ɽ��๦�ܵ���������ʾ
		
	#elif defined(__USART_DISPLAY_MATRIX)
	
	Usart_Display_Matrix();	//�Ӵ����������ֱ���ڴ��ڵ��������ϲ鿴
		
	#elif defined(__USART_DISPALY_WAVE)
	
	Usart_Display_Wave();	//�����������
	
	#endif
	
	
	
	//�������LCD_DISPLAY��include.h�У����ͱ���LCD����
	#ifdef LCD_DISPLAY
	
		#if defined(__DISPALY_DATA)
		
		Display_data();	//��ʾƫ�ƾ����ˮƽ�ٶ�
		
		#endif
		
		#if defined(__DISPALY_GRAPH)
		
		Draw_Graph();	//����ͼ������
		
		#endif
	
//		Camera_Buffer_To_Lcd_Buffer();							//�ֶ���ͼ��ӻ�����˵��Դ�
		DMA_AtoB_Config(FSMC_LCD_ADDRESS,LCD_FRAME_BUFFER);		//��DMA��ͼ��ӻ�����˵��Դ�
	
	#endif

}


//�ɵĵ������ҶȺ���

//uint8_t To_Gray(uint16_t row,uint16_t column)
//{
//	//�У�coiumn   ��Χ��1 -- IMG_WIDTH
//	//�У�row      ��Χ��1 -- IMG_HEIGHT

//	uint32_t r,g,b;
//	uint32_t num;
//	uint32_t gray; 
//	
//	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
//	
//	r = (CAMERA_BUFFER_ARRAY[0][num] >> 3) * 8;
//	g = ((CAMERA_BUFFER_ARRAY[0][num] & 0x07) << 3) + (CAMERA_BUFFER_ARRAY[0][num+1] >> 5) * 4;
//	b = (CAMERA_BUFFER_ARRAY[0][num+1] & 0x1F) * 8;

//	gray = (r * 299 + g * 587 + b * 114 + 500) / 1000;
//	
//	if(gray >= 256)
//		printf("�Ҷ��㷨����\n");
//	
//	return (uint8_t)gray;
//}


//�ɵĵ���ת��ΪRGB565����

//void To_LCD(uint16_t row,uint16_t column,uint8_t gray,uint8_t threshold)
//{
//	uint32_t num;
//	
//	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
//	
//	if(gray > threshold)
//	{
//		CAMERA_BUFFER_ARRAY[1][num] = 0xFF;
//		CAMERA_BUFFER_ARRAY[1][num+1] = 0xFF;
//	}
//	else
//	{
//		CAMERA_BUFFER_ARRAY[1][num] = 0x00;
//		CAMERA_BUFFER_ARRAY[1][num+1] = 0x00;
//	}
//}

//void To_Gray_LCD(uint16_t row,uint16_t column,uint8_t gray)
//{
//	uint32_t num;
//	uint8_t rb,g;
//	
//	num = (row-1)*IMG_WIDTH*2 + (column-1)*2;
//	
//	rb = gray / 8;
//	g = gray / 4;
//	
//	CAMERA_BUFFER_ARRAY[1][num+1] = (rb << 3) + ((g >> 3) & 0x07);	//�Ͱ�λ�ں��棬�߰�λ��ǰ��
//	CAMERA_BUFFER_ARRAY[1][num] = (g << 5) + rb;
//	
//}


//	//��ֻ�Ǽ򵥵�ͼ����
//	for(i = 0;i<IMG_WIDTH*IMG_HEIGHT*2;i++)
//	{
//		CAMERA_BUFFER_ARRAY[1][i] = CAMERA_BUFFER_ARRAY[0][i];
//		
//		
//	}
	
	//ʹ��8λ�ҶȺ���
//	for(i = 1;i<=IMG_HEIGHT;i++)
//	{
//		for(j = 1;j<=IMG_WIDTH;j++)
//		{
//			To_LCD(i,j,		To_Gray(i,j)	,0x80);
//		}
//	}
