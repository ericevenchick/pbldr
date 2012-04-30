#ifndef _UART_H_
#define _UART_H_

void UART1Init(long baud);
void UART1TxByte(char byte);
char UART1RxByte(void);
void UART1TxROMString(const rom char *str);
void UART1TxString(char *str);
int a2h(char c);

#endif