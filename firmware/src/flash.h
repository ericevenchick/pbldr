#ifndef _FLASH_H_
#define _FLASH_H_

void FlashErase(long addr);
void FlashWrite(long addr, char *data);

#endif
