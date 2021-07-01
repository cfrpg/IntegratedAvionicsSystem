#include "gearSensor.h"

StrokeState GSState;

void GSInit(void)
{
	GPIO_InitTypeDef gi;
	EXTI_InitTypeDef ei;
	NVIC_InitTypeDef ni;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	
	
	gi.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	gi.GPIO_Mode=GPIO_Mode_IPU;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gi);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource15);
	
	ei.EXTI_Line=EXTI_Line12;
	ei.EXTI_Mode=EXTI_Mode_Interrupt;
	ei.EXTI_Trigger=EXTI_Trigger_Falling;
	ei.EXTI_LineCmd=ENABLE;
	EXTI_Init(&ei);
	ei.EXTI_Line=EXTI_Line13;
	EXTI_Init(&ei);
	ei.EXTI_Line=EXTI_Line14;
	EXTI_Init(&ei);
	ei.EXTI_Line=EXTI_Line15;
	EXTI_Init(&ei);
	
	ni.NVIC_IRQChannel=EXTI15_10_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=1;
	ni.NVIC_IRQChannelSubPriority=2;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);
	
	GSState.hallState[0]=0;
	GSState.hallState[1]=0;
	GSState.hallState[2]=0;
	GSState.hallState[3]=0;	
	
	GSState.counter=0;
	GSState.freqCounter=0;
	
	GSState.phase=-1;
}

void GSUpdate(void)
{
	
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET)
	{
		
		EXTI_ClearITPendingBit(EXTI_Line12);
		GSState.hallState[0]=1;		
		//printf("12\r\n");
	}
	if(EXTI_GetITStatus(EXTI_Line13)!=RESET)
	{
		
		EXTI_ClearITPendingBit(EXTI_Line13);
		GSState.hallState[1]=1;
		//printf("13\r\n");
	}
	if(EXTI_GetITStatus(EXTI_Line14)!=RESET)
	{
		
		EXTI_ClearITPendingBit(EXTI_Line14);
		GSState.hallState[2]=1;
		GSState.peroid[2]=GSState.peroid[1];
		GSState.peroid[1]=GSState.freqCounter;
		GSState.peroid[0]=(GSState.peroid[1]+GSState.peroid[2])>>1;		
		//printf("14\r\n");
	}
	if(EXTI_GetITStatus(EXTI_Line15)!=RESET)
	{		
		EXTI_ClearITPendingBit(EXTI_Line15);
		GSState.hallState[3]=1;
		//printf("15\r\n");
	}

}
