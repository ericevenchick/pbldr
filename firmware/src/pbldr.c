/*
 * Copyright 2012 Eric Evenchick
 *
 * This file is part of pbldr.
 *
 * pbldr is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * pbldr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with pbldr If not, see http://www.gnu.org/licenses/.
 */

#include <p18f26k80.h>

// Configuration Bits
#pragma config XINST = OFF	// disable extended instructions
#pragma config FOSC = INTIO2	// using internal RC oscillator (change me)
#pragma config PLLCFG = OFF	// disable 4x pll
#pragma config FCMEN = OFF	// disable fail-safe clock monitor
#pragma config IESO = OFF	// disable internal oscillator switch over
#pragma config PWRTEN = OFF	// disable power up timer
#pragma config BOREN = OFF	// disable brown out protect
#pragma config BORV = 2		// set brownout threshold at 2V
#pragma config BORPWR = MEDIUM	// set BORMV to medium power level
#pragma config WDTEN = OFF	// disable watchdog
#pragma config CANMX = PORTC	// use port c pins for CAN
#pragma config MCLRE = ON	// enable MCLR (needed for debug)
#pragma config CPB = OFF	// disable boot code protect
#pragma config CPD = OFF	// disable ee read protect
#pragma config CP1 = OFF	// disable code protect
#pragma config CP2 = OFF
#pragma config CP3 = OFF
#pragma config WRT1 = OFF	// disable table write protect
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF

#define FCY 8000000
#define BOOT_ADDR 0x800

/********************
 UART 1 Functions
********************/

// initializes UART1 at specified baud rate
void UART1Init(long baud){
    RCSTA1bits.SPEN = 1;	// enable port
    TRISCbits.TRISC7 = 1;	// make rx pin an input
    RCSTA1bits.CREN = 1;	// enable receive
    TRISCbits.TRISC6 = 0;	// make tx pin an output
    TXSTA1bits.TXEN = 1;	// enable transmit
    TXSTA1bits.SYNC = 0;	// use async serial
    TXSTA1bits.BRGH = 1;	// high speed mode
    BAUDCON1bits.BRG16 = 1;	// use 16 bit baud rate generator
    SPBRG1 = (FCY/baud/4)-1;	// set baud rate generator
    return;
}

// writes a byte to UART1
void UART1TxByte(char byte)
{
    while (!TXSTA1bits.TRMT); // wait until buffer is empty
    TXREG1 = byte;            // write the byte
    return;
}

// reads a byte from UART 1  
char UART1RxByte(unsigned int timeout)
{
    while (!PIR1bits.RC1IF && timeout > 0)	// wait for data to be available
    	timeout--;
	return RCREG1;							// return data byte

}
// writes a string from ROM to UART1
void UART1TxROMString(const rom char *str)
{
    int i = 0;

    while(str[i] != 0){
        UART1TxByte(str[i]);
        i++;
    }
    return;
}

// writes a string from RAM to UART1
void UART1TxString(char *str)
{
    int i = 0;

    while(str[i] != 0){
        UART1TxByte(str[i]);
        i++;
    }
    return;
}

/************************
 Program Memory Functions
*************************/
void FlashErase(long addr)
{
//    TBLPTRL = (int)addr;	// load the lower table pointer
//    TBLPTRH = (int)(addr<<8);	// load the upper table pointer

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

void FlashWrite(long addr, char *data)
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
// test code
void main()
{
    int i;
    long cur_addr = BOOT_ADDR;
    char buf[64];
    char done = 0;

    UART1Init(115200);


    TRISBbits.TRISB0 = 0;
    PORTBbits.RB0 = 1;

	if (UART1RxByte(20000) == 0)
		_asm goto 0x800 _endasm

    UART1TxROMString("OK\n");

	for (;;)
    {
        for (i = 0; i < 64; i++)
		{
            buf[i] = UART1RxByte(5000);
           	if (buf[i-3] == 'D' && buf[i-2] == 'O' &&
				buf[i-1] == 'N' && buf[i] == 'E')
			{
               	done = 1;
           		break;
			}
		}
		FlashWrite(cur_addr, buf);
           cur_addr += 64;
           UART1TxByte('K');
   		if (done)
			break;	    
	}
       UART1TxROMString("DONE\n");
	_asm goto 0x800 _endasm
}
