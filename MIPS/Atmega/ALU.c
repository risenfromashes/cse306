/
 * ALU.c
 *
 * Created: 2/5/2023 11:16:28 PM
 * Author : Team GGWP
 */ 
#define F_CPU 1000000

#include <avr/io.h>


char performOp(char a, char b, char aluOp) {
	char result;
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
	return result & 15;
}

int main(void)
{
	volatile char opCode =0;
	volatile char inp1;
	volatile char inp2;
    /* Replace with your application code */
	DDRA = 0b00001111;
	DDRB = 0b00000000;
	DDRD = 0b00000000;
    while (1) 
    {
		opCode = (PIND) & 7;
		inp1 = (PINB) & 15;
		inp2 = ((PINB) & (15 << 4)) >> 4;
		PORTA = performOp(inp1,inp2, opCode);
    }
	
}

