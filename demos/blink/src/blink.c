#include <p18f26k80.h>

#pragma interrupt isr_high
void isr_high()
{
	return;
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



#pragma code main=0x820
void main()
{
    int i;
    TRISBbits.TRISB0 = 0;
    
	for (;;)
	{
	    PORTBbits.RB0 = 1;
		for (i = 0; i < 10000; i++);
		PORTBbits.RB0 = 0;
		for (i = 0; i < 10000; i++);
	}
}