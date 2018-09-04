//	@NRF24L01		CSN->B12	SCK->B13	MISO->B14		MOSI->B15		IRQ->A8		CE->A11
//	全双工的主/从模式

#include "sys.h"

extern Bit_Flag Flag;

void Master_Mode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	
	//SCK->B13 & MOSI->B15    --> AF_PP
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;		//推挽复用输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	// MISO->B14     -->IN_FLOATING
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_14;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//IRQ->A8 -->IN_FLOATING     	
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//CSN->B12 &CE->A11 -->Out_PP
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void Slave_Mode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	
	//SCK->B13 & MOSI->B15    -->IN_FLOATING
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//MISO->B14     -->IN_FLOATING
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;		//复用推挽输出
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_14;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void SPI2_Config(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;				//全双工模式
  SPI_InitStructure.SPI_Mode=(1==Flag.SPI_Mode)?SPI_Mode_Master:SPI_Mode_Slave;    //主从模式选择
  SPI_InitStructure.SPI_DataSize=SPI_DataSize_16b;        //收发数据为16bit
  SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low ; 
  SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge; 
  SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;                 //软件使能片选
  SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;   //
  SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;        //高位在前
  SPI_InitStructure.SPI_CRCPolynomial=0;       						//不使用CRC校验
	
	SPI_Init(SPI2,&SPI_InitStructure);
}









