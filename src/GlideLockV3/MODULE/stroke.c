#include "stroke.h"
#include "pwm.h"
#include "glidelock.h"
#include "parameter.h"

u16 strokeThro;

const s32* upstrokeThro;
const float* upstrokeRatio;
const float* downstrokeRatio;
const s32* stk_thro_min;
const s32* stk_thro_max;

void StrokeInit(void)
{
	strokeThro=1000;
	upstrokeThro=ParamGetFromName("THRO_UP");
	upstrokeRatio=ParamGetFromName("RATIO_UP");
	downstrokeRatio=ParamGetFromName("RATIO_DOWN");
	stk_thro_min=ParamGetFromName("STK_THRO_MIN");
	stk_thro_max=ParamGetFromName("STK_THRO_MAX");
}

void StrokeUpdate(void)
{
	s32 thro;
	if(sysState.strokeType==NORMAL)
	{
		thro=pwmValues[CH_THRO];
	}
	if(sysState.strokeType==CONST_UPTIME)
	{
		if(GLState.phase==0)
		{
			//Up stroke
			thro= *upstrokeThro;
		}
		else
		{
			thro=pwmValues[CH_THRO];
		}
	}
	if(sysState.strokeType==CONST_RATIO)
	{
		thro=pwmValues[CH_THRO]-1000;
		if(thro<0)
			thro=0;
		if(GLState.phase==0)
		{
			//Up stroke
			thro*= *upstrokeRatio;			
		}
		else
		{
			thro*= *downstrokeRatio;		
		}
		thro+=1000;
	}
	if(thro<*stk_thro_min)
		thro=*stk_thro_min;
	if(thro>*stk_thro_max)
		thro=*stk_thro_max;
	
	strokeThro=(u16)thro;
}
