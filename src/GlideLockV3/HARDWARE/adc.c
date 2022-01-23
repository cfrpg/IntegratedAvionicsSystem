#include "adc.h"

u32 adcbuff[8];
#ifdef IAS_V2_BOARD
void ADCInit(void)
{
	GPIO_InitTypeDef gi;
	ADC_InitTypeDef ai;
	DMA_InitTypeDef di;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_ADC1|RCC_APB2Periph_ADC2,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	gi.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_0;
	gi.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_Init(GPIOB,&gi);
	
	DMA_DeInit(DMA1_Channel1);
	di.DMA_PeripheralBaseAddr=(uint32_t)0x4001244C;
	di.DMA_MemoryBaseAddr=(u32)adcbuff;
	di.DMA_DIR=DMA_DIR_PeripheralSRC;
	di.DMA_BufferSize=8;
	di.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	di.DMA_MemoryInc=DMA_MemoryInc_Enable;
	di.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Word;
	di.DMA_MemoryDataSize=DMA_MemoryDataSize_Word;
	di.DMA_Mode=DMA_Mode_Normal;
	di.DMA_Priority=DMA_Priority_High;
	di.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1,&di);
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	ADC_DeInit(ADC1);
	ADC_DeInit(ADC2);
	
	ai.ADC_Mode=ADC_Mode_RegSimult;
	ai.ADC_ScanConvMode=ENABLE;
	ai.ADC_ContinuousConvMode=ENABLE;
	ai.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	ai.ADC_DataAlign=ADC_DataAlign_Right;
	ai.ADC_NbrOfChannel=1;
	ADC_Init(ADC1,&ai);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_8,1,ADC_SampleTime_28Cycles5);
	ADC_DMACmd(ADC1,ENABLE);
	
	ai.ADC_Mode=ADC_Mode_RegSimult;
	ai.ADC_ScanConvMode=ENABLE;
	ai.ADC_ContinuousConvMode=ENABLE;
	ai.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	ai.ADC_DataAlign=ADC_DataAlign_Right;
	ai.ADC_NbrOfChannel=1;
	ADC_Init(ADC2,&ai);
	ADC_RegularChannelConfig(ADC2,ADC_Channel_9,1,ADC_SampleTime_28Cycles5);
	ADC_ExternalTrigConvCmd(ADC2,ENABLE);
	
	
	ADC_Cmd(ADC1,ENABLE);
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_Cmd(ADC2,ENABLE);	
	ADC_ResetCalibration(ADC2);
	while(ADC_GetResetCalibrationStatus(ADC2));
	ADC_StartCalibration(ADC2);
	while(ADC_GetCalibrationStatus(ADC2));
	

}

void ADCReadFiltered(u16 val[2])
{
	u32 res=0;
	u8 i;	
	ADC_Cmd(ADC1,ENABLE);
	DMA_Cmd(DMA1_Channel1,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel1,8);
	DMA_Cmd(DMA1_Channel1,ENABLE);
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	DMA_ClearFlag(DMA1_FLAG_TC1);
	ADC_Cmd(ADC1,DISABLE);
	for(i=0;i<8;i++)
	{
		res+=adcbuff[i];		
	}	
	val[0]=res>>19;
	val[1]=(res&0xFFFF)>>3;
}
#else
void ADCInit(void)
{
	return;
}
void ADCReadFiltered(u16 val[2])
{
	val[0]=0;
	val[1]=0;
}
#endif

