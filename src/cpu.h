#ifndef GB_CPU_H
#define GB_CPU_H

#include <stdint.h>
#include "registers.h"

typedef struct uint3_t {
  unsigned int value:3;
} uint3_t;

typedef struct Memory_Bus {
  uint8_t memory[0xFFFF];
} Memory_Bus;

uint8_t Memory_Bus_read_u8(Memory_Bus*, uint16_t);
int8_t Memory_Bus_read_r8(Memory_Bus *, uint16_t);
uint16_t Memory_Bus_read_u16(Memory_Bus*, uint16_t);
void Memory_Bus_write_u8(Memory_Bus*, uint16_t, uint8_t);
void Memory_Bus_write_u16(Memory_Bus*, uint16_t, uint16_t);

typedef enum CPU_Instruction { 
  ADD, ADDHL, ADC, SUB, SBC, SUBHL,
  AND, OR, XOR, CP, INC, DEC, CCF, SCF,
  RRA, RLA, RRCA, RLCA, CPL, BIT, RES, SET,
  RR, RL, RRC, RLC, SRL, SRA, SLA, SWAP,
  JP, JR, LD,
  NOP, STOP
} CPU_Instruction;

/**
* CPU Operation params
*/
typedef struct CPU_OP_Params {
  Register_Name reg; // TODO: split to 8/16 bit?
  uint8_t n8; // Immediate 8 bit data
  uint16_t n16; // Immediate 16 bit data
  int8_t r8; // Immediate signed 8 bit data
  uint3_t u3; // 
  // Used by LD 
  uint8_t load_into_n16;
  uint8_t load_into_sp;
} CPU_OP_Params;


typedef struct CPU {
  Registers registers;
  uint16_t pc; // program counter
  uint16_t sp; // stack pointer
  Memory_Bus bus;
} CPU;

// Debug Function
void CPU_dump(CPU *);

// utility function
uint8_t CPU_get_reg_value_by_name(CPU*, Register_Name);
uint16_t CPU_get_combo_reg_value_by_name(CPU*, Register_Name);

// utility function
void CPU_set_reg_value_by_name(CPU*, Register_Name, uint8_t);
void CPU_set_combo_reg_value_by_name(CPU*, Register_Name, uint16_t);

// utility function
void CPU_set_flags(CPU*, uint8_t, uint8_t, uint8_t, uint8_t);

void CPU_add(CPU*, Register_Name);

void CPU_adc(CPU*, Register_Name);

void CPU_addhl(CPU*, Register_Name);

void CPU_sub(CPU*, Register_Name);

void CPU_sbc(CPU*, Register_Name);

// Unnecessary?
void CPU_subhl(CPU*, Register_Name);

void CPU_cp(CPU*, Register_Name);

void CPU_and(CPU*, Register_Name);

void CPU_or(CPU*, Register_Name);

void CPU_xor(CPU*, Register_Name);

void CPU_inc(CPU*, Register_Name);

void CPU_dec(CPU*, Register_Name);

void CPU_ccf(CPU*);

void CPU_scf(CPU*);

void CPU_rra(CPU*);

void CPU_rla(CPU*);

void CPU_rrca(CPU*);

void CPU_rlca(CPU*);

void CPU_cpl(CPU*, Register_Name);

void CPU_bit(CPU*, Register_Name, uint3_t);

void CPU_res(CPU*, Register_Name, uint3_t);

void CPU_set(CPU*, Register_Name, uint3_t);

void CPU_rr(CPU*, Register_Name);

void CPU_rl(CPU*, Register_Name);

void CPU_rrc(CPU*, Register_Name);

void CPU_rlc(CPU*, Register_Name);

void CPU_srl(CPU*, Register_Name);

void CPU_sra(CPU*, Register_Name);

void CPU_sla(CPU*, Register_Name);

void CPU_swap(CPU*, Register_Name);

void CPU_jp(CPU*, uint16_t, uint8_t);

void CPU_jr(CPU*, int8_t, uint8_t);

void CPU_ld(CPU*, Register_Name, uint8_t, uint16_t, uint8_t, uint8_t);

// utility function kinda
uint8_t CPU_execute(CPU*, CPU_Instruction, CPU_OP_Params);

CPU_Instruction CPU_Instruction_from_ix_byte(CPU *, uint8_t);

CPU_OP_Params CPU_OP_Params_by_ix_byte(CPU *, CPU_Instruction, uint8_t);

uint8_t CPU_step(CPU*);

void CPU_run(CPU *, uint8_t);

#endif // !GB_CPU_H

