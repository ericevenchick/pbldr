#include <p18f26k80.h>

/************************
 Program Memory Functions
*************************/
void FlashErase(unsigned int addr)
{

    TBLPTR = addr;

    EECON1bits.EEPGD = 1;	// select program memory
    EECON1bits.CFGS = 0;	// enable program memory access
    EECON1bits.WREN = 1;	// enable write access
    EECON1bits.FREE = 1;	// enable the erase

    INTCONbits.GIE = 0;		// disable interrupts

    // erase sequence
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;

    INTCONbits.GIE = 1;		// enable interrupts
}

void FlashWrite(unsigned int addr, char *data)
{
    int i;

    FlashErase(addr);		// must erase flash before writing

    TBLPTR = addr;

    // load the table latch with data
    for (i = 0; i < 64; i++)
    {
	TABLAT = data[i];	// copy data from buffer
	_asm
	    TBLWTPOSTINC	// increment the table latch
	_endasm
    }

    TBLPTR = addr;

    EECON1bits.EEPGD = 1;	// select program memory
    EECON1bits.CFGS = 0;	// enable program memory access
    EECON1bits.WREN = 1;	// enable write access

    INTCONbits.GIE = 0;		// disable interrupts

    // write sequence
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;

    INTCONbits.GIE = 1;		// enable interrupts

}

unsigned int CalcProgramChecksum(unsigned int addr)
{
	long checksum = 0;
	int temp;
	while (addr < 0xfffc)
	{
		TBLPTR = addr;
		_asm TBLRD _endasm
		checksum += TABLAT;
		addr++;
	}
	// add the checksum value
	TBLPTR = 0xfffc;
	_asm TBLRD _endasm	
	temp = TABLAT;	
	checksum += (temp << 8);
	TBLPTR = 0xfffd;
	_asm TBLRD _endasm		
	checksum += TABLAT;
	return (unsigned int)checksum;
}