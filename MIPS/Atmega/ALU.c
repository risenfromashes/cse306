/*
 * ALU.c
 *
 * Created: 2/5/2023 11:16:28 PM
 * Author : Team GGWP
 */ 
#define F_CPU 1000000

#include "inttypes.h"
#include <avr/io.h>

unsigned char performOp(unsigned char a, unsigned char b, char aluOp) {
	unsigned char result;
	switch(aluOp) {
		case 0: result = a + b; break;
		case 1: result = a - b; break;
		case 2: result = a | b; break;
		case 3: result = a & b; break;
		case 4: result = a << b; break;
		case 5: result = a >> b; break;
		case 6: result = ~(a | b); break;
		default:
		result = -1;
	}
	return result;
}

int main(void)
{
	volatile char opCode =0;
	volatile unsigned char  inp1;
	volatile unsigned char inp2;
    /* For taking input from C pin */
	MCUCSR = (1<<JTD);
	MCUCSR = (1<<JTD);
	
	DDRA = 0b00000000;
	DDRC = 0b00000000;
	DDRB = 0b11111111;
	DDRD = 0b00000000;
    while (1) 
    {
		opCode = (PIND) & 7;
		inp1 = PINA;
		inp2 = PINC;
		PORTB = performOp(inp1,inp2, opCode);
    }	
}

