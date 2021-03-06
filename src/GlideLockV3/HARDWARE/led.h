#ifndef __LED_H
#define __LED_H
#include "sys.h"

#ifdef IAS_V2_BOARD
#define LED_R PAout(6)
#define LED_G PAout(5)
#define LED_B PAout(4)
#else
#define LED_R PBout(3)
#define LED_G PBout(4)
#define LED_B PBout(5)
#endif


#define LED_FLASH_T 100

#define LED_1Hz 0x0F83E0
#define LED_2Hz 0x0E739C
#define LED_5Hz 0x0AAAAA
#define LED_Half 0x0FFC00
#define LED_ON 0x000000
#define LED_OFF 0xFFFFFF


void LEDInit(void);
void LEDSet(u8 v);
void LEDFlip(void);
void LEDFlash(u8 t);
void LEDSetRGB(u8 r,u8 g,u8 b);
void LEDSetPattern(u32 r,u32 g,u32 b);
void LEDUpdate();


#endif
