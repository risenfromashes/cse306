/*
 * Instruction Fetch.cpp
 *
 * Created: 2/15/2023 8:14:23 PM
 * Author : Team GGWP
 */

#include <avr/io.h>
#define F_CPU 1000000

unsigned short arr[1 << 9];
unsigned short temp[] = {0x100F, 0x620B, 0x130B, 0x400F, 0x434F, 0x133B,
                         0x123D, 0x0025, 0x8005, 0xA419, 0xA218};

void init() {
  for (int i = 0; i < 11; i++) {
    arr[i] = temp[i];
  }
}

unsigned short read_address() {
  // read such that the bits are in
  //   B3, B4, B5, B6, B7, D0, D1, D2, D3, D4, D5, D6 - this order
  unsigned short address = 0;
  if (PIND & (1 << 6)) address |= (1 << 0);
  if (PIND & (1 << 5)) address |= (1 << 1);
  if (PIND & (1 << 4)) address |= (1 << 2);
  if (PIND & (1 << 3)) address |= (1 << 3);
  if (PIND & (1 << 2)) address |= (1 << 4);
  if (PIND & (1 << 1)) address |= (1 << 5);
  if (PIND & (1 << 0)) address |= (1 << 6);
  if (PINB & (1 << 7)) address |= (1 << 7);
  if (PINB & (1 << 6)) address |= (1 << 8);
  if (PINB & (1 << 5)) address |= (1 << 9);
  if (PINB & (1 << 4)) address |= (1 << 10);
  if (PINB & (1 << 3)) address |= (1 << 11);
  return address;
}

int main(void) {
  init();
  unsigned volatile short uiAddress = 0, instruction;

  // input address is of 12 bits
  // its LSB will be in D6, MSB in B3
  MCUCSR = 1 << JTD;
  MCUCSR = 1 << JTD;
  DDRB = 0x00;
  DDRD = 0x00;
  // output will be an instruction of 16 bits
  // its MSB will be in A0, LSB in C0
  DDRC = 0xFF;
  DDRA = 0xFF;

  while (1) {
    uiAddress = read_address();
    uiAddress /= 2;
    instruction = arr[uiAddress];
    unsigned char temp =
        (instruction & (255LL << 8)) >> 8;  // upper 8 bits of instruction
    unsigned char reversed_temp = 0;
    for (int i = 0; i < 8; i++) {
      if (temp & (1 << i)) {
        reversed_temp |= (1 << (7 - i));
      }
    }
    PORTC = instruction & 255;
    PORTA = reversed_temp;
  }
}