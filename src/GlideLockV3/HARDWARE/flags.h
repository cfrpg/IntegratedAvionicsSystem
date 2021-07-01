#ifndef __FLAGS_H
#define __FLAGS_H

#include "sys.h"

#define INVALID -1

#define ARMED 1
#define DISARMED 0

#define ACTIVATED 1
#define DEACTIVATED 0

#define NORMAL 0
#define CONST_UPTIME 1
#define CONST_RATIO 2

typedef struct
{
	s8 isarmed;
	s8 glideLock;
	s8 rollTest;
	s8 strokeType;
	s8 arming;
} SysState;

extern SysState sysState;

#endif
