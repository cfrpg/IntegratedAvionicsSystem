#include "parameter.h"

const ParameterRecord parameterList[PARAM_NUM]=
{
//Type	Name				default value			min value				max value
	0,	"PWM_RATE",			{.intValue=		50},	{.intValue=		50},	{.intValue=		450},
	5,	"THRO_MIN",			{.floatValue=	0.4},	{.floatValue=	0},		{.floatValue=	1},
	5,	"THRO_HOMING",		{.floatValue=	0.35},	{.floatValue=	0},		{.floatValue=	1},
	0,	"DEAD_ZONE",		{.intValue=		20},	{.intValue=		0},		{.intValue=		1000},
	5,	"ROLL_DELAY",		{.floatValue=	0.5},	{.floatValue=	0},		{.floatValue=	2},
	5,	"ROLL_STEP_VAL",	{.floatValue=	0.1},	{.floatValue=	0},		{.floatValue=	0.33},
	5,	"ROLL_SPAN",		{.floatValue=	0.5},	{.floatValue=	0},		{.floatValue=	10},
	5,	"THRO_SLOPE",		{.floatValue=	1.5},	{.floatValue=	0},		{.floatValue=	1000},
	-1,	"INPUT_REV",		{.intValue=		0},		{.intValue=		0},		{.intValue=		1024},
	-1,	"OUTPUT_REV",		{.intValue=		0},		{.intValue=		0},		{.intValue=		1024},
	5,	"THRO_UP",			{.floatValue=	0.5},	{.floatValue=	0},		{.floatValue=	1.0},
	5,	"RATIO_UP",			{.floatValue=	1},		{.floatValue=	0},		{.floatValue=	10},
	5,	"RATIO_DOWN",		{.floatValue=	1},		{.floatValue=	0},		{.floatValue=	10},
	5,	"STK_THRO_MIN",		{.floatValue=	0.3},	{.floatValue=	0},		{.floatValue=	1.0},
	5,	"STK_THRO_MAX",		{.floatValue=	1.0},	{.floatValue=	0},		{.floatValue=	1.0},
	0,	"STROKE_TYPE",		{.intValue=		0},		{.intValue=		0},		{.intValue=		2},
	5,	"STK_UP_EXP",		{.floatValue=	1},		{.floatValue=	0},		{.floatValue=	100},
	5,	"STK_DOWN_EXP",		{.floatValue=	1},		{.floatValue=	0},		{.floatValue=	100},
	5,	"STK_XPOS_LOW",		{.floatValue=	0.2},	{.floatValue=	0},		{.floatValue=	1},
	5,	"STK_XPOS_HIGH",	{.floatValue=	0.7},	{.floatValue=	0},		{.floatValue=	1},
	5,	"STK_UP_LOW",		{.floatValue=	0.2},	{.floatValue=	0},		{.floatValue=	1},
	5,	"STK_UP_HIGH",		{.floatValue=	0.7},	{.floatValue=	0},		{.floatValue=	1},
	5,	"STK_DOWN_LOW",		{.floatValue=	0.2},	{.floatValue=	0},		{.floatValue=	1},
	5,	"STK_DOWN_HIGH",	{.floatValue=	0.7},	{.floatValue=	0},		{.floatValue=	1}
};
