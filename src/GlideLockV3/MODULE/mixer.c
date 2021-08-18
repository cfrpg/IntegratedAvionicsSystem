#include "mixer.h"
#include "pwm.h"
#include "glidelock.h"
#include "parameter.h"
#include "stroke.h"

u16 mixerResult[4];

const float* thro_slope;
const s32* output_rev;
float curr_thro;
float thro_tgt;
	

void MixerInit(void)
{
	thro_slope=ParamGetFromName("THRO_SLOPE");
	output_rev=ParamGetFromName("OUTPUT_REV");
	curr_thro=0;
	thro_tgt=0;
}

void MixerUpdate(void)
{
	s32 t;
	u8 i;
	//default:pass through
	mixerResult[OUT_THRO]=pwmValues[CH_THRO];
	mixerResult[OUT_LAILE]=pwmValues[CH_LAILE];
	mixerResult[OUT_RAILE]=pwmValues[CH_RAILE];
	mixerResult[OUT_ELEV]=pwmValues[CH_ELEV];
	
	//mix thro
//	if(sysState.glideLock==ACTIVATED && GLState.working)
//	{
//		thro_tgt=GLState.output[GL_THRO];
//	}
//	else
//	{
//		thro_tgt=(float)strokeThro;
//	}
	thro_tgt=(float)strokeThro;
	//thro slope
	curr_thro+= *thro_slope;
	if(curr_thro>thro_tgt)
		curr_thro=thro_tgt;
	mixerResult[OUT_THRO]=(u16)(curr_thro*1000)+1000;
	//printf("%d\r\n",mixerResult[OUT_THRO]);
	
	//mix alie
	if(sysState.rollTest==ACTIVATED)
	{
		mixerResult[OUT_LAILE]=(u16)(GLState.output[GL_LAILE]*500+1500);
		mixerResult[OUT_RAILE]=(u16)(GLState.output[GL_RAILE]*500+1500);
	}
	
	//mix elev
	//do nothing
	
	//reverse output
	t=*output_rev;
	for(i=0;i<4;i++)
	{
		if(t&1)
			mixerResult[i]=3000-mixerResult[i];
		t>>=1;
	}
}
