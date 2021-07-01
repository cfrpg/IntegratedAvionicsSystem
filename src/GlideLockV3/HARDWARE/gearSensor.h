#ifndef __GEARSENSOR_H
#define __GEARSENSOR_H

#include "sys.h"

typedef struct
{
	s32 counter;
	s32 freqCounter;
	s32 peroid[3];
	s32 rollValue;
	float throVal;
	u16 pwmBackup[4];	
	u16 throTgt;	
	u8 mode;	
	u8 hallState[4];
	u8 GLEnabled;
	u8 rollEnabled;
	s8 phase;
	
} StrokeState;

extern StrokeState GSState;

void GSInit(void);
void GSUpdate(void);

#endif