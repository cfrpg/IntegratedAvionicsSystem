#include "stm32f10x.h"
#include "string.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "math.h"
#include "parameter.h"
#include "led.h"
#include "pwm.h"
#include "key.h"
#include "glideLock.h"
#include "flags.h"
#include "stroke.h"
#include "mixer.h"

void AnalyzePkg(void);
void MainWork(void);
void InitTestPin(void);
void printLogo(void);
void updateState(void);
void initState(void);

u16 tick[4]={0,0,0,0};
u16 cpucnt;
u16 mainWorkRate;

SysState sysState;

u8 state;
u8 subState;
u8 keyState;
u16 ledInterval;
u8 ledFlash;

u8 caliCnt;
u8 caliNext;
float caliTmp[3];

float pwmOut[4];

s8 armed;
s8 dir;
s8 position;

const s32* pwm_rate;
const s32* roll_step_val;

int main(void)
{	
	u8 i;
	
	u8 ledr=1,ledg=1,ledb=0;
	
	u8 key;
	
	u16 t;
	
	//Init drivers
	delay_init();
	NVIC_Configuration();
	uart_init(115200);	
	LEDInit();	
	KeyInit();
	USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	
	printLogo();	
	//Load parameters
	printf("Loading parameters...\r\n");	
	ParamRead();
	if(params.headFlag!=0xCFCF||params.tailFlag!=0xFCFC)
	{
		ParamReset();
		ParamWrite();
		printf("Reset parameters.\r\n");		
	}
	
	pwm_rate=ParamGetFromName("PWM_RATE");
	roll_step_val=ParamGetFromName("ROLL_STEP_VAL");
	
	//Init modules
	LEDSetRGB(ledr,ledg,ledb);
	LEDSet(1);	
		
	MainClockInit();
	
	PWMInit(*pwm_rate);
	
	PWMInInit();
	
	delay_ms(50);
	ledInterval=5000;
	//ledFlash=1;
	
	GLInit();
	StrokeInit();
	MixerInit();
	
	
//	mainWorkRate=10000/params.pwm_rate;
	mainWorkRate=10000/200;
	state=0;
	PWMLock(900);
	LEDSet(1);

//	if(params.pwm_rate<=100)
//		ledInterval=2500;
//	else if(params.pwm_rate<=250)
//		ledInterval=5000;
//	else
//		ledInterval=10000;
	ledr=1;
	ledg=0;
	ledb=1;	
	LEDSetRGB(ledr,ledg,ledb);
	while(1)
	{	
		//main work
		//200Hz
		if(tick[2]>=mainWorkRate)
		{
			tick[2]=0;
			updateState();
			
			if(sysState.isarmed==ARMED)			
			{
				GLUpdate(sysState.glideLock,sysState.rollTest);
				StrokeUpdate();
				MixerUpdate();
				PWMSetVal(0,mixerResult[0]);
				PWMSetVal(1,mixerResult[1]);
				PWMSetVal(2,mixerResult[2]);
				PWMSetVal(3,mixerResult[3]);
			}
			else
			{
				PWMSetVal(OUT_THRO,900);
				PWMSetVal(OUT_LAILE,(u16)pwmValues[CH_LAILE]);
				PWMSetVal(OUT_RAILE,(u16)pwmValues[CH_RAILE]);
				PWMSetVal(OUT_ELEV,(u16)pwmValues[CH_ELEV]);				
			}
			sysState.arming=0;

//			if(state)
//			{				
//				if(pwmValues[0]<=950)
//				{
//					state=0;
//					PWMSetVal(0,900);
//					
//					//ledFlash=0;
//				}
//				else
//				{										
//					if(pwmValues[4]>1500)
//						t1=1;
//					GLUpdate(t,t1);
//					PWMSetVal(OUT_THRO,GLState.pwmBackup[0]);
//					PWMSetVal(1,GLState.pwmBackup[1]);
//					PWMSetVal(2,GLState.pwmBackup[2]);
//				}
//				
//			}
//			else
//			{
//				if(pwmValues[0]>950)
//				{
//					state=1;
//					GLState.throTgt=1000;
//					GLState.throVal=1000;
//					//LEDSet(1);
//					//ledFlash=1;
//				}
//				PWMSetVal(0,900);
//				PWMSetVal(1,(u16)pwmValues[1]);
//				PWMSetVal(2,(u16)pwmValues[2]);
//				GLUpdate(0,0);
//				if(GLState.mode)
//				{
//					if(GLState.pwmBackup[0]<1200)
//						PWMSetVal(0,1200);
//					else
//						PWMSetVal(0,GLState.pwmBackup[0]);
//				}
//			}
//			
//			//printf("---\r\n");
//			//printf("%d,%d,%d,%d,%d,%d\r\n",pwmValues[0],pwmValues[1],pwmValues[2],pwmValues[3],pwmValues[4],pwmValues[5]);
		}				
		//LED
		if(tick[0]>ledInterval)
		{			
			LEDFlip();
			tick[0]=0;
		}
		if(tick[1]>5000)
		{						
			tick[1]=0;	
			if(USART_RX_STA&0x8000)
			{				
				AnalyzePkg();
				USART_RX_STA=0;
			}
			key|=KeyRead();
			if((key&0x7)==3)
			{
				GLState.mode^=1;
				if(GLState.mode)
				{
					ledr=0;
					ledg=1;
					ledb=1;					
				}
				else
				{
					ledr=1;
					ledg=0;
					ledb=1;		
				}
				LEDSetRGB(ledr,ledg,ledb);
				LEDFlash(3);				
			}
			key<<=1;
//			printf("%d %f\r\n",GLState.peroid[0],10000.0/GLState.peroid[0]);
//			for(i=0;i<8;i++)
//				printf("%d,",pwmValues[i]);
//			printf("\r\n");
//			printf("%d\r\n",t);

		}		
	}
}



void AnalyzePkg(void)
{
	u8 len=USART_RX_STA&0x3FFF;
	u8 t,i;
	s32 v;
	
	if(USART_RX_BUF[0]=='s'&&USART_RX_BUF[1]=='e'&&USART_RX_BUF[2]=='t')
	{
		printf("SET CMD\r\n");
		if(len!=14)
		{
			printf("Invalid length:%d.\r\n",len);		
			return;
		}
		t=(USART_RX_BUF[3]-'0')*10+(USART_RX_BUF[4]-'0');
		printf("ID:%d\r\n",t);
		v=0;
		for(i=0;i<8;i++)
		{
			v*=10;
			v+=USART_RX_BUF[6+i]-'0';
		}
		if(USART_RX_BUF[5]=='-')
			v*=-1;
		printf("Value:%d\r\n",v);
		if(ParamSet(t,v)==0)
		{
			printf("Complete.\r\n");
			LEDFlash(5);
		}
		else
		{
			printf("Failed.\r\n");
			LEDFlash(3);
		}
		return;
	}
	if(USART_RX_BUF[0]=='s'&&USART_RX_BUF[1]=='h'&&USART_RX_BUF[2]=='o'&&USART_RX_BUF[3]=='w')
	{
		printf("SHOW CMD\r\n");
		ParamShow();
	}
	if(USART_RX_BUF[0]=='r'&&USART_RX_BUF[1]=='s'&&USART_RX_BUF[2]=='t'&&USART_RX_BUF[3]=='p')
	{
		printf("RSTP CMD\r\n");
		ParamReset();
		ParamWrite();
		printf("Param reset complete.\r\n");
		ParamShow();
	}
}

void initState(void)
{
	sysState.isarmed=DISARMED;
	sysState.glideLock=DEACTIVATED;
	sysState.rollTest=DEACTIVATED;
	sysState.strokeType=NORMAL;
	sysState.arming=0;
}

void updateState(void)
{
	//Select roll strength
	if(pwmValues[CH_ROLL_STR]<1300)
		GLState.rollValue=*roll_step_val;
	else if(pwmValues[CH_ROLL_STR]<1600)
		GLState.rollValue=(*roll_step_val)<<1;
	else
		GLState.rollValue=(*roll_step_val)*3;
	
	//Check glide lock	
	if(pwmValues[CH_GLOCK]>1500)
	{
		sysState.glideLock=ACTIVATED;
		ledInterval=3000;
	}
	else
	{
		sysState.glideLock=DEACTIVATED;
		ledInterval=10000;
	}
	
	//Check roll test
	if(pwmValues[CH_ROLLTEST]>1500)
		sysState.rollTest=ACTIVATED;
	else
		sysState.rollTest=DEACTIVATED;
	
	//Check is armed
	if(sysState.isarmed==ARMED)
	{
		if(pwmValues[CH_THRO]<950)
		{
			sysState.isarmed=DISARMED;
		}
	}
	else
	{
		if(pwmValues[CH_THRO]>950)
		{
			sysState.isarmed=ARMED;
			sysState.arming=1;		
		}
	}
}


void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		u8 n=4;
		while(n--)
		{
			tick[n]++;
		}
		GLState.counter++;
		GLState.freqCounter++;
	}
}

void InitTestPin(void)
{
	GPIO_InitTypeDef gi;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_4;
	gi.GPIO_Mode=GPIO_Mode_Out_PP;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gi);
}

void printLogo(void)
{
//printf(" _____  _____ _______ ______ _____ \r\n");
//printf("|  __ \\|  __ \\__   __|  ____/ ____|\r\n");
//printf("| |__) | |__) | | |  | |__ | |     \r\n");
//printf("|  ___/|  ___/  | |  |  __|| |     \r\n");
//printf("| |    | |      | |  | |   | |____ \r\n");
//printf("|_|    |_|      |_|  |_|    \\_____|\r\n");
	
printf("         _          _        _            _           _      \r\n");
printf("        /\\ \\       /\\ \\     /\\ \\         /\\ \\       /\\ \\     \r\n");
printf("       /  \\ \\     /  \\ \\    \\_\\ \\       /  \\ \\     /  \\ \\    \r\n");
printf("      / /\\ \\ \\   / /\\ \\ \\   /\\__ \\     / /\\ \\ \\   / /\\ \\ \\   \r\n");
printf("     / / /\\ \\_\\ / / /\\ \\_\\ / /_ \\ \\   / / /\\ \\_\\ / / /\\ \\ \\  \r\n");
printf("    / / /_/ / // / /_/ / // / /\\ \\ \\ / /_/_ \\/_// / /  \\ \\_\\ \r\n");
printf("   / / /__\\/ // / /__\\/ // / /  \\/_// /____/\\  / / /    \\/_/ \r\n");
printf("  / / /_____// / /_____// / /      / /\\____\\/ / / /          \r\n");
printf(" / / /      / / /      / / /      / / /      / / /________   \r\n");
printf("/ / /      / / /      /_/ /      / / /      / / /_________\\  \r\n");
printf("\\/_/       \\/_/       \\_\\/       \\/_/       \\/____________/ \r\n");
printf("                           by\r\n");
printf("               _____ ____ ___   ___   _____\r\n");
printf("              / ___// __// _ \\ / _ \\ / ___/\r\n");
printf("             / /__ / _/ / , _// ___// (_ / \r\n");
printf("             \\___//_/  /_/|_|/_/    \\___/ \r\n");
}
