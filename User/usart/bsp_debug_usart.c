/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   重定向c库printf函数到usart端口
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_debug_usart.h"
#include "include.h"

//处理USART2接收到的信息
void USART2_Receive_Data_Handle(u8 data)
{
	static int state = 0;

	switch(state)
	{
		case 0:
			if(data == 0x01)	//帧头 0x01 0xFA
				state = 1;
			break;
		case 1:
			if(data == 0xFA)
				state = 2;
			break;
		case 2:
			if(data == 0x01)
				state = 3;		//转图像传输指令
			else if(data == 0x02)
				state = 4;		//转波形传输指令
			break;
		case 3:
			state = 0;	//归零
		
			break;
		case 4:
			state = 0;	//归零
		
			break;
		default:
			break;
	}
}

u8 TxBuffer2[4000];
u16 TxCounter2=0;
u16 cnt2=0;
int full_flag = 0;

//配置USART2
void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 使能GPIOA时钟 */
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);

	/* 使能 UART 时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* 配置中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* 连接 PXx 到 USARTx_Tx*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2, GPIO_AF_USART2);

	/*  连接 PXx 到 USARTx__Rx*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

	/* 配置Tx引脚为复用功能  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 配置Rx引脚为复用功能 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
			
	/* 配置USART2模式 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure); 
	USART_Cmd(USART2, ENABLE);
	
	//打开接收中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}


void USART2_IRQHandler(void)
{
	u8 com_data;
	
	//usart2接收中断
	if( USART_GetITStatus(USART1,USART_IT_RXNE) )
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//清除中断标志

		com_data = USART1->DR;
		
		//此处添加接收处理函数
		
	}
	
	//usart2发送完成中断
	if(USART_GetITStatus(USART2,USART_IT_TXE )) 
	{		
		//发送数据
		USART2->DR = TxBuffer2[TxCounter2++];	//写DR清除中断标志位
		
		//处理发送计数指针TxCounter2
		if(TxCounter2>=4000)
		{
			TxCounter2=0;
		}
		
		if(TxCounter2 == cnt2)	//发送指针追上了输入指针
		{
			USART2->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
			full_flag = 0;	//清除队满标志
		}
		
//		USART_ClearITPendingBit(USART2,USART_IT_TXE);
	}
}

void USART2_Send(unsigned char ch)
{
	if(!full_flag)
	{
		TxBuffer2[cnt2++] = ch;
	
		//处理传入计数指针数值
		if(cnt2>=4000)
		{
			cnt2=0;
		}
		
		if(cnt2 == TxCounter2)	//输入指针追上了发送指针
		{
			full_flag = 1;
		}

		if(!(USART2->CR1 & USART_CR1_TXEIE))//检测是否发完，发完CR1的TXEIE被上面置为0了（关闭了中断）
		{
			USART_ITConfig(USART2, USART_IT_TXE, ENABLE); //打开发送中断
		}
	}
}



 /**
  * @brief  DEBUG_USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void Debug_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
  RCC_AHB1PeriphClockCmd( DEBUG_USART_RX_GPIO_CLK|DEBUG_USART_TX_GPIO_CLK, ENABLE);

  /* 使能 UART 时钟 */
  RCC_APB2PeriphClockCmd(DEBUG_USART_CLK, ENABLE);
  
  /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT,DEBUG_USART_RX_SOURCE, DEBUG_USART_RX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT,DEBUG_USART_TX_SOURCE,DEBUG_USART_TX_AF);

  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
  GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
			
  /* 配置串DEBUG_USART 模式 */
  USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(DEBUG_USART, &USART_InitStructure); 
  USART_Cmd(DEBUG_USART, ENABLE);
}

///重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口DEBUG_USART */
		USART_SendData(DEBUG_USART, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口DEBUG_USART，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USART);
}
/*********************************************END OF FILE**********************/
