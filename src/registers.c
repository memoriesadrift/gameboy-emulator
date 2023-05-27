#include "registers.h"

uint16_t Registers_get_bc(Registers* reg) {
  return (uint16_t)reg->b << 8 | (uint16_t)reg->c;
}

void Registers_set_bc(Registers* reg, uint16_t value) {
  reg->b = (uint8_t)((value & CLEAR_HIGH_BITS) >> 8);
  reg->c = (uint8_t)(value & CLEAR_LOW_BITS);
}

uint16_t Registers_get_af(Registers* reg) {
  return (uint16_t)reg->a << 8 | (uint16_t)reg->f;
}

void Registers_set_af(Registers* reg, uint16_t value) {
  reg->a = (uint8_t)((value & CLEAR_HIGH_BITS) >> 8);
  reg->f = (uint8_t)(value & CLEAR_LOW_BITS);
}

uint16_t Registers_get_de(Registers* reg) {
  return (uint16_t)reg->d << 8 | (uint16_t)reg->e;
}

void Registers_set_de(Registers* reg, uint16_t value) {
  reg->d = (uint8_t)((value & CLEAR_HIGH_BITS) >> 8);
  reg->e = (uint8_t)(value & CLEAR_LOW_BITS);
}

uint16_t Registers_get_hl(Registers* reg) {
  return (uint16_t)reg->h << 8 | (uint16_t)reg->l;
}

void Registers_set_hl(Registers* reg, uint16_t value) {
  reg->h = (uint8_t)((value & CLEAR_HIGH_BITS) >> 8);
  reg->l = (uint8_t)(value & CLEAR_LOW_BITS);
}

uint8_t Registers_get_zero_flag(Registers* reg) {
  return (reg->f & 0b10000000) >> 7;
}

uint8_t Registers_get_sub_flag(Registers* reg) {
  return (reg->f & 0b01000000) >> 6;
}

uint8_t Registers_get_h_carry_flag(Registers* reg) {
  return (reg->f & 0b00100000) >> 5;
}

uint8_t Registers_get_carry_flag(Registers* reg) {
  return (reg->f & 0b00010000) >> 4;
}

