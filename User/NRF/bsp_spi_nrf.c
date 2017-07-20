/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   2.4g无线模块/nrf24l01+/slave 应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
	/*																*\
		引脚说明：

		SCK（SPI）：		PB13
		MISO（SPI）：	PB14
		MOSI（SPI）：	PB15
		SCN（片选）：	PB12	低有效
		CE：			PB0		模式控制
		IRQ：			PB1
		
		使用方法：
		发送：
		调用模式设置函数 NRF_TX_Mode() 设置为发送模式
		向发送缓冲区 TX_BUF 写入 TX_PLOAD_WIDTH 字节数据
		调用 NRF_Tx_Dat() 函数一次性发送 TX_PLOAD_WIDTH 字节数据

	\*																*/

#include "bsp_spi_nrf.h"
#include "include.h"
#include "stdio.h"

//定义静态发送地址
u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01}; 
u8 RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};

//自己定义的发送缓冲数组
u8 RX_BUF[RX_PLOAD_WIDTH];		//接收数据缓存
u8 TX_BUF[TX_PLOAD_WIDTH];		//发射数据缓存

//中断级发送
#define __NRF_EXIT_SEND_BUFFER		150

u8 SPI_TX_BUF[__NRF_EXIT_SEND_BUFFER][32];			//用于中断级发送的缓冲队列
u16 head = 0,tail = 0;			//SPI_TX_BUF的头指针和尾指针
u8 disable_flag=1;				//失能发送完成中断标志

int NRF24L01_State = 0;	//0 -- 连接失败  1 -- 连接成功

void Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
}

/**
  * @brief  SPI的 I/O配置
  * @param  无
  * @retval 无
  */
void SPI_NRF_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启相应IO端口的时钟*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	/*使能SPI2时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2 );
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2 );
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2 );
	
	/*配置 SPI_NRF_SPI的 SCK,MISO,MOSI引脚，GPIOA^5,GPIOA^6,GPIOA^7 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*配置SPI_NRF_SPI的 CE 和 CSN 引脚*/
	
	//SCN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//CE
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*配置SPI_NRF_SPI的 IRQ 引脚*/
	
	//IRQ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	  
	/* 这是自定义的宏，用于拉高csn引脚，NRF进入空闲状态 */
	NRF_CSN_HIGH();

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//双线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 					//主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 				//数据大小8位
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;							//时钟极性，空闲时为低
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						//第1个边沿有效，上升沿为采样时刻
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   					//NSS信号由软件产生
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  //8分频，9MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  				//高位在前
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	/* Enable SPI2  */
	SPI_Cmd(SPI2, ENABLE);
}

/**
  * @brief   用于向NRF读/写一字节数据
  * @param   写入的数据
  *		@arg dat 
  * @retval  读取得的数据
  */
u8 SPI_NRF_RW(u8 dat)
{  	
	/* 当 SPI发送缓冲器非空时等待 */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

	/* 通过 SPI2发送一字节数据 */
	SPI_I2S_SendData(SPI2, dat);		

	/* 当SPI接收缓冲器为空时等待 */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);
}

/**
  * @brief   用于向NRF特定的寄存器写入数据
  * @param   
  *		@arg reg:NRF的命令+寄存器地址
  *		@arg dat:将要向寄存器写入的数据
  * @retval  NRF的status寄存器的状态
  */
u8 SPI_NRF_WriteReg(u8 reg,u8 dat)
{
	u8 status;
	
	NRF_CE_LOW();
	
	/*置低CSN，使能SPI传输*/
	NRF_CSN_LOW();
				
	/*发送命令及寄存器号 */
	status = SPI_NRF_RW(reg);
		 
	 /*向寄存器写入数据*/
	SPI_NRF_RW(dat); 
			  
	/*CSN拉高，完成*/	   
	NRF_CSN_HIGH();	
		
	/*返回状态寄存器的值*/
	return(status);
}

/**
  * @brief   用于从NRF特定的寄存器读出数据
  * @param   
  *		@arg reg:NRF的命令+寄存器地址
  * @retval  寄存器中的数据
  */
u8 SPI_NRF_ReadReg(u8 reg)
{
 	u8 reg_val;

	NRF_CE_LOW();
	
	/*置低CSN，使能SPI传输*/
 	NRF_CSN_LOW();
				
  	 /*发送寄存器号*/
	SPI_NRF_RW(reg); 

	 /*读取寄存器的值 */
	reg_val = SPI_NRF_RW(NOP);
	            
   	/*CSN拉高，完成*/
	NRF_CSN_HIGH();		
   	
	return reg_val;
}	

/**
  * @brief   用于向NRF的寄存器中写入一串数据
  * @param   
  *		@arg reg : NRF的命令+寄存器地址
  *		@arg pBuf：用于存储将被读出的寄存器数据的数组，外部定义
  * 	@arg bytes: pBuf的数据长度
  * @retval  NRF的status寄存器的状态
  */
u8 SPI_NRF_ReadBuf(u8 reg,u8 *pBuf,u8 bytes)
{
 	u8 status, byte_cnt;

	NRF_CE_LOW();
	
	/*置低CSN，使能SPI传输*/
	NRF_CSN_LOW();
		
	/*发送寄存器号*/		
	status = SPI_NRF_RW(reg); 

 	/*读取缓冲区数据*/
	for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
		pBuf[byte_cnt] = SPI_NRF_RW(NOP); //从NRF24L01读取数据  

	 /*CSN拉高，完成*/
	NRF_CSN_HIGH();	
		
 	return status;		//返回寄存器状态值
}

/**
  * @brief   用于向NRF的寄存器中写入一串数据
  * @param   
  *		@arg reg : NRF的命令+寄存器地址
  *		@arg pBuf：存储了将要写入写寄存器数据的数组，外部定义
  * 	@arg bytes: pBuf的数据长度
  * @retval  NRF的status寄存器的状态
  */
u8 SPI_NRF_WriteBuf(u8 reg ,u8 *pBuf,u8 bytes)
{
	 u8 status,byte_cnt;
	
	 NRF_CE_LOW();
	
   	 /*置低CSN，使能SPI传输*/
	 NRF_CSN_LOW();			

	 /*发送寄存器号*/	
  	 status = SPI_NRF_RW(reg); 
 	
  	  /*向缓冲区写入数据*/
	 for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
		SPI_NRF_RW(*pBuf++);	//写数据到缓冲区 	 
	  	   
	/*CSN拉高，完成*/
	NRF_CSN_HIGH();			
  
  	return (status);	//返回NRF24L01的状态 		
}

/**
  * @brief  配置并进入接收模式
  * @param  无
  * @retval 无
  */
void NRF_RX_Mode(void)
{
	NRF_CE_LOW();	

	SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址

	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    

	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址    

	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);      //设置RF通信频率    

	SPI_NRF_WriteReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度      

	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f); //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   

	SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG, 0x0f);  //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 

	/*CE拉高，进入接收模式*/	
	NRF_CE_HIGH();

}    

/**
  * @brief  配置发送模式
  * @param  无
  * @retval 无
  */
void NRF_TX_Mode(void)
{  
	NRF_CE_LOW();		

	SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);    //写TX节点地址 

	SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK   

	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    

	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  

	SPI_NRF_WriteReg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次

	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);       //设置RF通道为CHANAL

	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   

	SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,发射模式,开启所有中断

	/*CE拉高，进入发送模式*/
	NRF_CE_HIGH();
	
	Delay(0xffff); //CE要拉高一段时间才进入发送模式
}

/**
  * @brief  主要用于NRF与MCU是否正常连接
  * @param  无
  * @retval SUCCESS/ERROR 连接正常/连接失败
  */
u8 NRF_Check(void)
{
	u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};
	u8 buf1[5];
	u8 i; 
	 
	/*写入5个字节的地址.  */  
	SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,buf,5);

	/*读出写入的地址 */
	SPI_NRF_ReadBuf(TX_ADDR,buf1,5); 
	 
	/*比较*/               
	for(i=0;i<5;i++)
	{
		if(buf1[i]!=0xC2)
		break;
	} 
	       
	if(i==5)
		return SUCCESS ;        //MCU与NRF成功连接 
	else
		return ERROR ;        //MCU与NRF不正常连接
}

/**
  * @brief   用于向NRF的发送缓冲区中写入数据
  * @param   
  *	@arg txBuf：存储了将要发送的数据的数组，外部定义	
  * @retval  发送结果，成功返回TXDS,失败返回MAXRT或ERROR
  */
u8 NRF_Tx_Dat(u8 *txbuf)
{
	/*ce为低，进入待机模式1*/
	NRF_CE_LOW();

	/*写数据到TX BUF 最大 32个字节*/						
	SPI_NRF_WriteBuf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);

	/*CE为高，txbuf非空，发送数据包 */
	NRF_CE_HIGH();
	
	return 1;
}

/**
  * @brief   用于从NRF的接收缓冲区中读出数据
  * @param   
  *		@arg rxBuf ：用于接收该数据的数组，外部定义	
  * @retval 
  *		@arg 接收结果
  */
u8 NRF_Rx_Dat(u8 *rxbuf)
{
	u8 state;
	
	NRF_CE_HIGH();	 //进入接收状态
	
	/*等待接收中断*/
	while(NRF_Read_IRQ()!=0); 
	
	NRF_CE_LOW();  	 //进入待机状态
	
	/*读取status寄存器的值  */               
	state=SPI_NRF_ReadReg(STATUS);
	 
	/* 清除中断标志*/      
	SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state);

	/*判断是否接收到数据*/
	if(state&RX_DR)                                 //接收到数据
	{
		SPI_NRF_ReadBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		SPI_NRF_WriteReg(FLUSH_RX,NOP);          //清除RX FIFO寄存器
		return RX_DR; 
	}
	else    
		return ERROR;                    //没收到任何数据
}

void EXTI1_IRQHandler()
{
	u8 state;
	u8 status = 0;                 //发送返回的状态

	if( EXTI_GetITStatus(EXTI_Line1) != RESET ) 
	{
		/****************处理上一次发送结果*************/
			
		/*读取状态寄存器的值 */
		state = SPI_NRF_ReadReg(STATUS);
		/*清除TX_DS或MAX_RT中断标志*/
		SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state); 	
	  	/*清除TX FIFO寄存器*/
		SPI_NRF_WriteReg(FLUSH_TX,NOP);

		if(state&MAX_RT)					//达到最大重发次数
			status=MAX_RT; 
		else if(state&TX_DS)				//上次发送完成
			status=TX_DS;
		else
			status=ERROR;					//其他原因发送失败
		
		//上次发送结束检测
		if( (status==TX_DS) || (status==MAX_RT) )  //上一次发送成功或达到最大重发次数
		{
			tail++;
			if(tail >= __NRF_EXIT_SEND_BUFFER)
			{
				tail=0;
			}
		}
		
		/*********判断是否开启下一次发送*****************/
	
		if(tail == head)
		{
			NRF_EXIT(0);  		//关闭发送完成中断
			disable_flag=1;
		}
		else
		{
			NRF_Tx_Dat(&SPI_TX_BUF[tail][0]);
		}
		
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}
/*

临时的NRF发送函数，按字节输入
只能发送TX_PLOAD_WIDTH的倍数的字节，否则会卡在发送缓冲区里不发送
没有发送超时功能

*/


uint8_t NRF_Send_Counter = 0;
void NRF_Send(u8 Data)
{
	u8 i;
	
	TX_BUF[NRF_Send_Counter] = Data;	//存入发送数组
	NRF_Send_Counter++;
	
	if(NRF_Send_Counter>=TX_PLOAD_WIDTH)	//发送缓冲区满
	{
		//清零计数器
		NRF_Send_Counter = 0;
		
		/****装入队列******/

		for(i=0;i<=31;i++)
		{
      		SPI_TX_BUF[head][i]=TX_BUF[i];
		}
		
		if(disable_flag == 1)
		{
			NRF_Tx_Dat(&SPI_TX_BUF[head][0]);
			NRF_EXIT(1);
			disable_flag=0;
		}
		
		head++;
		if(head >= __NRF_EXIT_SEND_BUFFER)
		{
			head=0;
		}
	}
}

void NRF_EXIT(u8 en)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 连接 EXTI 中断源 到key1引脚 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);

	/* 选择 EXTI 中断源 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	if(en)
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	else
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
	 
	/* 配置 NVIC */
  
	/* 配置中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void NRF24L01_Init(void)
{
	SPI_NRF_Init();
	NRF_EXIT(0);    //初始化发送完成中断
	if(NRF_Check() != 0)
	{
		printf("NRF初始化成功！\r\n");
		NRF_TX_Mode();				//设置为发送模式
		NRF24L01_State = 1;
	}
	else
	{
		printf("NRF24L01初始化失败！\r\n");
		NRF24L01_State = 0;
	}
}


/*********************************************END OF FILE**********************/

///*判断发送状态*/
//switch(status)
//{
//	case MAX_RT:
//		printf("\r\n 主机端 没接收到应答信号，发送次数超过限定值，发送失败。 \r\n");
//		break;

//	case ERROR:
//		printf("\r\n 未知原因导致发送失败。 \r\n");
//		break;

//	case TX_DS:
//		printf("\r\n 主机端 接收到 从机端 的应答信号，发送成功！ \r\n");	 		
//		break;  								
//}

