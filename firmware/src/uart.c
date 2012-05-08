#include <p18f26k80.h>

#define FCY 8000000
#define UART_READ_TIMEOUT 20000

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
char UART1RxByte(void)
{
	int timeout = 0;
	// wait for data to be available, or a timeout to occur
    while (!PIR1bits.RC1IF && timeout < UART_READ_TIMEOUT)
    	timeout++;

	// on timeout, return -1
	if (timeout == UART_READ_TIMEOUT)
		return -1;
	return RCREG1;							// return data byte

}
/*
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
*/
/*
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
*/
// return the hex value of an ascii character
// ie, F = 15
int a2h(char c)
{
	if (c >= 0x30 && c <= 0x39)
		return (int)(c-0x30);
	if (c >= 0x41 && c <= 0x46)
		return (int)(c-0x37);
	return -1;
}