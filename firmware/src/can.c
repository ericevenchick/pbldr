#include <p18f26k80.h>
void CANInit(long baud)
{
	// calculate the prescaler
	int prescale = (1000000/baud) - 1;

	// configure inputs and outputs
	TRISBbits.TRISB2 = 0;
	TRISBbits.TRISB3 = 1;
	
	// go to config mode
	CANCONbits.REQOP = 0b100;
	while(CANSTATbits.OPMODE != 0b100);

	// configure baud rate
	BRGCON1bits.BRP = prescale;	// prescaler
	BRGCON1bits.SJW = 0;		// 2x sync jump width
	BRGCON2bits.PRSEG = 0;		// 4x propagation time
	BRGCON2bits.SEG1PH = 0;		// 2x phase segment

	CIOCONbits.ENDRHI = 1;		// Vdd when recessive, prevents crosstalk 

	PIR5bits.RXB0IF = 0;
	PIE5bits.RXB0IE = 1;		// enable receive interrupt

	RXB0CONbits.RXM0 = 0;
	RXB0CONbits.RXM1 = 0;		

	// go to normal mode
	CANCONbits.REQOP = 0;
	while(CANSTATbits.OPMODE != 0);
}

void CANTx(int id, char dlc, char b0, char b1, char b2, char b3, 
	char b4, char b5, char b6, char b7)
{
	TXB0SIDH = id >> 3;
	TXB0SIDL = id<<5;
	TXB0D0 = b0;
	TXB0D1 = b1;
	TXB0D2 = b2;
	TXB0D3 = b3;
	TXB0D4 = b4;
	TXB0D5 = b5;
	TXB0D6 = b6;
	TXB0D7 = b7;

	TXB0DLC = dlc;
	TXB0CONbits.TXREQ = 1;
}
