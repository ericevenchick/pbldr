#include <p18f26k80.h>

#pragma interrupt isr_high
void isr_high()
{
	char state = LATBbits.LATB0;

	INTCONbits.TMR0IF = 0;

	if (state == 0)
		PORTBbits.RB0 = 1;
	else
		PORTBbits.RB0 = 0;	
}
#pragma interruptlow isr_low
void isr_low()
{
	return;
}

#pragma code high_vector=0x808
void high_int()
{
	_asm goto isr_high _endasm
}
#pragma code low_vector=0x818
void low_int()
{
	_asm goto isr_low _endasm
}



#pragma code main
void main()
{
    int i;
    TRISBbits.TRISB0 = 0;
	PORTBbits.RB0 = 1;

	T0CONbits.T08BIT = 0;
	T0CONbits.T0CS = 0;
	T0CONbits.T0PS = 255;
	T0CONbits.TMR0ON =1;
    INTCONbits.GIE = 1;
	INTCONbits.TMR0IF = 0;
	INTCONbits.TMR0IE = 1;


	for (;;);
}