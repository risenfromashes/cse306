/*
 * Control Unit.cpp
 *
 * Created: 2/18/2023 4:14:09 PM
 * Author : Team GGWP
 */

#define F_CPU 1000000

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

unsigned short instruction;  // 16 bit
unsigned char opcode;        // 4 bit
unsigned char msb_2;         // 2 bit, needed for jump flags

void UART_init(void) {
  // Normal speed, disable multi-proc
  UCSRA = 0b00000000;
  // Enable Tx (not Rx), disable interrupts
  UCSRB = 0b00001000;
  // Asynchronous mode, no parity, 1 stop bit, 8 data bits
  UCSRC = 0b10000110;
  // Baud rate 9600bps, assuming 1MHz clock
  UBRRL = 12;
  UBRRH = 0;
}

void UART_send(unsigned char data) {
  // wait until UDRE flag is set to logic 1
  while ((UCSRA & (1 << UDRE)) == 0x00)
    ;
  UDR = data;  // Write character to UDR for transmission
}

ISR(INT1_vect) {
  cli();
  unsigned char reg1 = (instruction >> 4) & 15;
  unsigned char reg2 = (instruction >> 8) & 15;
  unsigned char concatenated = (reg1 << 4) | reg2;
  UART_send(concatenated);
  sei();
}

inline void set_jmp(char jmp) {
  // D7
  PORTD &= ~(1 << 7);
  PORTD |= (jmp << 7);
}

inline void set_cjmp(char cjmp) {
  // D6
  PORTD &= ~(1 << 6);
  PORTD |= (cjmp << 6);
}

inline void set_jumpreg(char rjmp) {
  // D5
  PORTD &= ~(1 << 5);
  PORTD |= (rjmp << 5);
}

inline void set_aluop(char aluop) {
  // 3 bits, D4 D3 D2
  PORTD &= ~(7 << 2);
  PORTD |= (aluop << 2);
}

// D1 for Tx

inline void set_memwrite(char memwrite) {
  // D0
  PORTD &= ~(1 << 0);
  PORTD |= (memwrite << 0);
}

inline void set_alumem(char alumem) {
  // B7
  PORTB &= ~(1 << 7);
  PORTB |= (alumem << 7);
}

inline void set_regim(char regim) {
  // B6
  PORTB &= ~(1 << 6);
  PORTB |= (regim << 6);
}

inline void set_regwrite(char regwrite) {
  // B5
  PORTB &= ~(1 << 5);
  PORTB |= (regwrite << 5);
}

inline void set_destreg(char regdest) {
  // B0
  PORTB &= ~(1 << 0);
  PORTB |= (regdest << 0);
}

inline void set_all(unsigned short all) {
  // in the sheet order
  // aluop (3 bits), jmp, regwrite, memwrite,
  // alumem, regim, destreg, jumpreg, cjmp
  set_cjmp(all & 1);
  set_jumpreg((all >> 1) & 1);
  set_destreg((all >> 2) & 1);
  set_regim((all >> 3) & 1);
  set_alumem((all >> 4) & 1);
  set_memwrite((all >> 5) & 1);
  set_regwrite((all >> 6) & 1);
  set_jmp((all >> 7) & 1);
  set_aluop((all >> 8) & 7);
}

int main(void) {
  UART_init();
  DDRA = 0x00;
  DDRC = 0x00;  // for 16 bit instruction input, MSB in C0, LSB in A0
  DDRB = 0xFF;
  DDRD = 0xFF;
  MCUCSR = (1 << JTD);
  MCUCSR = (1 << JTD);  // for enabling port C for I/O
  GICR = (1 << INT1);
  MCUCR = MCUCR | (1 << ISC11);
  MCUCR = MCUCR | (1 << ISC10);
  sei();

  while (1) {
    instruction = PINA;
    // now need to reverse PINC and then put it in the upper bits of instruction
    unsigned char temp1, temp2 = 0;
    temp1 = PINC;
    for (int i = 0; i < 8; i++) {
      if (temp1 & (1 << i)) {
        temp2 |= (1 << (7 - i));
      }
    }
    instruction |= (temp2 << 8);

    opcode = PINA & 15;
    msb_2 = (instruction & (3LL << 14)) >> 14;

    unsigned short all = 0b00000001000;  // initialize with no-op one

    if (opcode == 0) {
      // NOR
      all = 0b11001000000;
    } else if (opcode == 1) {
      // SRL
      all = 0b10101001100;
    } else if (opcode == 2) {
      // AND
      all = 0b01101000000;
    } else if (opcode == 3) {
      // SLL
      all = 0b10001001100;
    } else if (opcode == 4) {
      // BNEQ
      all = 0b00000001001;
    } else if (opcode == 5) {
      if (msb_2 == 0) {
        // J
        all = 0b00010001000;
      } else if (msb_2 == 1) {
        // JR
        all = 0b00010001010;
      } else if (msb_2 == 2) {
        // NO-OP
        all = 0b00000001000;
      }
    } else if (opcode == 6) {
      // ANDI
      all = 0b01101001100;
    } else if (opcode == 7) {
      // SUBI
      all = 0b00101001100;
    } else if (opcode == 8) {
      // LW
      all = 0b00001011100;
    } else if (opcode == 9) {
      // SW
      all = 0b00000101000;
    } else if (opcode == 10) {
      // SUB
      all = 0b00101000000;
    } else if (opcode == 11) {
      // ADDI
      all = 0b00001001100;
    } else if (opcode == 12) {
      // OR
      all = 0b01001000000;
    } else if (opcode == 13) {
      // BEQ
      all = 0b00000001001;
    } else if (opcode == 14) {
      // ORI
      all = 0b01001001100;
    } else if (opcode == 15) {
      // ADD
      all = 0b00001000000;
    }

    set_all(all);
  }
}
