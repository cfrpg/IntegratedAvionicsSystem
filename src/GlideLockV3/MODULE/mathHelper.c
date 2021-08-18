#include "mathHelper.h"

float Lerp(float x0,float y0,float x1,float y1,float x)
{
	return y0+(x-x0)*(y1-y0)/(x1-x0);
}
