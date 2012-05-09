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
#include "flash.h"
#include "can.h"

// Configuration Bits
#pragma config XINST = OFF	// disable extended instructions
#pragma config FOSC = XT	// using XTal oscillator
#pragma config PLLCFG = OFF	// disable 4x pll
#pragma config FCMEN = OFF	// disable fail-safe clock monitor
#pragma config IESO = OFF	// disable internal oscillator switch over
#pragma config PWRTEN = OFF	// disable power up timer
#pragma config BOREN = OFF	// disable brown out protect
#pragma config BORV = 2		// set brownout threshold at 2V
#pragma config BORPWR = MEDIUM	// set BORMV to medium power level
#pragma config WDTEN = OFF	// disable watchdog
#pragma config CANMX = PORTB	// use port b pins for CAN
#pragma config MCLRE = ON	// enable MCLR (needed for debug)
#pragma config CPB = OFF	// disable boot code protect
#pragma config CPD = OFF	// disable ee read protect
#pragma config CP1 = OFF	// disable code protect
#pragma config CP2 = OFF
#pragma config CP3 = OFF
#pragma config WRT1 = OFF	// disable table write protect
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF



// Configuration

#define CAN_BITRATE 500000
#define CAN_ID 0x1

// End of configuration

// Constants

#define FCY 8000000
#define FLASH_ADDR 0x940
#define BOOT_ADDR 0x960
#define REMAP_HIGH_INTERRUPT 0x948
#define REMAP_LOW_INTERRUPT 0x958

// End of constants

int chk;
void run(void);

void CANFlash(void)
{
    int timeout;
    int i;
    char buf[64];
    int cur_addr = FLASH_ADDR;

    CANInit(CAN_BITRATE);

    // configure CAN filter
    RXF0SIDH = CAN_ID >> 3;
    RXF0SIDL = CAN_ID << 5;

    timeout = 30000;
    while(!RXB0CONbits.RXFUL && timeout > 0)
        timeout--;
    if (timeout == 0)
        run();		// timeout occured, start program

    i = 0;
    for (;;)
    {

        RXB0CONbits.RXFUL = 0;
        while(!RXB0CONbits.RXFUL); // wait for a message
        if (RXB0D1 = 0x10)
            run();
        buf[i] = RXB0D2;
        i++;
        buf[i] = RXB0D3;
        i++;
        buf[i] = RXB0D4;
        i++;
        buf[i] = RXB0D5;
        i++;

        if (i == 64)
        {
            FlashWrite(cur_addr, buf);
            cur_addr += 64;
            CANTx(0x10, 1, 'k',0,0,0,0,0,0,0);
            i = 0;
        }
    }
}

void main(void)
{
    chk = CalcProgramChecksum((unsigned int)FLASH_ADDR);
    CANFlash();
}

// interrupt remapping
#pragma code high_vector=0x08
void high_isr()
{
    _asm goto REMAP_HIGH_INTERRUPT _endasm
}
#pragma code low_vector=0x18
void low_isr()
{
    _asm goto REMAP_LOW_INTERRUPT _endasm
}

// start the user program
void run(void)
{
    // only run if checksum is valid
    if (chk == 0)
    {
        _asm goto BOOT_ADDR _endasm
    }
    _asm reset _endasm
}
