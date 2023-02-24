/*
 * Clock.c
 *
 * Created: 2/24/2023 5:50:12 PM
 * Author : Team GGWP
 */

#define F_CPU 1000000

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

ISR(INT2_vect) { PORTA ^= (1 << 1); }

int main(void) {
  DDRA = 0x01;  // A0 will be for manual clock (0) / auto clock choice (1)
  // A1 will output the clock signal
  MCUCSR = (1 << ISC2);

  sei();

  while (1) {
    if (PINA & 0x01) {
      // auto clock
      GICR = 0;
      PORTA ^= (1 << 1);
      _delay_ms(2000);
    } else {
      // manual clock
      GICR = (1 << INT2);
    }
  }
}
