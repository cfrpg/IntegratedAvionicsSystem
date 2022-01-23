#include "logger.h"
#include "flash.h"
#include "stdio.h"

LoggerState loggerState;
LogHeader logHeader;

#ifdef IAS_V2_BOARD
void LoggerInit(void)
{
	u8 buf[2];
	u16 pos;
	u16 i;
	u32 addr;
	
	//init vars
	loggerState.state=Logger_Idle;
	loggerState.headSector=-1;
	loggerState.tailSector=-1;
	loggerState.nextID=0;
	
	loggerState.pkgHead=0;
	loggerState.pkgTail=0;
	for(i=0;i<4;i++)
	{
		loggerState.pkgBuff[i].stx=STX_Package;
		loggerState.pkgBuff[i].tailFlag=0x0F;
	}
	
	
	pos=0;
	//find first blank sector
	for(pos=0;pos<FLASH_SECTOR_CNT;pos++)
	{
		addr=i<<12;
		FlashRead(addr,1,buf);
		if(buf[0]==STX_Blank)
		{
			loggerState.tailSector=i;
			break;			
		}
	}
	if(pos==FLASH_SECTOR_CNT)
	{
		
	}
	loggerState.headSector=loggerState.tailSector;
	//find first file
	for(i=0;i<FLASH_SECTOR_CNT;i++)
	{
		addr=((pos+i)&FLASH_SECTOR_MASK)<<12;
		FlashRead(addr,1,buf);
		if(buf[0]!=STX_Blank)
		{
			if(buf[0]==STX_File)
			{
				loggerState.headSector=(pos+i)&FLASH_SECTOR_MASK;
				break;
			}
			else
			{
				//abandoned sector
				FlashEraseSector(addr,1);
			}
		}
	}
	
	//find last file ID
	if(loggerState.headSector==loggerState.tailSector)
	{
		//blank flash
		loggerState.nextID=0;
	}
	else
	{
		for(i=0;i<FLASH_SECTOR_CNT;i++)
		{
			addr=(loggerState.tailSector+FLASH_SECTOR_CNT-i)&FLASH_SECTOR_MASK;
			addr<<=12;
			FlashRead(addr,1,buf);
			if(buf[0]==STX_File)
			{
				FlashRead(addr+2,2,buf);
				loggerState.nextID=buf[1];
				loggerState.nextID<<=8;
				loggerState.nextID|=buf[0];
				loggerState.nextID+=1;
				break;
			}
		}
	}
}

void loggerPrepareNextSector(void)
{
	
}

void loggerWritePackage(void* data)
{
	u8 buf;

	FlashStartDMAWrite((loggerState.tailSector<<12)|loggerState.currAddr,64,data,0);
	loggerState.length+=64;
	loggerState.currAddr+=64;
	if(loggerState.currAddr>=FLASH_SECTOR_SIZE)
	{
		//enter new sector
		loggerState.tailSector++;
		loggerState.tailSector&=FLASH_SECTOR_MASK;
		if(((loggerState.tailSector+1)&FLASH_SECTOR_MASK)==loggerState.headSector)
		{
			//queue full
			loggerState.headSector+=1;
			loggerState.headSector&=FLASH_SECTOR_MASK;
		}
		loggerState.currAddr=0;
		//prepare next sector
		FlashWaitDMA();
		
		//check next sector
		FlashRead(((loggerState.tailSector+1)&FLASH_SECTOR_MASK)<<12,1,&buf);
		if(buf!=STX_Blank)
		{
			//prepare next sector
			FlashEraseSector(((loggerState.tailSector+1)&FLASH_SECTOR_MASK)<<12,0);
		}
	}
}

void LoggerCreate(void)
{
	if(loggerState.state!=Logger_Idle)
	{
		printf("logger busy.\r\n");
		return;
	}
	logHeader.stx=STX_File;
	logHeader.id=loggerState.nextID;
	logHeader.length=0xFFFFFFFF;
	sprintf(logHeader.name,"LOG#%d",logHeader.id);
	loggerState.nextID++;
	loggerState.headAddr=(loggerState.tailSector)<<12;
	loggerState.currAddr=0;
	loggerState.length=0;
	loggerState.state=Logger_Logging;
	loggerState.pkgHead=0;
	loggerState.pkgTail=0;
	loggerWritePackage(&logHeader);
}

void LoggerClose(void)
{
	FlashWaitDMA();
	while(FlashReadBusy());
	FlashWritePage(loggerState.headAddr+4,4,&loggerState.length,1);
	//move tail pointer
	
}

void LoggerWrite(void)
{
	
}

void LoggerGetList(void)
{
	
}

void LoggerRead(void)
{
	
}

#else
void LoggerInit(void)
{
	return;
}
void LoggerCreate(void)
{
	return;
}
void LoggerClose(void)
{
	return;
}
void LoggerWrite(void)
{
	return;
}
void LoggerGetList(void)
{
	return;
}
void LoggerRead(void)
{
	return;
}

#endif