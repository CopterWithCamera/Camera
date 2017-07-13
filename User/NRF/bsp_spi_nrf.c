/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   2.4g����ģ��/nrf24l01+/slave Ӧ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
	/*																*\
		����˵����

		SCK��SPI����		PB13
		MISO��SPI����	PB14
		MOSI��SPI����	PB15
		SCN��Ƭѡ����	PB12	����Ч
		CE��			PB0		ģʽ����
		IRQ��			PB1
		
		ʹ�÷�����
		���ͣ�
		����ģʽ���ú��� NRF_TX_Mode() ����Ϊ����ģʽ
		���ͻ����� TX_BUF д�� TX_PLOAD_WIDTH �ֽ�����
		���� NRF_Tx_Dat() ����һ���Է��� TX_PLOAD_WIDTH �ֽ�����

	\*																*/

#include "bsp_spi_nrf.h"
#include "include.h"
#include "stdio.h"

u8 RX_BUF[RX_PLOAD_WIDTH];		//�������ݻ���
u8 TX_BUF[TX_PLOAD_WIDTH];		//�������ݻ���
u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};  // ����һ����̬���͵�ַ
u8 RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};

int NRF24L01_State = 0;	//0 -- ����ʧ��  1 -- ���ӳɹ�

void Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
}

/**
  * @brief  SPI�� I/O����
  * @param  ��
  * @retval ��
  */
void SPI_NRF_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/*������ӦIO�˿ڵ�ʱ��*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	/*ʹ��SPI2ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2 );
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2 );
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2 );
	
	/*���� SPI_NRF_SPI�� SCK,MISO,MOSI���ţ�GPIOA^5,GPIOA^6,GPIOA^7 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*����SPI_NRF_SPI�� CE �� CSN ����*/
	
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

	/*����SPI_NRF_SPI�� IRQ ����*/
	
	//IRQ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	  
	/* �����Զ���ĺ꣬��������csn���ţ�NRF�������״̬ */
	NRF_CSN_HIGH();

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 					//��ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 				//���ݴ�С8λ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;							//ʱ�Ӽ��ԣ�����ʱΪ��
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						//��1��������Ч��������Ϊ����ʱ��
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   					//NSS�ź����������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  //8��Ƶ��9MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  				//��λ��ǰ
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	/* Enable SPI2  */
	SPI_Cmd(SPI2, ENABLE);
}

/**
  * @brief   ������NRF��/дһ�ֽ�����
  * @param   д�������
  *		@arg dat 
  * @retval  ��ȡ�õ�����
  */
u8 SPI_NRF_RW(u8 dat)
{  	
	/* �� SPI���ͻ������ǿ�ʱ�ȴ� */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

	/* ͨ�� SPI2����һ�ֽ����� */
	SPI_I2S_SendData(SPI2, dat);		

	/* ��SPI���ջ�����Ϊ��ʱ�ȴ� */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);
}

/**
  * @brief   ������NRF�ض��ļĴ���д������
  * @param   
  *		@arg reg:NRF������+�Ĵ�����ַ
  *		@arg dat:��Ҫ��Ĵ���д�������
  * @retval  NRF��status�Ĵ�����״̬
  */
u8 SPI_NRF_WriteReg(u8 reg,u8 dat)
{
	u8 status;
	
	NRF_CE_LOW();
	
	/*�õ�CSN��ʹ��SPI����*/
	NRF_CSN_LOW();
				
	/*��������Ĵ����� */
	status = SPI_NRF_RW(reg);
		 
	 /*��Ĵ���д������*/
	SPI_NRF_RW(dat); 
			  
	/*CSN���ߣ����*/	   
	NRF_CSN_HIGH();	
		
	/*����״̬�Ĵ�����ֵ*/
	return(status);
}

/**
  * @brief   ���ڴ�NRF�ض��ļĴ�����������
  * @param   
  *		@arg reg:NRF������+�Ĵ�����ַ
  * @retval  �Ĵ����е�����
  */
u8 SPI_NRF_ReadReg(u8 reg)
{
 	u8 reg_val;

	NRF_CE_LOW();
	
	/*�õ�CSN��ʹ��SPI����*/
 	NRF_CSN_LOW();
				
  	 /*���ͼĴ�����*/
	SPI_NRF_RW(reg); 

	 /*��ȡ�Ĵ�����ֵ */
	reg_val = SPI_NRF_RW(NOP);
	            
   	/*CSN���ߣ����*/
	NRF_CSN_HIGH();		
   	
	return reg_val;
}	

/**
  * @brief   ������NRF�ļĴ�����д��һ������
  * @param   
  *		@arg reg : NRF������+�Ĵ�����ַ
  *		@arg pBuf�����ڴ洢���������ļĴ������ݵ����飬�ⲿ����
  * 	@arg bytes: pBuf�����ݳ���
  * @retval  NRF��status�Ĵ�����״̬
  */
u8 SPI_NRF_ReadBuf(u8 reg,u8 *pBuf,u8 bytes)
{
 	u8 status, byte_cnt;

	NRF_CE_LOW();
	
	/*�õ�CSN��ʹ��SPI����*/
	NRF_CSN_LOW();
		
	/*���ͼĴ�����*/		
	status = SPI_NRF_RW(reg); 

 	/*��ȡ����������*/
	for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
		pBuf[byte_cnt] = SPI_NRF_RW(NOP); //��NRF24L01��ȡ����  

	 /*CSN���ߣ����*/
	NRF_CSN_HIGH();	
		
 	return status;		//���ؼĴ���״ֵ̬
}

/**
  * @brief   ������NRF�ļĴ�����д��һ������
  * @param   
  *		@arg reg : NRF������+�Ĵ�����ַ
  *		@arg pBuf���洢�˽�Ҫд��д�Ĵ������ݵ����飬�ⲿ����
  * 	@arg bytes: pBuf�����ݳ���
  * @retval  NRF��status�Ĵ�����״̬
  */
u8 SPI_NRF_WriteBuf(u8 reg ,u8 *pBuf,u8 bytes)
{
	 u8 status,byte_cnt;
	
	 NRF_CE_LOW();
	
   	 /*�õ�CSN��ʹ��SPI����*/
	 NRF_CSN_LOW();			

	 /*���ͼĴ�����*/	
  	 status = SPI_NRF_RW(reg); 
 	
  	  /*�򻺳���д������*/
	 for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
		SPI_NRF_RW(*pBuf++);	//д���ݵ������� 	 
	  	   
	/*CSN���ߣ����*/
	NRF_CSN_HIGH();			
  
  	return (status);	//����NRF24L01��״̬ 		
}

/**
  * @brief  ���ò��������ģʽ
  * @param  ��
  * @retval ��
  */
void NRF_RX_Mode(void)
{
	NRF_CE_LOW();	

	SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ

	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    

	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ    

	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);      //����RFͨ��Ƶ��    

	SPI_NRF_WriteReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��      

	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f); //����TX�������,0db����,2Mbps,���������濪��   

	SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG, 0x0f);  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 

	/*CE���ߣ��������ģʽ*/	
	NRF_CE_HIGH();

}    

/**
  * @brief  ���÷���ģʽ
  * @param  ��
  * @retval ��
  */
void NRF_TX_Mode(void)
{  
	NRF_CE_LOW();		

	SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);    //дTX�ڵ��ַ 

	SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK   

	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    

	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  

	SPI_NRF_WriteReg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��

	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);       //����RFͨ��ΪCHANAL

	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   

	SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�

	/*CE���ߣ����뷢��ģʽ*/
	NRF_CE_HIGH();
	
	Delay(0xffff); //CEҪ����һ��ʱ��Ž��뷢��ģʽ
}

/**
  * @brief  ��Ҫ����NRF��MCU�Ƿ���������
  * @param  ��
  * @retval SUCCESS/ERROR ��������/����ʧ��
  */
u8 NRF_Check(void)
{
	u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};
	u8 buf1[5];
	u8 i; 
	 
	/*д��5���ֽڵĵ�ַ.  */  
	SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,buf,5);

	/*����д��ĵ�ַ */
	SPI_NRF_ReadBuf(TX_ADDR,buf1,5); 
	 
	/*�Ƚ�*/               
	for(i=0;i<5;i++)
	{
		if(buf1[i]!=0xC2)
		break;
	} 
	       
	if(i==5)
		return SUCCESS ;        //MCU��NRF�ɹ����� 
	else
		return ERROR ;        //MCU��NRF����������
}

/**
  * @brief   ������NRF�ķ��ͻ�������д������
  * @param   
  *	@arg txBuf���洢�˽�Ҫ���͵����ݵ����飬�ⲿ����	
  * @retval  ���ͽ�����ɹ�����TXDS,ʧ�ܷ���MAXRT��ERROR
  */
u8 NRF_Tx_Dat(u8 *txbuf)
{
	u8 state;

	/*ceΪ�ͣ��������ģʽ1*/
	NRF_CE_LOW();

	/*д���ݵ�TX BUF ��� 32���ֽ�*/						
	SPI_NRF_WriteBuf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);

	/*CEΪ�ߣ�txbuf�ǿգ��������ݰ� */
	NRF_CE_HIGH();

	/*�ȴ���������ж� */                            
	while(NRF_Read_IRQ()!=0);

	/*��ȡ״̬�Ĵ�����ֵ */
	state = SPI_NRF_ReadReg(STATUS);

	/*���TX_DS��MAX_RT�жϱ�־*/
	SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state); 	

	SPI_NRF_WriteReg(FLUSH_TX,NOP);		//���TX FIFO�Ĵ���

	/*�ж��ж�����*/    
	if(state&MAX_RT)					//�ﵽ����ط�����
		 return MAX_RT; 
	else if(state&TX_DS)				//�������
		return TX_DS;
	else						  
		return ERROR;					//����ԭ����ʧ��
}

/**
  * @brief   ���ڴ�NRF�Ľ��ջ������ж�������
  * @param   
  *		@arg rxBuf �����ڽ��ո����ݵ����飬�ⲿ����	
  * @retval 
  *		@arg ���ս��
  */
u8 NRF_Rx_Dat(u8 *rxbuf)
{
	u8 state;
	
	NRF_CE_HIGH();	 //�������״̬
	
	/*�ȴ������ж�*/
	while(NRF_Read_IRQ()!=0); 
	
	NRF_CE_LOW();  	 //�������״̬
	
	/*��ȡstatus�Ĵ�����ֵ  */               
	state=SPI_NRF_ReadReg(STATUS);
	 
	/* ����жϱ�־*/      
	SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state);

	/*�ж��Ƿ���յ�����*/
	if(state&RX_DR)                                 //���յ�����
	{
		SPI_NRF_ReadBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		SPI_NRF_WriteReg(FLUSH_RX,NOP);          //���RX FIFO�Ĵ���
		return RX_DR; 
	}
	else    
		return ERROR;                    //û�յ��κ�����
}

/*

��ʱ��NRF���ͺ��������ֽ�����
ֻ�ܷ���TX_PLOAD_WIDTH�ı������ֽڣ�����Ῠ�ڷ��ͻ������ﲻ����
û�з��ͳ�ʱ����

*/
uint8_t NRF_Send_Counter = 0;
u8 NRF_Send(u8 Data)
{
	u8 status = 0;
	u8 send_counter = 0;
	
	TX_BUF[NRF_Send_Counter] = Data;	//���뷢������
	NRF_Send_Counter++;
	
	if(NRF_Send_Counter>=TX_PLOAD_WIDTH)	//���ͻ�������
	{		
		//���������
		NRF_Send_Counter = 0;
		
		//���뷢������
		
		/*
			#define MAX_RT      0x10 //�ﵽ����ط������жϱ�־λ
			#define TX_DS		0x20 //��������жϱ�־λ	  
			#define RX_DR		0x40 //���յ������жϱ�־λ
		*/
		
		//�Զ��ط�
		send_counter = 20;		//�Զ��ط�����
		while(send_counter>0)	
		{
			send_counter--;
			status = NRF_Tx_Dat(TX_BUF);	//�������ݣ���ȡ����״̬
			
			if(status == TX_DS)
				return 1;	//���ͳɹ�
		}
		
		//printf("NRF Send Error!\r\n");
		return 2;	//����ʧ��
	}
	return 0;	//���ͻ�����δ��
}

void NRF24L01_Init(void)
{
	SPI_NRF_Init();
	if(NRF_Check() != 0)
	{
		printf("NRF��ʼ���ɹ���\r\n");
		NRF_TX_Mode();				//����Ϊ����ģʽ
		NRF24L01_State = 1;
	}
	else
	{
		printf("NRF24L01��ʼ��ʧ�ܣ�\r\n");
		NRF24L01_State = 0;
	}
}

/*********************************************END OF FILE**********************/

///*�жϷ���״̬*/
//switch(status)
//{
//	case MAX_RT:
//		printf("\r\n ������ û���յ�Ӧ���źţ����ʹ��������޶�ֵ������ʧ�ܡ� \r\n");
//		break;

//	case ERROR:
//		printf("\r\n δ֪ԭ���·���ʧ�ܡ� \r\n");
//		break;

//	case TX_DS:
//		printf("\r\n ������ ���յ� �ӻ��� ��Ӧ���źţ����ͳɹ��� \r\n");	 		
//		break;  								
//}

