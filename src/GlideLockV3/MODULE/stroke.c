#include "stroke.h"
#include "pwm.h"
#include "glidelock.h"
#include "parameter.h"
#include "math.h"
#include "mathHelper.h"

float strokeThro;

const float* upstrokeThro;
const float* upstrokeRatio;
const float* downstrokeRatio;
const float* stk_thro_min;
const float* stk_thro_max;
s32* def_stk_type;

const float* up_exp;
const float* down_exp;
const float* x_low;
const float* x_high;
const float* up_low;
const float* up_high;
const float* down_low;
const float* down_high;


void StrokeInit(void)
{
	strokeThro=0;
	upstrokeThro=ParamGetFromName("THRO_UP");
	upstrokeRatio=ParamGetFromName("RATIO_UP");
	downstrokeRatio=ParamGetFromName("RATIO_DOWN");
	stk_thro_min=ParamGetFromName("STK_THRO_MIN");
	stk_thro_max=ParamGetFromName("STK_THRO_MAX");
	def_stk_type=ParamGetFromName("STROKE_TYPE");
	up_exp=ParamGetFromName("STK_UP_EXP");
	down_exp=ParamGetFromName("STK_DOWN_EXP");
	x_low=ParamGetFromName("STK_XPOS_LOW");
	x_high=ParamGetFromName("STK_XPOS_HIGH");
	up_low=ParamGetFromName("STK_UP_LOW");
	up_high=ParamGetFromName("STK_UP_HIGH");
	down_low=ParamGetFromName("STK_DOWN_LOW");
	down_high=ParamGetFromName("STK_DOWN_HIGH");
	sysState.strokeType=*def_stk_type;
}

float strokeConstUptime(void)
{
	if(GLState.phase==0)
	{
		//Up stroke
		return *upstrokeThro;
	}
	else
	{
		return GLState.output[GL_THRO];
	}
}

float strokeConstRatio(void)
{	
	
	if(GLState.phase==0)
	{
		//Up stroke
		return GLState.output[GL_THRO]*(*upstrokeRatio);				
	}
	else
	{
		return GLState.output[GL_THRO]*(*downstrokeRatio);		
	}	
}

float strokeDualExpo(void)
{
	if(GLState.phase==0)
	{
		//Up stroke
		//printf("%f\r\n", powf(GLState.output[GL_THRO],*up_exp));
		return powf(GLState.output[GL_THRO],*up_exp);
	}
	else
	{
		return powf(GLState.output[GL_THRO],*down_exp);	
	}	
	return GLState.output[GL_THRO];
}

float strokeFourPoints(void)
{
	if(GLState.phase==0)
	{
		//Up stroke
		if(GLState.output[GL_THRO]>*x_high)
		{
			return Lerp(*x_high,*up_high,1,1,GLState.output[GL_THRO]);
		}
		else if(GLState.output[GL_THRO]>*x_low)
		{
			return Lerp(*x_low,*up_low,*x_high,*up_high,GLState.output[GL_THRO]);
		}
		else
		{
			return Lerp(0,0,*x_low,*up_low,GLState.output[GL_THRO]);
		}
			
	}
	else
	{
		if(GLState.output[GL_THRO]>*x_high)
		{
			return Lerp(*x_high,*down_high,1,1,GLState.output[GL_THRO]);
		}
		else if(GLState.output[GL_THRO]>*x_low)
		{
			return Lerp(*x_low,*down_low,*x_high,*down_high,GLState.output[GL_THRO]);
		}
		else
		{
			return Lerp(0,0,*x_low,*down_low,GLState.output[GL_THRO]);
		}
	}	
}

void StrokeUpdate(void)
{
	float thro;
	if(sysState.strokeType==NORMAL)
	{
//		strokeThro=pwmNorm[CH_THRO];
		strokeThro=GLState.output[GL_THRO];
	}
//	if(sysState.strokeType==CONST_UPTIME)
//	{
//		thro=strokeConstUptime();
//	}
//	if(sysState.strokeType==CONST_RATIO)
//	{
//		thro=strokeConstRatio();
//	}
	if(sysState.strokeType==DUAL_EXPO)
	{
		strokeThro=strokeDualExpo();
	}
	if(sysState.strokeType==FOUR_POINTS)
	{
		strokeThro=strokeFourPoints();
	}
	if(sysState.strokeType!=NORMAL)
	{
		if(strokeThro<*stk_thro_min)
			strokeThro=*stk_thro_min;
		if(strokeThro>*stk_thro_max)
			strokeThro=*stk_thro_max;
	}
}

void StrokeSetDefType(void)
{
	*def_stk_type=sysState.strokeType;
	ParamWrite();
}
