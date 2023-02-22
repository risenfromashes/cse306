/*
 * EEPROM.cpp
 *
 * Created: 2/15/2023 8:14:23 PM
 * Author : User
 */

#include <avr/io.h>

unsigned char EEPROM_read(unsigned int uiAddress) {
  /* Wait for completion of previous write */
  while (EECR & (1 << EEWE))
    ;
  /* Set up address register */
  EEAR = uiAddress;
  /* Start eeprom read by writing EERE */
  EECR |= (1 << EERE);
  /* Return data from data register */
  return EEDR;
}

unsigned short read_address() {
  // read such that the bits are in
  //   B3, B4, B5, B6, B7, D0, D1, D2, D3, D4, D5, D6 - this order
  unsigned short address = 0;
  //   address |= (PIND & (1 << 6));
  //   address |= (PIND & (1 << 5)) << 1;
  //   address |= (PIND & (1 << 4)) << 2;
  //   address |= (PIND & (1 << 3)) << 3;
  //   address |= (PIND & (1 << 2)) << 4;
  //   address |= (PIND & (1 << 1)) << 5;
  //   address |= (PIND & (1 << 0)) << 6;
  for (int i = 0; i <= 6; i++) {
    address |= ((PIND & (1 << (6 - i))) << i);
  }
  //   address |= (PINB & (1 << 7)) << 7;
  //   address |= (PINB & (1 << 6)) << 8;
  //   address |= (PINB & (1 << 5)) << 9;
  //   address |= (PINB & (1 << 4)) << 10;
  //   address |= (PINB & (1 << 3)) << 11;
  for (int i = 0; i <= 4; i++) {
    address |= (PINB & (1 << (7 - i))) << (7 + i);
  }
  return address;
}

int main(void) {
  unsigned short uiAddress;
  unsigned short instruction;

  // input address is of 12 bits
  // its LSB will be in D6, MSB in B3
  DDRB = 0x00;
  DDRD = 0x00;
  // output will be an instruction of 16 bits
  // its MSB will be in A0, LSB in C0
  DDRC = 0xFF;
  DDRA = 0xFF;

  while (1) {
    uiAddress = read_address();
    instruction = EEPROM_read(uiAddress);
    unsigned short temp =
        (instruction & (255 << 8)) >> 8;  // upper 8 bits of instruction
    unsigned char reversed_temp = 0;
    for (int i = 0; i < 8; i++) {
      if (temp & (1 << i)) {
        reversed_temp |= (1 << (7 - i));
      }
    }
    PORTC = instruction;
    PORTA = reversed_temp;
  }
}