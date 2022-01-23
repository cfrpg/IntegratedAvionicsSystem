#ifndef __FLASH_H
#define __FLASH_H
#include "sys.h"

#define FLASH_CS PAout(12)

#define FLASH_CAP 0x02000000
#define FLASH_LAST_SECTOR_ADDR 0x01FFF000
#define FLASH_LAST_PAGE_ADDR 0x01FFFF00
#define FLASH_SECTOR_CNT 8192
#define FLASH_SECTOR_MASK 0x1FFF
#define FLASH_SECTOR_SIZE 4096


void FlashInit(void);
void FlashStartup(void);
void FlashWritePage(u32 addr,u32 len,u8* data,u8 wait);
void FlashRead(u32 addr,u32 len,u8* data);
void FlashEraseSector(u32 addr,u8 wait);
u8 FlashReadBusy(void);
u8 FlashReadWEL(void);
u32 FlashGetAddr(u32 sector,u8 page,u8 byte);
void FlashStartDMAWrite(u32 addr,u32 len,u8* data,u8 wait);

void FlashWaitDMA(void);

#endif