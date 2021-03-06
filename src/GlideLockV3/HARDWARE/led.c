#include "led.h"
#include "delay.h"


u8 _led_state;
u8 _led_r,_led_g,_led_b;
u32 led_patterns[3];
u32 led_backup[3];
s8 led_phase;
void ledSetOut(void)
{
	if(_led_state)
	{
		LED_R=1;
		LED_G=1;
		LED_B=1;
	}
	else
	{
		LED_R=_led_r;
		LED_G=_led_g;
		LED_B=_led_b;
	}
	
}
void LEDInit(void)
{
	GPIO_InitTypeDef gi;

#ifdef IAS_V2_BOARD
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	
	gi.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	gi.GPIO_Mode=GPIO_Mode_Out_PP;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gi);
#else
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);  
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_3;
	gi.GPIO_Mode=GPIO_Mode_Out_PP;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gi);	
#endif

	_led_state=0;
	_led_r=1;
	_led_g=1;
	_led_b=1;
	
	ledSetOut();
	
	led_patterns[0]=0;
	led_patterns[1]=0;
	led_patterns[2]=0;
	
	led_backup[0]=0;
	led_backup[1]=0;
	led_backup[2]=0;
	
	led_phase=0;	
}



void LEDSet(u8 v)
{
	if(v)
	{
		_led_state=0;		
	}
	else
	{
		_led_state=1;		
	}
	ledSetOut();
}

void LEDFlip(void)
{
	_led_state^=1;
	ledSetOut();
}

void LEDFlash(u8 t)
{
	u8 tmp=_led_state;
	_led_state=1;	
	ledSetOut();
	delay_ms(LED_FLASH_T);	
	while(t--)
	{
		_led_state=0;
		ledSetOut();
		delay_ms(LED_FLASH_T);	
		_led_state=1;
		ledSetOut();
		delay_ms(LED_FLASH_T);		
	}
	_led_state=1;
	ledSetOut();
	delay_ms(LED_FLASH_T);	
	_led_state=tmp;
	ledSetOut();
}

void LEDSetRGB(u8 r,u8 g,u8 b)
{
	_led_r=r;
	_led_g=g;
	_led_b=b;
	ledSetOut();
}

void LEDSetPattern(u32 r,u32 g,u32 b)
{
	led_patterns[0]=r;
	led_patterns[1]=g;
	led_patterns[2]=b;
	led_backup[0]=r;
	led_backup[1]=g;
	led_backup[2]=b;
	led_phase=20;
}

void LEDUpdate()
{
	LED_R=led_backup[0]&0x00000001;
	LED_G=led_backup[1]&0x00000001;
	LED_B=led_backup[2]&0x00000001;
	led_backup[0]>>=1;
	led_backup[1]>>=1;
	led_backup[2]>>=1;
	led_phase--;
	if(!led_phase)
	{
		led_backup[0]=led_patterns[0];
		led_backup[1]=led_patterns[1];
		led_backup[2]=led_patterns[2];
		led_phase=20;
	}
}
