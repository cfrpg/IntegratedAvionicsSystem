#ifndef __LOGGER_H
#define __LOGGER_H
#include "sys.h"

#define Logger_Idle 0
#define Logger_Logging 1
#define Logger_Reading 2

#define STX_Blank 0xFF
#define STX_File 0xA0
#define STX_Package 0x0A

typedef struct
{
	u16 stx;		//0-1
	u16 id;			//2-3
	u32 length;		//4-7
	s8 name[24];	//8-31
	u8 padding[31];	//32-62
	u8 tailFlag;	//63
} LogHeader;

typedef struct
{
	u8 stx;			//0
	u8 rev[3];		//1-3
	u32 id;			//4-7
	u8 hallTime[4];	//8-11 
	u16 adc[2];		//12-15
	u16 pwmin[8];	//16-31
	u16 pwmout[4];	//32-39
	u8 padding[23];	//40-62
	u8 tailFlag;	//63
} LogPackage;

typedef struct
{
	u8 state;
	u16 nextID;
	s32 headSector;
	s32 tailSector;
	LogPackage pkgBuff[4];
	u8 pkgHead;
	u8 pkgTail;
	u16 currPkgID;
	u32 currAddr;
	u32 length;
	u32 headAddr;
} LoggerState;

extern LoggerState loggerState;

void LoggerInit(void);
void LoggerCreate(void);
void LoggerClose(void);
void LoggerWrite(void);
void LoggerGetList(void);
void LoggerRead(void);

#endif

