/*
 * RAM.cpp
 *
 * Created: 2/15/2023 10:04:14 PM
 * Author : Team GGWP
 */

#define F_CPU 1000000
#include <avr/io.h>

unsigned char arr[256];
char memWrite;
unsigned char uiAddress, writeData;

int main(void) {
  DDRB = 0x00;
  DDRD = 0x00;
  DDRA = 0xFF;
  MCUCSR = (1 << JTD);
  MCUCSR = (1 << JTD);
  DDRC = 0x00;  // PC0 will be used for memWrite flag

  while (1) {
    memWrite = PINC;
    uiAddress = PINB;
    writeData = PIND;
    if (memWrite & 1) {
      // write
      arr[uiAddress] = writeData;
    } else {
      // read
      PORTA = arr[uiAddress];
    }
  }
}