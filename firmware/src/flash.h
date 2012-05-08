#ifndef _FLASH_H_
#define _FLASH_H_

void FlashErase(unsigned int addr);
void FlashWrite(unsigned int addr, char *data);
int CalcProgramChecksum(unsigned int addr);

#endif
