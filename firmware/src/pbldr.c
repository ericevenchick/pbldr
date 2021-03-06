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
#include "uart.h"

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

#define USE_UART
#define UART_BITRATE 115200

// End of configuration

// Constants, don't change these
// ...unless you know what you're doing

#define FCY 8000000
#define FLASH_ADDR 0x900
#define BOOT_ADDR 0x920
#define REMAP_HIGH_INTERRUPT 0x908
#define REMAP_LOW_INTERRUPT 0x918

// End of constants

int chk;
void run(void);

void UARTFlash(void)
{
    int i;
    char high, low, res;
    int cur_addr = FLASH_ADDR;
    char buf[64];
    char done = 0;

    UART1Init(UART_BITRATE);

    // wait for request to load code
    if (UART1RxByte() == -1)
        run();

    UART1TxByte('O');
    UART1TxByte('K');
    UART1TxByte('\n');

    for (;;)
    {
        for (i = 0; i < 64; i++)
        {
            // receive data as ascii and convert to hex
            high = a2h(UART1RxByte());
            low = a2h(UART1RxByte());
            // non-hex byte received, stop flashing
            if (high == -1 || low == -1)
            {
                done = 1;
                break;
            }
            res = (high<<4) + low;
            buf[i] = res;
        }
        FlashWrite(cur_addr, buf);
        cur_addr += 64;
        UART1TxByte('K');
        if (done)
            break;
    }
    // tell the PC that we've finished flashing
    UART1TxByte('D');
    UART1TxByte('O');
    UART1TxByte('N');
    UART1TxByte('E');
    UART1TxByte('\n');
    // new code, so recalculate the checksum
    chk = CalcProgramChecksum((unsigned int)FLASH_ADDR);
    run();
}

void main(void)
{
    chk = CalcProgramChecksum((unsigned int)FLASH_ADDR);
    UARTFlash();
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
    // otherwise, reset the device
    _asm reset _endasm
}
