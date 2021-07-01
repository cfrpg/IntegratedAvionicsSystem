#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

#define CH_THRO 0
#define CH_LAILE 1
#define CH_RAILE 2
#define CH_GLOCK 3
#define CH_ROLLTEST 4
#define CH_ROLL_STR 5
#define CH_STROKE_MODE 6
#define CH_ELEV 7

#define OUT_THRO 0
#define OUT_LAILE 1
#define OUT_RAILE 2
#define OUT_ELEV 3

typedef struct
{
	s32 value;
	u32 tmp;
	u16 upv;
	u16 downv;
	u8 STA;	
	u8 flag;
} TIMCCState;

typedef struct
{
	u8 currChannel;
	u8 state;
	u32 lastVal;
	u32 currVal;
	s32 value;
} PPMState;

typedef struct
{
	s32 center;
	s32 min;
	s32 max;
	s32 range;
	s32 halfRange;
} PWMParams;

//extern TIMCCState pwmState[6];
extern s32 pwmValues[8];
extern PWMParams pwmParams;

void PWMInit(u32 freq);
void PWMInInit(void);
void PWMSet(float out[4]);
void PWMLock(u16 v);
void PWMSetVal(u8 ch,u16 val);

#endif
