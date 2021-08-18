#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

#define CH_LAILE 0
#define CH_RAILE 1
#define CH_ELEV 2
#define CH_THRO 3
#define CH_GLOCK 4
#define CH_ROLLTEST 5
#define CH_ROLL_STR 6
#define CH_STROKE_MODE 7

#define OUT_LAILE 0
#define OUT_RAILE 1
#define OUT_ELEV 2
#define OUT_THRO 3

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
extern float pwmNorm[8];

void PWMInit(u32 freq);
void PWMInInit(void);
void PWMSet(float out[4]);
void PWMLock(u16 v);
void PWMSetVal(u8 ch,u16 val);

#endif
