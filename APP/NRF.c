//	@NRF24L01		CSN->B12	SCK->B13	MISO->B14		MOSI->B15		IRQ->A8		CE->A11
//	全双工的主/从模式

#include "NRF.h"

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址

/**
	*	@brief 	初始化SPI
	*	@param	None
	*	@retval	Nome
	*/
void SPI2_NRF_CONFIG(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	
	//SCK->B13 & MISO->B14 & MOSI->B15    --> AF_PP
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;		//推挽复用输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	GPIO_SetBits(GPIOB,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);	//置高
	//IRQ->A8 -->IPU     	CE->A11 -->Out_PP
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8;
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//CSN->B12-->Out_PP
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;				//全双工模式
  SPI_InitStructure.SPI_Mode=SPI_Mode_Master;    					//设置主SPI
  SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;        	//收发数据为8bit
  SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low ; 							//时钟悬空低
  SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge; 							//数据捕获与第一个时钟沿
  SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;                 //软件使能片选
  SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_16;   //波特率预分频值为16
  SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;        //高位在前
	SPI_InitStructure.SPI_CRCPolynomial = 7;								//CRC校验	
	SPI_Init(SPI2,&SPI_InitStructure);
	
	SPI_Cmd(SPI2,ENABLE);				//使能SPI
	
	NRF_CE=0;								//使能24L01
	NRF_CSN=1;							//SPI片选取消
}

/**
	*	@brief 	设置SPI的传输速率
	*	@param	分频值
	*	@retval	None
	*/
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7;
	SPI2->CR1|=SPI_BaudRatePrescaler; //设置SPI2速度
	SPI_Cmd(SPI2,ENABLE);
}
/**
	*	@brief 	SPI写字节
	*	@param	写入的数据
	*	@retval	读取的字节
	*/
u8 SPI2_WRByte(u8 TxData)
{
	u8 retry=0;
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET)	//等待发送完成
	{
		retry++; 
		if(retry>200) return 0;
	}
	SPI_I2S_SendData(SPI2,TxData);//通过SPI2发送一个字节 
	retry=0;
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET)	//等待接收完成
	{
		retry++;
		if(retry>200) return 0;
	}
	return SPI_I2S_ReceiveData(SPI2);//返回SPI2接收到的数据
}

/**
	*	@brief 	检测NRF24L01是否存在
	*	@param	None
	*	@retval	0：存在		1：不存在
	*/
u8 NRF_Check(void)
{
	u8 buf[5]={0xA5,0xA5,0xA5,0xA5,0xA5};
	u8 i;
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4);	//36/4=9MHz 
	NRF_WBuf(NRF_WRITE_REG+TX_ADDR,buf,5);		//写入5个字节的地址
	NRF_RBuf(TX_ADDR,buf,5);		//读出写入的地址
	for(i=0;i<5;i++)
	{
		if(buf[i]!=0xA5)
			break;
	}
	if(5!=i)	return 1;	//for循环中途退出，检测NRF24L01异常
	return 0;						//检测到NRF24L01正常
}

/**
	*	@brief 	SPI写寄存器
	*	@param	寄存器地址，写入的数据
	*	@retval	None
	*/
u8 NRF_WReg(u8 reg,u8 data)
{
	u8 status;
	NRF_CSN=0;									//使能SPI传输
	status = SPI2_WRByte(reg);	//发送寄存器号
	SPI2_WRByte(data);					//写入寄存器的值
	NRF_CSN=1;									//禁止SPI传输
	return status;
}

/**
	*	@brief 	SPI读取寄存器值
	*	@param	要读取的寄存器地址
	*	@retval 读取的值
	*/
u8 NRF_RReg(u8 reg)
{
	u8 data;
	NRF_CSN=0;									//使能SPI传输
	SPI2_WRByte(reg);						//寄存器地址
	data=SPI2_WRByte(0xff);			//读取寄存内容
	NRF_CSN=1;									//禁止SPI传输
	return data;
}

/**
	*	@brief 	在制定的寄存器地址中读出指定长度的数值
	*	@param	寄存器地址，数据长度，数据首地址指针
	*	@retval	状态值
	*/
u8 NRF_RBuf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,i;
	NRF_CSN=0;
	status=SPI2_WRByte(reg);
	for(i=0;i<len;i++)
		pBuf[i]=SPI2_WRByte(0xff);
	NRF_CSN=1;
	return status;
}

/**
	*	@brief 	在指定位置写入指定长度的数据
	*	@param	寄存器地址，数据指针，数据长度
	*	@retval	状态值
	*/
u8 NRF_WBuf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,i;
	NRF_CSN=0;
	status=SPI2_WRByte(reg);
	for(i=0;i<len;i++)
		SPI2_WRByte(*pBuf++);
	NRF_CSN=1;
	return status;
}

/**
	*	@brief 	启动NRF24L01发送一次数据
	*	@param	待发送数据首地址
	*	@retval 发送完成情况
	*/
u8 NRF_TxPacket(u8 *txbuf)
{
	u8 status;
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4);
	NRF_CE=0;
	NRF_WBuf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);	//写数据到TxBuf
	NRF_CE=1;		//启动发送
	while(NRF_IRQ!=0);		//等待发送完成
	status=NRF_RReg(STATUS);	//读取状态寄存器值
	NRF_WReg(NRF_WRITE_REG+STATUS,status);	//清除TX——DSMAX_RT中断标识
	if(status&MAX_TX)		//达到最大重发次数
	{
		NRF_WReg(FLUSH_TX,0xff);	//清除TX_FIFO寄存器
		return MAX_TX;
	}
	if(status&TX_OK)	//发送完成
		return TX_OK;
	return 0xff;		//发送失败
}


/**
	*	@brief 	启动NRF24L01 接收一次数据
	*	@param	待发送数据首地址
	*	@retval 0：接收完成  其他：错误代码
	*/
u8 NRF_RxPacket(u8 *rxbuf)
{
	u8 status;
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4);
	status=NRF_RReg(STATUS);		//读取状态寄存器值
	NRF_WReg(NRF_WRITE_REG+STATUS,status);		//清除TX_DSMAX_RT中断标识
	if(status&RX_OK)		//接受到数据
	{
		NRF_RBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF_WReg(FLUSH_RX,0xff);//清除RX_FIFO寄存器
		return 0;
	}
	return 1;   //没有接收到数据
}

/**
	*	@brief 	初始化NRF24L01为RX模式
	*	@param  None
	*	@retval None
	*/
void NRF_RX_Mode(void)
{
	NRF_CE=0;
	NRF_WBuf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);
	NRF_WReg(NRF_WRITE_REG+EN_AA,0x01);	//使能通道0的自动应答
	NRF_WReg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址
	NRF_WReg(NRF_WRITE_REG+RF_CH,40);				//设置RF通信频率
	NRF_WReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道的有效数据宽度
	NRF_WReg(NRF_WRITE_REG+RF_SETUP,0x0f);	//设置TX发射0db增益，2Mbps,低噪声增益开启
	NRF_WReg(NRF_WRITE_REG+NRF_CONFIG,0x0f);		//配置基本工作模式的参数：PWR_UP,EN_CRC,16BIT_CRC，接收模式
	NRF_CE=1;		//进入接收模式
}

/**
	*	@brief 	初始化NRF24L01为TX模式
	*	@param  None
	*	@retval None
	*/
void NRF_Tx_Mode(void)
{
	NRF_CE=0;
	NRF_WBuf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);
	NRF_WBuf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//设置TX节点地址，主要为了使能ACK
	NRF_WReg(NRF_WRITE_REG+EN_AA,0x01);	//使能通道0的自动应答
	NRF_WReg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址
	NRF_WReg(NRF_WRITE_REG+SETUP_RETR,0x0a); //设置自动重发间隔时间500us+86us,，最大重发次数10次
	NRF_WReg(NRF_WRITE_REG+RF_CH,40);				//设置RF通道40
	NRF_WReg(NRF_WRITE_REG+RF_SETUP,0x0f);	//设置TX发射0db增益，2Mbps,低噪声增益开启
	NRF_WReg(NRF_WRITE_REG+NRF_CONFIG,0x0e);		//配置基本工作模式的参数：PWR_UP,EN_CRC,16BIT_CRC，接收模式，开启所有中断
	NRF_CE=1;		//进入接收模式，10us后启动发送
}

//test
void NRF_Tx(u8 txbuf)
{
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4);
	NRF_CE=0;
	NRF_CSN=0;
	SPI2_WRByte(WR_TX_PLOAD);
	SPI2_WRByte(txbuf);
	NRF_CSN=1;
	NRF_CE=1;		//启动发送
}

