#include "glideLock.h"
#include "stdio.h"
#include "pwm.h"
#include "parameter.h"

const float* thro_min;
const float* thro_homing;
const float* roll_delay;
const float* roll_span;

u8 glstate;
u8 rollstate;
GlideLockState GLState;
s32 rollpos1,rollpos2;

void GLInit(void)
{
	thro_min=ParamGetFromName("THRO_MIN");
	thro_homing=ParamGetFromName("THRO_HOMING");
	roll_delay=ParamGetFromName("ROLL_DELAY");
	roll_span=ParamGetFromName("ROLL_SPAN");

	
	//Init Exti
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
	
	glstate=2;
	
//	GLState.pwmBackup[0]=0;
//	GLState.pwmBackup[1]=0;
//	GLState.pwmBackup[2]=0;
//	GLState.pwmBackup[3]=0;
	
	GLState.output[0]=0;
	GLState.output[1]=0;
	GLState.output[2]=0;
	GLState.output[3]=0;

	
	GLState.GLEnabled=0;
	GLState.rollEnabled=0;
	
	GLState.hallState[0]=0;
	GLState.hallState[1]=0;
	GLState.hallState[2]=0;
	GLState.hallState[3]=0;	
	
	GLState.counter=0;
	GLState.freqCounter=0;
	
	GLState.phase=-1;
	GLState.working=0;
	
	rollstate=0;
}

void updateGlideLock(void)
{
	
	if(glstate==0)
	{
		GLState.working=0;
		//printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaagl0\r\n");
		if(pwmNorm[CH_THRO]<*thro_min)
		{
			glstate=1;
			//GLState.hallState[0]=0;
		}
		else
		{
//			GLState.pwmBackup[0]=(u16)pwmValues[CH_THRO];
			GLState.output[GL_THRO]=pwmNorm[CH_THRO];
		}
	}
	if(glstate==1)
	{		
		//printf("aaaaaaaaaaaaaaaaaagl1\r\n");
		GLState.working=1;
		if(pwmNorm[CH_THRO]>=*thro_min)
		{
			glstate=0;
//			GLState.pwmBackup[0]=(u16)pwmValues[CH_THRO];
			GLState.output[GL_THRO]=pwmNorm[CH_THRO];
		}
		else if(GLState.hallState[2])
		{
			//GLState.hallState[0]=0;
			glstate=2;
		}
		else
		{
//			GLState.pwmBackup[0]=(u16)(*thro_homing);
			GLState.output[GL_THRO]=*thro_homing;
		}
	}
	if(glstate==2)
	{	
		//printf("gl2\r\n");
		GLState.working=1;
		if(pwmNorm[CH_THRO]>=*thro_min)
		{
			//printf("%d\r\n",pwmValues[CH_THRO]);
			glstate=0;
//			GLState.pwmBackup[0]=(u16)pwmValues[CH_THRO];
			GLState.output[GL_THRO]=pwmNorm[CH_THRO];
		}
		else if(GLState.mode)
		{
			if(GLHallInt)
			{
				//printf("a%d\r\n",pwmValues[CH_THRO]);
				//GLState.hallState[0]=0;
				glstate=1;
//				GLState.pwmBackup[0]=(u16)(*thro_homing);
				GLState.output[GL_THRO]=*thro_homing;
			}
		}
		else
		{
			GLState.output[GL_THRO]=0;
		}
	}	
}

void updateRollTest(void)
{
	if(rollstate==0)
	{
		//printf("rt0\r\n");
		if(GLState.hallState[2])
		{
			//GLState.hallState[2]=0;
			rollstate=1;
			GLState.counter=0;
			rollpos1=*roll_delay*GLState.peroid[0];
			rollpos2=*roll_span*GLState.peroid[0]+rollpos1;
		}
		else
		{
//			GLState.pwmBackup[1]=LAileZero;
//			GLState.pwmBackup[2]=RAileZero;
			GLState.output[GL_LAILE]=LAileZero;
			GLState.output[GL_RAILE]=RAileZero;
		}
	}
	if(rollstate==1)
	{
		//printf("rt1\r\n");
		if(GLState.counter>=rollpos1)
		{
			rollstate=2;
		}
		else
		{
//			GLState.pwmBackup[1]=LAileZero;
//			GLState.pwmBackup[2]=RAileZero;
			GLState.output[GL_LAILE]=LAileZero;
			GLState.output[GL_RAILE]=RAileZero;
		}
	}
	if(rollstate==2)
	{
		//printf("rt2\r\n");
		if(GLState.counter>rollpos2)
		{
			rollstate=3;
		}
		else
		{
//			GLState.pwmBackup[1]=(u16)(LAileZero-GLState.rollValue);
//			GLState.pwmBackup[2]=RAileZero;
			GLState.output[GL_LAILE]=LAileZero-GLState.rollValue;
			GLState.output[GL_RAILE]=RAileZero;
		}
	}
	if(rollstate==3)
	{		
//		GLState.pwmBackup[1]=LAileZero;
//		GLState.pwmBackup[2]=RAileZero;
		GLState.output[GL_LAILE]=LAileZero;
		GLState.output[GL_RAILE]=RAileZero;
	}
}

void GLUpdate(s8 glEnabled,s8 rollEnabled)
{
	//default:follow input
//	GLState.pwmBackup[0]=(u16)pwmValues[0];
//	GLState.pwmBackup[1]=(u16)pwmValues[1];
//	GLState.pwmBackup[2]=(u16)pwmValues[2];

	GLState.output[GL_THRO]=pwmNorm[CH_THRO];
	if(glEnabled==ACTIVATED)
	{
		if(!GLState.GLEnabled)
		{		
			glstate=0;
			GLState.throVal=(u16)pwmValues[CH_THRO];
		}
		updateGlideLock();
//		GLState.throVal+=*thro_slope;
//		if(GLState.throVal>GLState.throTgt)
//			GLState.throVal=GLState.throTgt;
//		GLState.pwmBackup[0]=(u16)GLState.throVal;
	}
	else
	{
//		GLState.pwmBackup[0]=(u16)pwmValues[CH_THRO];
		GLState.output[GL_THRO]=pwmNorm[CH_THRO];
	}
	GLState.GLEnabled=glEnabled;
	
	if(rollEnabled==ACTIVATED)
	{
		if(!GLState.rollEnabled)
		{
			rollstate=0;
		}
		updateRollTest();
	}
	else
	{
//		GLState.pwmBackup[1]=(u16)pwmValues[1];
//		GLState.pwmBackup[2]=(u16)pwmValues[2];
		GLState.output[GL_LAILE]=LAileZero;
		GLState.output[GL_RAILE]=RAileZero;
	}
	
	if(GLState.phase<0)
	{
		if(GLState.hallState[1])
		{
			GLState.phase=1;
		}
		if(GLState.hallState[3])
		{
			GLState.phase=0;
		}
	}
	else if(GLState.phase==0)
	{
		if(GLState.hallState[1])
		{
			GLState.phase=1;
		}
	}
	else if(GLState.phase==1)
	{
		if(GLState.hallState[3])
		{
			GLState.phase=0;
		}
	}
	
//	if(GLState.phase==0)
//	{
//		GLState.pwmBackup[0]=1600;
//	}
	
	GLState.rollEnabled=rollEnabled;
	GLState.hallState[0]=0;
	GLState.hallState[1]=0;
	GLState.hallState[2]=0;
	GLState.hallState[3]=0;
	
	
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET)
	{
		
		EXTI_ClearITPendingBit(EXTI_Line12);
		GLState.hallState[0]=1;		
		//printf("12\r\n");
	}
	if(EXTI_GetITStatus(EXTI_Line13)!=RESET)
	{
		
		EXTI_ClearITPendingBit(EXTI_Line13);
		GLState.hallState[1]=1;
		//printf("13\r\n");
	}
	if(EXTI_GetITStatus(EXTI_Line14)!=RESET)
	{
		
		EXTI_ClearITPendingBit(EXTI_Line14);
		GLState.hallState[2]=1;
		GLState.peroid[2]=GLState.peroid[1];
		GLState.peroid[1]=GLState.freqCounter;
		GLState.peroid[0]=(GLState.peroid[1]+GLState.peroid[2])>>1;		
		//printf("14\r\n");
	}
	if(EXTI_GetITStatus(EXTI_Line15)!=RESET)
	{		
		EXTI_ClearITPendingBit(EXTI_Line15);
		GLState.hallState[3]=1;
		//printf("15\r\n");
	}
	//printf("%d\r\n",GLState.phase);
}

