#include "parameter.h"

const ParameterRecord parameterList[PARAM_NUM]=
{
//Type	Name				default value			min value				max value
	0,	"PWM_RATE",			{.intValue=		50},	{.intValue=		50},	{.intValue=		450},
	0,	"THRO_MIN",			{.intValue=		1400},	{.intValue=		0},		{.intValue=		2000},
	0,	"THRO_HOMING",		{.intValue=		1350},	{.intValue=		0},		{.intValue=		2000},
	0,	"DEAD_ZONE",		{.intValue=		20},	{.intValue=		0},		{.intValue=		1000},
	5,	"ROLL_DELAY",		{.floatValue=	0.5},	{.floatValue=	0},		{.floatValue=	2},
	0,	"ROLL_STEP_VAL",	{.intValue=		100},	{.intValue=		0},		{.intValue=		1000},
	5,	"ROLL_SPAN",		{.floatValue=	0.5},	{.floatValue=	0},		{.floatValue=	10},
	5,	"THRO_SLOPE",		{.floatValue=	1.5},	{.floatValue=	0},		{.floatValue=	1000},
	-1,	"INPUT_REV",		{.intValue=		0},		{.intValue=		0},		{.intValue=		1024},
	-1,	"OUTPUT_REV",		{.intValue=		0},		{.intValue=		0},		{.intValue=		1024},
	0,	"THRO_UP",			{.intValue=		1500},	{.intValue=		1000},	{.intValue=		2000},
	5,	"RATIO_UP",			{.intValue=		1},		{.intValue=		0},		{.intValue=		10},
	5,	"RATIO_DOWN",		{.intValue=		1},		{.intValue=		0},		{.intValue=		10},
	0,	"STK_THRO_MIN",		{.intValue=		1300},	{.intValue=		1000},	{.intValue=		2000},
	0,	"STK_THRO_MAX",		{.intValue=		2000},	{.intValue=		1000},	{.intValue=		2000},
};
