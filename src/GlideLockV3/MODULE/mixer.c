#include "mixer.h"
#include "pwm.h"
#include "glidelock.h"
#include "parameter.h"
#include "stroke.h"

u16 mixerResult[4];

const float* thro_slope;
float curr_thro;
float thro_tgt;
	

void MixerInit(void)
{
	thro_slope=ParamGetFromName("THRO_SLOPE");
	curr_thro=1000;
	thro_tgt=1000;
}

void MixerUpdate(void)
{
	//default:pass through
	mixerResult[OUT_THRO]=pwmValues[CH_THRO];
	mixerResult[OUT_LAILE]=pwmValues[CH_LAILE];
	mixerResult[OUT_RAILE]=pwmValues[CH_RAILE];
	mixerResult[OUT_ELEV]=pwmValues[CH_ELEV];
	
	//mix thro
	if(sysState.glideLock==ACTIVATED && GLState.working)
	{
		thro_tgt=(float)GLState.pwmBackup[0];
	}
	else
	{
		thro_tgt=(float)strokeThro;
	}
	//thro slope
	curr_thro+= *thro_slope;
	if(curr_thro>thro_tgt)
		curr_thro=thro_tgt;
	mixerResult[OUT_THRO]=(u16)curr_thro;
	
	//mix alie
	if(sysState.rollTest==ACTIVATED)
	{
		mixerResult[OUT_LAILE]=GLState.pwmBackup[1];
		mixerResult[OUT_RAILE]=GLState.pwmBackup[2];
	}
	
	//mix elev
	//do nothing
	
}
