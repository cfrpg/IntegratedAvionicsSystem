#include "flash.h"

DMA_InitTypeDef di;

u8 flash_dma_busy;
u32 timer;

void FlashInit(void)
{
#ifndef IAS_V2_BOARD
	return;
#endif
	GPIO_InitTypeDef gi;
	SPI_InitTypeDef si;
	NVIC_InitTypeDef ni;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);  
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SPI1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	//CS GPIO
	gi.GPIO_Pin=GPIO_Pin_12;
	gi.GPIO_Mode=GPIO_Mode_Out_PP;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gi);
	
	//SPI GPIO
	gi.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	gi.GPIO_Mode=GPIO_Mode_AF_PP;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gi);
	FLASH_CS=1;
	
	//SPI config
	si.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	si.SPI_Mode=SPI_Mode_Master;
	si.SPI_DataSize=SPI_DataSize_8b;
	si.SPI_CPOL=SPI_CPOL_High;
	si.SPI_CPHA=SPI_CPHA_2Edge;
	si.SPI_NSS=SPI_NSS_Soft;
	si.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
	si.SPI_FirstBit=SPI_FirstBit_MSB;
	si.SPI_CRCPolynomial=7;
	SPI_Init(SPI1,&si);
	SPI_Cmd(SPI1,ENABLE);
	
	//DMA config
	DMA_DeInit(DMA1_Channel3);
	di.DMA_PeripheralBaseAddr=(u32)&SPI1->DR;
	//di.DMA_MemoryBaseAddr=(u32)buf;
	di.DMA_Mode=DMA_Mode_Normal;
	di.DMA_DIR=DMA_DIR_PeripheralDST;
	di.DMA_BufferSize=256;
	di.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	di.DMA_MemoryInc=DMA_MemoryInc_Enable;
	di.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	di.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	di.DMA_Priority=DMA_Priority_Medium;
	di.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3,&di);
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
	
	//NVIC config
	ni.NVIC_IRQChannel=DMA1_Channel3_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=2;
	ni.NVIC_IRQChannelSubPriority=1;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);
	flash_dma_busy=0;
	FlashStartup();
	
}

u8 spi1_read_write(u8 b)
{
#ifndef IAS_V2_BOARD
	return 0;
#endif
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,b);	
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI1);
}

void FlashStartup(void)
{
#ifndef IAS_V2_BOARD
	return;
#endif	
	u8 buff[8]={0};
	u8 i;
	
	FLASH_CS=0;
	//Enter 4 byte mode
	spi1_read_write(0xB7);
	FLASH_CS=1;
	
	//Read ID
	FLASH_CS=0;
	spi1_read_write(0x4B);
	spi1_read_write(0xFF);
	spi1_read_write(0xFF);
	spi1_read_write(0xFF);
	spi1_read_write(0xFF);
	spi1_read_write(0xFF);
	for(i=0;i<8;i++)
	{
		buff[i]=spi1_read_write(0xFF);
	}
	FLASH_CS=1;
	for(i=0;i<8;i++)
	{
		printf("%x ",buff[i]);
		
	}
	printf("\r\n");
	FLASH_CS=0;
	spi1_read_write(0x9F);	
	for(i=0;i<3;i++)
	{
		buff[i]=spi1_read_write(0xFF);
	}
	FLASH_CS=1;
	for(i=0;i<3;i++)
	{
		printf("%x ",buff[i]);
		
	}
	printf("\r\n");
}

u8 FlashReadBusy(void)
{
#ifndef IAS_V2_BOARD
	return 0;
#endif
	u8 t;
	FLASH_CS=0;
	spi1_read_write(0x05);
	t=spi1_read_write(0xFF);
	FLASH_CS=1;
	//printf("%x\r\n",t);
	return t&0x01;
}

u8 FlashReadWEL(void)
{
#ifndef IAS_V2_BOARD
	return 0;
#endif
	u8 t;
	FLASH_CS=0;
	spi1_read_write(0x05);
	t=spi1_read_write(0xFF);
	FLASH_CS=1;
	return t&0x02;
}

void FlashWritePage(u32 addr,u32 len,u8* data,u8 wait)
{
#ifndef IAS_V2_BOARD
	return;
#endif
	u16 i;
	if(len>256)
	{
		printf("Page length exceeded.\r\n");
		return;
	}
	addr&=0xFFFFFF00;
	if(addr>FLASH_LAST_PAGE_ADDR)
	{
		printf("Flash length exceeded.\r\n");
		return;
	}
	FLASH_CS=0;
	spi1_read_write(0x06);	
	FLASH_CS=1;
	//while(!FlashReadWEL());
	FLASH_CS=0;
	spi1_read_write(0x02);
	spi1_read_write((addr>>24)&0xFF);
	spi1_read_write((addr>>16)&0xFF);
	spi1_read_write((addr>>8)&0xFF);
	spi1_read_write(addr&0xFF);
	for(i=0;i<len;i++)
	{
		spi1_read_write(data[i]);
	}
	FLASH_CS=1;
	
	if(wait)
	{
		//printf("Programming...\r\n");
		while(FlashReadBusy());
		//printf("Program complete.\r\n");
	}
}
	
void FlashRead(u32 addr,u32 len,u8* data)
{
#ifndef IAS_V2_BOARD
	return;
#endif
	u32 i;
	if(addr+len>FLASH_CAP)
	{
		printf("Flash length exceeded.\r\n");
		return;
	}
	FLASH_CS=0;
	spi1_read_write(0x03);
	spi1_read_write((addr>>24)&0xFF);
	spi1_read_write((addr>>16)&0xFF);
	spi1_read_write((addr>>8)&0xFF);
	spi1_read_write(addr&0xFF);
	for(i=0;i<len;i++)
	{
		data[i]=spi1_read_write(0xFF);
	}
	FLASH_CS=1;
}

void FlashEraseSector(u32 addr,u8 wait)
{
#ifndef IAS_V2_BOARD
	return;
#endif
	addr&=0xFFFFF000;
	if(addr>FLASH_LAST_SECTOR_ADDR)
	{
		printf("Erase out of range.\r\n");
		return;
	}
	timer=0;
	FLASH_CS=0;
	spi1_read_write(0x06);	
	FLASH_CS=1;
	FLASH_CS=0;
	spi1_read_write(0x20);
	spi1_read_write((addr>>24)&0xFF);
	spi1_read_write((addr>>16)&0xFF);
	spi1_read_write((addr>>8)&0xFF);
	spi1_read_write(addr&0xFF);
	FLASH_CS=1;
	if(wait)
	{		
		while(FlashReadBusy()) timer++;	
		//printf("Erase complete.%d\r\n",timer);
	}
}

void FlashStartDMAWrite(u32 addr,u32 len,u8* data,u8 wait)
{
#ifndef IAS_V2_BOARD
	return;
#endif
	u16 i;
	if(len>256)
	{
		printf("Page length exceeded.\r\n");
		return;
	}
	addr&=0xFFFFFF00;
	if(addr>FLASH_LAST_PAGE_ADDR)
	{
		printf("Flash length exceeded.\r\n");
		return;
	}	
	//DMA_Cmd(DMA1_Channel3,DISABLE);	
	DMA1_Channel3->CMAR=(u32)data;
	DMA1_Channel3->CNDTR=len;
	//di.DMA_MemoryBaseAddr=(u32)data;
	//di.DMA_BufferSize=len;
	//DMA_Init(DMA1_Channel3,&di);
	
	//DMA_Cmd(DMA1_Channel3,DISABLE);
	//DMA_SetCurrDataCounter(DMA1_Channel3,len);
	
	FLASH_CS=0;
	spi1_read_write(0x06);	
	FLASH_CS=1;
	//while(!FlashReadWEL());
	FLASH_CS=0;
	spi1_read_write(0x02);
	spi1_read_write((addr>>24)&0xFF);
	spi1_read_write((addr>>16)&0xFF);
	spi1_read_write((addr>>8)&0xFF);
	spi1_read_write(addr&0xFF);
	flash_dma_busy=1;
	//SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
	timer=0;
	DMA_Cmd(DMA1_Channel3,ENABLE);	
	if(wait)
	{		
		while(flash_dma_busy) timer++;
		//printf("dma complete.%d\r\n",timer);
	}
}

u32 FlashGetAddr(u32 sector,u8 page,u8 byte)
{
#ifndef IAS_V2_BOARD
	return 0;
#endif
	return (sector<<12)|(page<<8)|byte;
}

void FlashWaitDMA(void)
{
#ifndef IAS_V2_BOARD
	return;
#endif
	while(flash_dma_busy);
}

void DMA1_Channel3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC3))
	{
		DMA_ClearITPendingBit(DMA1_IT_TC3);
		DMA_Cmd(DMA1_Channel3,DISABLE);	
		//SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,DISABLE);
		FLASH_CS=1;
		flash_dma_busy=0;
	}
}