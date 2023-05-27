#ifndef GB_REGISTERS_H
#define GB_REGISTERS_H

#include <stdint.h>

#define CLEAR_LOW_BITS 0x00FF 
#define CLEAR_HIGH_BITS 0xFF00 
#define CLEAR_ALL_BITS 0x0000 

typedef struct Registers {
  // Accumulator
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  // Flag rgister
  // zero subtraction half-carry carry 0000
  uint8_t f;
  uint8_t h;
  uint8_t l;
} Registers;

typedef enum Register_Name { A, B, C, D, E, F, H, L, AF, BC, DE, HL } Register_Name;

// Manipulation of two registers at once
uint16_t Registers_get_bc(Registers*);
void Registers_set_bc(Registers*, uint16_t);

uint16_t Registers_get_af(Registers*);
void Registers_set_af(Registers*, uint16_t);

uint16_t Registers_get_de(Registers*);
void Registers_set_de(Registers*, uint16_t);

uint16_t Registers_get_hl(Registers*);
void Registers_set_hl(Registers*, uint16_t);

// Manipulation of flags register
uint8_t Registers_get_zero_flag(Registers* reg);
uint8_t Registers_get_sub_flag(Registers* reg);
uint8_t Registers_get_h_carry_flag(Registers* reg);
uint8_t Registers_get_carry_flag(Registers* reg);

#endif /* !GB_REGISTERS_H */
