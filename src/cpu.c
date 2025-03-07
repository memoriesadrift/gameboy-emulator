#include "cpu.h"
#include "registers.h"
#include <_types/_uint16_t.h>
#include <stdint.h>
#include <stdio.h> // Just for debug

// TODO [16.4.23]: Check for correctness of half carry setting in subtraction routines, incl. CPU_cp()
// TODO [23.4.23]: Check for correctness of flag setting in bit routines (BIT)
// TODO [23.4.23]: Refactor RRA, RLA, RRCA, RLCA to use RR, RL, RRC, RLC for DRY code
// TODO [30.4.23]: Make CPU_step load next byte when necessary and ONLY once per step
// TODO [30.4.23]: STOP just stops execution atm, should it do something else?
// TODO [7.5.23]: Check for correctness of overflows in ADD, ADC, SUB, SBC etc.
// TODO [6.7.23]: Implement LD SP instructions
// TODO [18.1.25]: Implement HALT, EI, DI, rework STOP

void CPU_dump(CPU *cpu) {
    printf("==== CPU DUMP ====\n");
    printf("A: %d\n", cpu->registers.a);
    printf("B: %d\n", cpu->registers.b);
    printf("C: %d\n", cpu->registers.c);
    printf("D: %d\n", cpu->registers.d);
    printf("E: %d\n", cpu->registers.e);
    printf("F: %d\n", cpu->registers.f);
    printf("H: %d\n", cpu->registers.h);
    printf("L: %d\n", cpu->registers.l);
    printf("======= PC =======\n");
    printf("PC: %d\n", cpu->pc);
    printf("====== Flag ======\n");
    printf("Z: %d\n", Registers_get_zero_flag(&cpu->registers));
    printf("S: %d\n", Registers_get_sub_flag(&cpu->registers));
    printf("HC: %d\n", Registers_get_h_carry_flag(&cpu->registers));
    printf("C: %d\n", Registers_get_carry_flag(&cpu->registers));

    printf("=== Reading ix ===\n");
    printf("%x\n", Memory_Bus_read_u8(&cpu->bus, cpu->pc));
    printf("==================\n");
    printf("\n");
}

uint8_t Memory_Bus_read_u8(Memory_Bus *bus, uint16_t address) {
  return bus->memory[address];
}

int8_t Memory_Bus_read_r8(Memory_Bus *bus, uint16_t address) {
  return bus->memory[address];
}

uint16_t Memory_Bus_read_u16(Memory_Bus *bus, uint16_t address) {
    uint16_t next_byte = Memory_Bus_read_u8(bus, address);
    uint16_t second_next_byte = Memory_Bus_read_u8(bus, address + 1);

    return (second_next_byte << 8) | next_byte;
}

void Memory_Bus_write_u8(Memory_Bus *bus, uint16_t address, uint8_t value) {
  bus->memory[address] = value;
}

void Memory_Bus_write_u16(Memory_Bus *bus, uint16_t address, uint16_t value);

// utility function
uint8_t CPU_get_reg_value_by_name(CPU *cpu, Register_Name r) {
  switch (r) {
    case A:
      return cpu->registers.a;
    case B:
      return cpu->registers.b;
    case C:
      return cpu->registers.c;
    case D:
      return cpu->registers.d;
    case E:
      return cpu->registers.e;
    case F:
      return cpu->registers.f;
    case H:
      return cpu->registers.h;
    case L:
      return cpu->registers.l;
      // FIXME: Remove these paths
    case AF:
      return Registers_get_af(&cpu->registers);
    case BC:
      return Registers_get_bc(&cpu->registers);
    case DE:
      return Registers_get_de(&cpu->registers);
    case HL:
      return Registers_get_hl(&cpu->registers);
  }
}

uint16_t CPU_get_combo_reg_value_by_name(CPU *cpu, Register_Name r) {
  switch (r) {
    case AF:
      return Registers_get_af(&cpu->registers);
    case BC:
      return Registers_get_bc(&cpu->registers);
    case DE:
      return Registers_get_de(&cpu->registers);
    case HL:
      return Registers_get_hl(&cpu->registers);
    default:
      return 0;
  }
}

// utility function
void CPU_set_reg_value_by_name(CPU *cpu, Register_Name r, uint8_t to) {
  switch (r) {
    case A:
      cpu->registers.a = to;
      break;
    case B:
      cpu->registers.b = to;
      break;
    case C:
      cpu->registers.c = to;
      break;
    case D:
      cpu->registers.d = to;
      break;
    case E:
      cpu->registers.e = to;
      break;
    case F:
      cpu->registers.f = to;
      break;
    case H:
      cpu->registers.h = to;
      break;
    case L:
      cpu->registers.l = to;
      break;
    default:
      break;
  }
}

void CPU_set_combo_reg_value_by_name(CPU *cpu, Register_Name r, uint16_t to) {
  switch (r) {
    case AF:
      Registers_set_af(&cpu->registers, to);
      break;
    case BC:
      Registers_set_bc(&cpu->registers, to);
      break;
    case DE:
      Registers_set_de(&cpu->registers, to);
      break;
    case HL:
      Registers_set_hl(&cpu->registers, to);
      break;
    default:
      break;
  }
}

// utility function
void CPU_set_flags(CPU *cpu, uint8_t zero, uint8_t subtract, uint8_t half_carry, uint8_t carry) {
  uint8_t curr = cpu->registers.f;
  cpu->registers.f = (zero << 7 | subtract << 6 | half_carry << 5 | carry << 4) | (curr & 0x0F);
}

void CPU_add(CPU *cpu, Register_Name target) {
  uint8_t to_add = CPU_get_reg_value_by_name(cpu, target);
  uint8_t reg_a = CPU_get_reg_value_by_name(cpu, A);

  uint8_t did_overflow = 0;
  uint8_t res = 0;

  // Overflow check
  if (to_add > 0 && reg_a > UINT8_MAX - to_add) {
    did_overflow = 1;
    res = reg_a - (UINT8_MAX - to_add) - 1;
  } else {
    res = reg_a + to_add;
  }

  cpu->registers.a = res;
  CPU_set_flags(cpu, res == 0, 0, (((reg_a & 0xF) + (to_add & 0xF)) & 0x10) == 0x10, did_overflow);
}

void CPU_adc(CPU *cpu, Register_Name target) {
  uint8_t to_add = CPU_get_reg_value_by_name(cpu, target);
  uint8_t carry = Registers_get_carry_flag(&cpu->registers);
  uint8_t reg_a = CPU_get_reg_value_by_name(cpu, A);

  uint8_t did_overflow = 0;
  uint8_t res = 0;

  // Overflow check
  if (to_add + carry > 0 && reg_a > UINT8_MAX - (to_add + carry)) {
    did_overflow = 1;
    res = (to_add + carry) - (UINT8_MAX - reg_a) - 1;
  } else {
    res = reg_a + to_add + carry;
  }

  cpu->registers.a = res;

  CPU_set_flags(cpu, res == 0, 0, (((reg_a & 0xF) + (to_add & 0xF) + (carry & 0xF)) & 0x10) == 0x10, did_overflow);
}

void CPU_addhl(CPU *cpu, Register_Name target) {
  uint16_t to_add = CPU_get_reg_value_by_name(cpu, target);
  uint16_t reg_a = CPU_get_reg_value_by_name(cpu, HL);

  uint8_t did_overflow = 0;
  uint16_t res = 0;

  // Overflow check
  if (to_add > 0 && reg_a > UINT16_MAX - to_add) {
    did_overflow = 1;
    res = to_add - (UINT16_MAX - reg_a);
  } else {
    res = to_add + reg_a;
  }

  Registers_set_hl(&cpu->registers, res);
  CPU_set_flags(cpu, res == 0, 0, (res & 0xF) + (to_add & 0xF) > 0xF, did_overflow);
}

void CPU_sub(CPU *cpu, Register_Name target) {
  uint8_t to_sub = CPU_get_reg_value_by_name(cpu, target);
  uint8_t reg_a = CPU_get_reg_value_by_name(cpu, A);

  uint8_t res = 0;

  // Overflow check
  if (to_sub > 0 && reg_a < to_sub) {
    res = UINT8_MAX - (to_sub - reg_a);
  } else {
    res = reg_a - to_sub;
  }

  cpu->registers.a = res;
  CPU_set_flags(cpu, res == 0, 1, (((reg_a & 0xF) - (to_sub & 0xF)) & 0x10) == 0x10, to_sub > reg_a);
}

void CPU_sbc(CPU *cpu, Register_Name target) {
  uint8_t to_sub = CPU_get_reg_value_by_name(cpu, target);
  uint8_t carry = (CPU_get_reg_value_by_name(cpu, F) & 0b00010000) >> 4;
  uint8_t reg_a = CPU_get_reg_value_by_name(cpu, A);

  uint8_t did_underflow = 0;
  uint8_t res = 0;

  // Overflow check
  if (to_sub + carry > 0 && reg_a < to_sub + carry) {
    did_underflow = 1;
    res = UINT8_MAX - ((to_sub + carry) - reg_a);
  } else {
    res = reg_a - to_sub - carry;
  }

  cpu->registers.a = res;
  CPU_set_flags(cpu, res == 0, 1, (((reg_a & 0xF) - (to_sub & 0xF) - (carry & 0xF)) & 0x10) == 0x10, did_underflow);
}

// Unnecessary?
void CPU_subhl(CPU *cpu, Register_Name target) {
  uint16_t to_sub = CPU_get_reg_value_by_name(cpu, target);
  uint16_t reg_hl = CPU_get_reg_value_by_name(cpu, HL);

  uint8_t did_underflow = 0;
  uint16_t res = 0;

  // Overflow check
  if (to_sub > 0 && reg_hl < to_sub) {
    did_underflow = 1;
    res = UINT8_MAX - (to_sub - reg_hl);
  } else {
    res = reg_hl - to_sub;
  }

  Registers_set_hl(&cpu->registers, res);
  CPU_set_flags(cpu, res == 0, 1, (res & 0xF) + (to_sub & 0xF) > 0xF, did_underflow);
}

void CPU_cp(CPU *cpu, Register_Name target) {
  uint8_t to_sub = CPU_get_reg_value_by_name(cpu, target);
  uint8_t reg_a = CPU_get_reg_value_by_name(cpu, A);

  uint8_t did_underflow = 0;
  uint8_t res = 0;

  // Overflow check
  if (to_sub > 0 && reg_a < to_sub) {
    did_underflow = 1;
    res = UINT8_MAX - (to_sub - reg_a);
  } else {
    res = reg_a - to_sub;
  }

  CPU_set_flags(cpu, res == 0, 1, (res & 0xF) + (to_sub & 0xF) > 0xF, did_underflow);
}

void CPU_and(CPU *cpu, Register_Name target) {
  uint8_t other = CPU_get_reg_value_by_name(cpu, target);
  uint8_t reg_a = CPU_get_reg_value_by_name(cpu, target);

  uint8_t res = reg_a & other;
  cpu->registers.a = res;

  CPU_set_flags(cpu, res == 0, 0, 1, 0);
}

void CPU_or(CPU *cpu, Register_Name target) {
  uint8_t other = CPU_get_reg_value_by_name(cpu, target);
  uint8_t reg_a = CPU_get_reg_value_by_name(cpu, target);

  uint8_t res = reg_a | other;
  cpu->registers.a = res;

  CPU_set_flags(cpu, res == 0, 0, 0, 0);
}

void CPU_xor(CPU *cpu, Register_Name target) {
  uint8_t other = CPU_get_reg_value_by_name(cpu, target);
  uint8_t reg_a = CPU_get_reg_value_by_name(cpu, target);

  uint8_t res = reg_a ^ other;
  cpu->registers.a = res;

  CPU_set_flags(cpu, res == 0, 0, 0, 0);
}

void CPU_inc(CPU *cpu, Register_Name target) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t carry = (CPU_get_reg_value_by_name(cpu, F) & 0b00010000) << 4;

  uint8_t did_overflow = 0;
  uint8_t res = 0;

  // Overflow check
  if (reg > UINT8_MAX - 1) {
    did_overflow = 1;
    res = 0;
  } else {
    res = reg++;
  }

  cpu->registers.a = res;
  CPU_set_flags(cpu, res == 0, 0, did_overflow, carry);
}

void CPU_dec(CPU *cpu, Register_Name target) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t carry = (CPU_get_reg_value_by_name(cpu, F) & 0b00010000) << 4;

  uint8_t did_underflow = 0;
  uint8_t res = 0;

  // Overflow check
  if (reg < 1) {
    did_underflow = 1;
    res = UINT8_MAX;
  } else {
    res = reg--;
  }

  cpu->registers.a = res;
  CPU_set_flags(cpu, res == 0, 1, did_underflow, carry);
}

void CPU_ccf(CPU *cpu) {
  cpu->registers.f &= ~(1 << 4);
}

void CPU_scf(CPU *cpu) {
  cpu->registers.f |= (1 << 4);
}

void CPU_rra(CPU *cpu) {
  uint8_t reg = cpu->registers.a;
  uint8_t prev_carry = Registers_get_carry_flag(&cpu->registers);
  CPU_set_flags(cpu, 0, 0, 0, reg & (0b00000001));
  reg = reg >> 1;
  if (prev_carry == 1) {
    reg = reg | (prev_carry << 7);
  } else {
    reg = reg & ~(prev_carry << 7);
  }

  cpu->registers.a = reg;
}

void CPU_rla(CPU *cpu) {
  uint8_t reg = cpu->registers.a;
  uint8_t prev_carry = Registers_get_carry_flag(&cpu->registers);
  CPU_set_flags(cpu, 0, 0, 0, (reg & 0b10000000) >> 7);
  reg = reg << 1;
  if (prev_carry == 1) {
    reg = reg | prev_carry;
  } else {
    reg = reg & ~prev_carry;
  }

  cpu->registers.a = reg;
}

void CPU_rrca(CPU *cpu) {
  uint8_t reg = cpu->registers.a;
  uint8_t shifted = reg & (0b00000001);
  CPU_set_flags(cpu, 0, 0, 0, shifted);
  reg = reg >> 1;
  if (shifted == 1) {
    reg = reg | (shifted << 7);
  } else {
    reg = reg & ~(shifted << 7);
  }

  cpu->registers.a = reg;
}

void CPU_rlca(CPU *cpu) {
  uint8_t reg = cpu->registers.a;
  uint8_t shifted = (reg & 0b10000000) >> 7;
  CPU_set_flags(cpu, 0, 0, 0, shifted);
  reg = reg << 1;
  if (shifted == 1) {
    reg = reg | shifted;
  } else {
    reg = reg & ~shifted;
  }

  cpu->registers.a = reg;
}

void CPU_cpl(CPU *cpu, Register_Name target) {
  uint8_t to = ~CPU_get_reg_value_by_name(cpu, target);
  CPU_set_reg_value_by_name(cpu, target, to);
  CPU_set_flags(cpu, 0, 1, 1, 0);
}

void CPU_bit(CPU *cpu, Register_Name target, uint3_t pos) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t bit = (reg & (1 << pos.value)) >> pos.value;

  CPU_set_flags(cpu, bit == 0, 0, 1, 0);
}

void CPU_res(CPU *cpu, Register_Name target, uint3_t pos) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t new_reg = (reg & ~(1 << pos.value));
  CPU_set_reg_value_by_name(cpu, target, new_reg);
}

void CPU_set(CPU *cpu, Register_Name target, uint3_t pos) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t new_reg = (reg | (1 << pos.value));
  CPU_set_reg_value_by_name(cpu, target, new_reg);
}

void CPU_rr(CPU *cpu, Register_Name target) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t prev_carry = Registers_get_carry_flag(&cpu->registers);
  CPU_set_flags(cpu, 0, 0, 0, reg & (0b00000001));
  reg = reg >> 1;
  if (prev_carry == 1) {
    reg = reg | (prev_carry << 7);
  } else {
    reg = reg & ~(prev_carry << 7);
  }

  CPU_set_reg_value_by_name(cpu, target, reg);
}

void CPU_rl(CPU *cpu, Register_Name target) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t prev_carry = Registers_get_carry_flag(&cpu->registers);
  CPU_set_flags(cpu, 0, 0, 0, (reg & 0b10000000) >> 7);
  reg = reg << 1;
  if (prev_carry == 1) {
    reg = reg | prev_carry;
  } else {
    reg = reg & ~prev_carry;
  }

  CPU_set_reg_value_by_name(cpu, target, reg);
}

void CPU_rrc(CPU *cpu, Register_Name target) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t shifted = reg & (0b00000001);
  CPU_set_flags(cpu, 0, 0, 0, shifted);
  reg = reg >> 1;
  if (shifted == 1) {
    reg = reg | (shifted << 7);
  } else {
    reg = reg & ~(shifted << 7);
  }

  CPU_set_reg_value_by_name(cpu, target, reg);
}

void CPU_rlc(CPU *cpu, Register_Name target) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t shifted = (reg & 0b10000000) >> 7;
  CPU_set_flags(cpu, 0, 0, 0, shifted);
  reg = reg << 1;
  if (shifted == 1) {
    reg = reg | shifted;
  } else {
    reg = reg & ~shifted;
  }

  CPU_set_reg_value_by_name(cpu, target, reg);
}

void CPU_srl(CPU *cpu, Register_Name target) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t carry = reg & 1;
  reg = reg >> 1;

  CPU_set_reg_value_by_name(cpu, target, reg);
  CPU_set_flags(cpu, reg == 0, 0, 0, carry);
}

void CPU_sra(CPU *cpu, Register_Name target) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t carry = reg & 1;
  uint8_t msb = (reg & (0b10000000)) >> 7;
  reg = reg >> 1;

  if (msb == 1) {
    reg = reg | (1 << 7);
  } else {
    reg = reg & ~(1 << 7);
  }

  CPU_set_reg_value_by_name(cpu, target, reg);
  CPU_set_flags(cpu, reg == 0, 0, 0, carry);
}

void CPU_sla(CPU *cpu, Register_Name target) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t carry = (reg & (0b10000000)) >> 7;
  reg = reg << 1;

  CPU_set_reg_value_by_name(cpu, target, reg);
  CPU_set_flags(cpu, reg == 0, 0, 0, carry);
}

void CPU_swap(CPU *cpu, Register_Name target) {
  uint8_t reg = CPU_get_reg_value_by_name(cpu, target);
  uint8_t lower_nibble = reg & 0xF0;
  uint8_t upper_nibble = reg & 0x0F;
  lower_nibble = lower_nibble >> 4;
  upper_nibble = upper_nibble << 4;

  reg = 0 | lower_nibble | upper_nibble;
  CPU_set_reg_value_by_name(cpu, target, reg);
  CPU_set_flags(cpu, reg == 0, 0, 0, 0);
}

void CPU_jp(CPU *cpu, uint16_t to, uint8_t jump) {
  if (jump) {
    cpu->pc = to;
  } else {
    // If no jump, skip the JP ix and the n16 ix data
    (cpu->pc) += 3;
  }
}

// FIXME: Do we need to check for illegal jumps or are illegal jumps UB?
void CPU_jr(CPU *cpu, int8_t by, uint8_t jump) {
  if (jump) {
    cpu->pc += by;
  } else {
    // If no jump, skip the JR ix and the r8 ix data
    (cpu->pc) += 2;
  }
}

void CPU_push(CPU *cpu, Register_Name reg) {
  uint16_t value = CPU_get_combo_reg_value_by_name(cpu, reg);
  uint8_t lo = (uint8_t)(value & CLEAR_HIGH_BITS);
  uint8_t hi = (uint8_t)((value & CLEAR_LOW_BITS) >> 8);

  cpu->sp--;
  cpu->bus.memory[cpu->sp] = hi;
  cpu->sp--;
  cpu->bus.memory[cpu->sp] = lo;
}

void CPU_pop(CPU *cpu, Register_Name reg) {
  uint8_t lo = Memory_Bus_read_u8(&cpu->bus, cpu->sp);
  uint8_t hi = Memory_Bus_read_u8(&cpu->bus, cpu->sp + 1);

  switch (reg) {
    case BC:
      cpu->registers.b = lo;
      cpu->registers.c = hi;
    case DE:
      cpu->registers.d = lo;
      cpu->registers.e = hi;
    case HL:
      cpu->registers.h = lo;
      cpu->registers.l = hi;
    case AF:
      cpu->registers.a = lo;
      cpu->registers.f = hi;
    default:
        break;
  }

  cpu->sp +=2;
}

void CPU_call(CPU *cpu, uint8_t n8, uint16_t n16) {
  // Assume CALL by default
  uint8_t should_call = 1;
  switch (n8) {
    // NZ
    case 0xC4:
      should_call = !Registers_get_zero_flag(&cpu->registers);
      break;
    // NC
    case 0xD4:
      should_call = !Registers_get_carry_flag(&cpu->registers);
      break;
    // Z
    case 0xCC:
      should_call = Registers_get_zero_flag(&cpu->registers);
      break;
    // C
    case 0xDC:
      should_call = Registers_get_carry_flag(&cpu->registers);
      break;
  }

  if (!should_call) {
    // Skip the n16 param
    cpu->pc += 3;
    return;
  }
  // Push the pc we would go to if we wouldn't be calling onto the stack
  uint16_t next_pc_if_no_jump = Memory_Bus_read_u16(&cpu->bus, cpu->pc+3);
  uint8_t lo = (uint8_t)(next_pc_if_no_jump & CLEAR_HIGH_BITS);
  uint8_t hi = (uint8_t)((next_pc_if_no_jump & CLEAR_LOW_BITS) >> 8);

  cpu->sp--;
  cpu->bus.memory[cpu->sp] = hi;
  cpu->sp--;
  cpu->bus.memory[cpu->sp] = lo;

  CPU_jp(cpu, n16, 1);
}

void CPU_ret(CPU *cpu, uint8_t n8) {
  // Assume RET by default
  uint8_t should_ret = 1;
  switch (n8) {
    // NZ
    case 0xC0:
      should_ret = !Registers_get_zero_flag(&cpu->registers);
      break;
    // NC
    case 0xD0:
      should_ret = !Registers_get_carry_flag(&cpu->registers);
      break;
    // Z
    case 0xC8:
      should_ret = Registers_get_zero_flag(&cpu->registers);
      break;
    // C
    case 0xD8:
      should_ret = Registers_get_carry_flag(&cpu->registers);
      break;
  }

  if (!should_ret) {
    cpu->pc++;
    return;
  }

  // Pop the return value off the stack
  uint8_t lo = Memory_Bus_read_u8(&cpu->bus, cpu->sp);
  uint8_t hi = Memory_Bus_read_u8(&cpu->bus, cpu->sp + 1);
  cpu->sp += 2;

  uint16_t ret_addr = hi + (((uint16_t)lo) << 8);

  CPU_jp(cpu, ret_addr, 1);
}

void CPU_ld(
    CPU* cpu,
    Register_Name reg,
    uint8_t n8,
    uint16_t n16,
    uint8_t load_into_n16,
    uint8_t load_into_sp
  ) {
  /*
  printf("======= DEBUG =======\n");
  printf("LD Params\n");
  printf("reg: %d\n", reg);
  printf("n8: %x\n", n8);
  printf("n16: %d\n", n16);
  printf("load_into_n16: %d\n", load_into_n16);
  printf("load_into_sp: %d\n", load_into_sp);
  printf("=====================\n");
  */

  // Load into (n16)
  if (load_into_n16 > 0) {
    Memory_Bus_write_u8(&cpu->bus, n16, n8);
    return;
  }

  // Load into SP
  if (load_into_sp > 0) {
    // TODO: implement
  }

  // Load 16 bit value
  if (Registers_is_16_bit_register(reg) > 0) {
    CPU_set_combo_reg_value_by_name(cpu, reg, n16);
    return;
  }

  // LD r8, d8 / r8 / ...
  CPU_set_reg_value_by_name(cpu, reg, n8);
}

// utility function kinda
uint8_t CPU_execute(CPU *cpu, CPU_Instruction ix, CPU_OP_Params params) {
  switch (ix) {
    case ADD:
      CPU_add(cpu, params.reg);
      break;
    case ADC:
      CPU_adc(cpu, params.reg);
      break;
    case ADDHL:
      CPU_addhl(cpu, params.reg);
      break;
    case SUB:
      CPU_sub(cpu, params.reg);
      break;
    case SBC:
      CPU_sub(cpu, params.reg);
      break;
    case SUBHL:
      CPU_sub(cpu, params.reg);
      break;
    case AND:
      CPU_and(cpu, params.reg);
      break;
    case OR:
      CPU_or(cpu, params.reg);
      break;
    case XOR:
      CPU_xor(cpu, params.reg);
      break;
    case CP:
      CPU_cp(cpu, params.reg);
      break;
    case INC:
      CPU_inc(cpu, params.reg);
      break;
    case DEC:
      CPU_dec(cpu, params.reg);
      break;
    case CCF:
      CPU_ccf(cpu);
      break;
    case SCF:
      CPU_scf(cpu);
      break;
    case RRA:
      CPU_rra(cpu);
      break;
    case RLA:
      CPU_rla(cpu);
      break;
    case RRCA:
      CPU_rrca(cpu);
      break;
    case RLCA:
      CPU_rlca(cpu);
      break;
    case CPL:
      CPU_cpl(cpu, params.reg);
      break;
    case LD:
      CPU_ld(cpu, params.reg, params.n8, params.n16, params.load_into_n16, params.load_into_sp);
      break;
    // 2 byte ixs
    case BIT:
      CPU_bit(cpu, params.reg, params.u3);
      break;
    case RES:
      CPU_res(cpu, params.reg, params.u3);
      break;
    case SET:
      CPU_set(cpu, params.reg, params.u3);
      break;
    case RR:
      CPU_rr(cpu, params.reg);
      break;
    case RL:
      CPU_rl(cpu, params.reg);
      break;
    case RRC:
      CPU_rrc(cpu, params.reg);
      break;
    case RLC:
      CPU_rlc(cpu, params.reg);
      break;
    case SRA:
      CPU_sra(cpu, params.reg);
      break;
    case SLA:
      CPU_sla(cpu, params.reg);
      break;
    case SRL:
      CPU_srl(cpu, params.reg);
      break;
    case SWAP:
      CPU_swap(cpu, params.reg);
      break;
    case JP:
      CPU_jp(cpu, params.n16, params.n8);
      break;
    case JR:
      CPU_jr(cpu, params.r8, params.n8);
      break;
    case PUSH:
      CPU_push(cpu, params.reg);
      break;
    case POP:
      CPU_pop(cpu, params.reg);
      break;
    case CALL:
      CPU_call(cpu, params.n8, params.n16);
      break;
    case RET:
      CPU_ret(cpu, params.n8);
      break;
    case STOP:
      return 1;
    case NOP:
    default:
      break;
  }
  return 0;
}

CPU_Instruction CPU_Instruction_from_ix_byte(CPU *cpu, uint8_t byte) {
  uint8_t next;
  if (byte == 0xCB) {
    next = Memory_Bus_read_u8(&cpu->bus, cpu->pc + 1);
  }

  switch (byte) {
    case 0x00:
      return NOP;
    case 0x03:
    case 0x04:
    case 0x0C:
    case 0x13:
    case 0x14:
    case 0x1C:
    case 0x23:
    case 0x24:
    case 0x2C:
    case 0x33:
    case 0x34:
    case 0x3C:
      return INC;
    case 0x05:
    case 0x0B:
    case 0x0D:
    case 0x15:
    case 0x1B:
    case 0x1D:
    case 0x25:
    case 0x2B:
    case 0x2D:
    case 0x35:
    case 0x3B:
    case 0x3D:
      return DEC;
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
    case 0x09:
    case 0x19:
    case 0x29:
    case 0x39:
    case 0xC6:
    case 0xE8:
      return ADD;
    case 0x88:
    case 0x89:
    case 0x8A:
    case 0x8B:
    case 0x8C:
    case 0x8D:
    case 0x8E:
    case 0x8F:
    case 0xCE:
      return ADC;
    case 0x90:
    case 0x91:
    case 0x92:
    case 0x93:
    case 0x94:
    case 0x95:
    case 0x96:
    case 0x97:
    case 0xD6:
      return SUB;
    case 0x98:
    case 0x99:
    case 0x9A:
    case 0x9B:
    case 0x9C:
    case 0x9D:
    case 0x9E:
    case 0x9F:
    case 0xDE:
      return SBC;
    case 0xA0:
    case 0xA1:
    case 0xA2:
    case 0xA3:
    case 0xA4:
    case 0xA5:
    case 0xA6:
    case 0xA7:
    case 0xE6:
      return AND;
    case 0xA8:
    case 0xA9:
    case 0xAA:
    case 0xAB:
    case 0xAC:
    case 0xAD:
    case 0xAE:
    case 0xAF:
    case 0xEE:
      return XOR;
    case 0xB0:
    case 0xB1:
    case 0xB2:
    case 0xB3:
    case 0xB4:
    case 0xB5:
    case 0xB6:
    case 0xB7:
    case 0xF6:
      return OR;
    case 0xB8:
    case 0xB9:
    case 0xBA:
    case 0xBB:
    case 0xBC:
    case 0xBD:
    case 0xBE:
    case 0xBF:
    case 0xFE:
      return CP;
    // Prefix instructions
    case 0xCB:
      if (next >= 0x00 && next <= 0x07) {
        return RLC;
      }
      else if (next >= 0x08 && next <= 0x0F) {
        return RRC;
      }
      if (next >= 0x10 && next <= 0x17) {
        return RL;
      }
      else if (next >= 0x18 && next <= 0x1F) {
        return RR;
      }
      if (next >= 0x20 && next <= 0x27) {
        return SLA;
      }
      else if (next >= 0x28 && next <= 0x2F) {
        return SRA;
      }
      if (next >= 0x30 && next <= 0x37) {
        return SWAP;
      }
      else if (next >= 0x38 && next <= 0x3F) {
        return SRL;
      }
      else if (next >= 0x40 && next <= 0x7F) {
        return BIT;
      }
      else if (next >= 0x80 && next <= 0xBF) {
        return RES;
      }
      else if (next >= 0xC0 && next <= 0xFF) {
        return SET;
      }
    // Singleton instructions
    case 0x3F:
      return CCF;
    case 0x37:
      return SCF;
    case 0x07:
      return RLCA;
    case 0x17:
      return RLA;
    case 0x0F:
      return RRCA;
    case 0x1F:
      return RRA;
    case 0x10:
      return STOP;
    case 0x20:
    case 0x30:
    case 0x18:
    case 0x28:
    case 0x38:
      return JR;
    case 0xC2:
    case 0xC3:
    case 0xD2:
    case 0xCA:
    case 0xDA:
    case 0xE9:
      return JP;
    case 0x01:
    case 0x02:
    case 0x06:
    case 0x08:
    case 0x0A:
    case 0x0E:
    case 0x11:
    case 0x12:
    case 0x16:
    case 0x1A:
    case 0x1E:
    case 0x21:
    case 0x22:
    case 0x26:
    case 0x2A:
    case 0x2E:
    case 0x31:
    case 0x32:
    case 0x36:
    case 0x3A:
    case 0x3E:
    case 0xE0:
    case 0xE2:
    case 0xEA:
    case 0xF0:
    case 0xF2:
    case 0xF8:
    case 0xF9:
    case 0xFA:
      return LD;
    case 0xC5:
    case 0xD5:
    case 0xE5:
    case 0xF5:
      return PUSH;
    case 0xC1:
    case 0xD1:
    case 0xE1:
    case 0xF1:
      return POP;
    case 0xC4:
    case 0xD4:
    case 0xCD:
    case 0xCC:
    case 0xDD:
      return CALL;
    case 0xC0:
    case 0xD0:
    case 0xC8:
    case 0xD8:
    case 0xC9:
      return RET;
    default:
      // LD range 0x40 to 0x7F (skipping HALT)
      if (
        (byte >= 0x40 && byte <= 0x75) || (byte >= 0x77 && byte <= 0x7F)
      ) {
        return LD;
      }

      return NOP;
  }
}

CPU_OP_Params CPU_OP_Params_by_ix_byte(CPU *cpu, CPU_Instruction ix, uint8_t ix_byte) {
  uint8_t lower_nibble, upper_nibble;
  CPU_OP_Params params = {};

  // if it is a prefix ix, take next byte
  if (ix_byte == 0xCB) {
    uint8_t next_byte = Memory_Bus_read_u8(&cpu->bus, cpu->pc + 1);
    lower_nibble = next_byte & (0b00001111);
    upper_nibble = (next_byte & (0b11110000)) >> 4;
  } else {
    lower_nibble = ix_byte & (0b00001111);
    upper_nibble = (ix_byte & (0b11110000)) >> 4;
  }

  // Read offset if it's JR
  // FIXME: move to JR handling
  if (ix == JR) {
    params.r8 = Memory_Bus_read_u8(&cpu->bus, cpu->pc + 1);
  }

  // LD instruction handling
  if (ix == LD) {
    // LD r8, d8
    if (
        upper_nibble >= 0x0 && upper_nibble <= 0x3 &&
        (lower_nibble == 0x6 || lower_nibble == 0xE)
      ) {
      // d8
      params.n8 = Memory_Bus_read_u8(&cpu->bus, cpu->pc + 1);

      // r8
      switch (upper_nibble) {
        case 0x0:
          if (lower_nibble == 0x6) {
            params.reg = B;
          } else {
            params.reg = C;
          }
          break;
        case 0x1:
          if (lower_nibble == 0x6) {
            params.reg = D;
          } else {
            params.reg = E;
          }
          break;
        case 0x2:
          if (lower_nibble == 0x6) {
            params.reg = H;
          } else {
            params.reg = L;
          }
          break;
        case 0x3:
          if (lower_nibble == 0x6) {
	    params.n16 = CPU_get_combo_reg_value_by_name(cpu, HL);
	    params.load_into_n16 = 1;
          } else {
            params.reg = A;
          }
          break;
      }
    }

    // LD r8, r8
    // treat the register data as if it were n8
    if (
        ix_byte >= 0x40 && ix_byte <= 0x7F &&
        ix_byte != 0x76 // extra check to not run for HALT
        ) {
      // reg = desttination register
      switch (upper_nibble) {
        case 0x4:
          if (lower_nibble < 0x8) {
            params.reg = B;
          } else {
            params.reg = C;
          }
          break;
        case 0x5:
          if (lower_nibble < 0x8) {
            params.reg = D;
          } else {
            params.reg = E;
          }
          break;
        case 0x6:
          if (lower_nibble < 0x8) {
            params.reg = H;
          } else {
            params.reg = L;
          }
          break;
        case 0x7:
          if (lower_nibble < 0x8) {
	    params.n16 = CPU_get_combo_reg_value_by_name(cpu, HL);
	    params.load_into_n16 = 1;
          } else {
            params.reg = A;
          }
          break;
      }

      // LD r8, (HL)
      // LD (HL), r8
      switch (lower_nibble) {
        case 0x0:
        case 0x8:
          params.n8 = CPU_get_reg_value_by_name(cpu, B);
          break;
        case 0x1:
        case 0x9:
          params.n8 = CPU_get_reg_value_by_name(cpu, C);
          break;
        case 0x2:
        case 0xA:
          params.n8 = CPU_get_reg_value_by_name(cpu, D);
          break;
        case 0x3:
        case 0xB:
          params.n8 = CPU_get_reg_value_by_name(cpu, E);
          break;
        case 0x4:
        case 0xC:
          params.n8 = CPU_get_reg_value_by_name(cpu, H);
          break;
        case 0x5:
        case 0xD:
          params.n8 = CPU_get_reg_value_by_name(cpu, L);
          break;
        case 0x7:
        case 0xF:
          params.n8 = CPU_get_reg_value_by_name(cpu, A);
          break;
        // LD r8, (HL)
        case 0x6:
          // HALT ix
          if (lower_nibble == 0x7) {
            break;
          }
	  params.n8 = Memory_Bus_read_u8(
	      &cpu->bus, 
	      CPU_get_combo_reg_value_by_name(cpu, HL)
	    );
        case 0xE:
          params.n8 = Memory_Bus_read_u8(
	      &cpu->bus, 
	      CPU_get_combo_reg_value_by_name(cpu, HL)
	  );
          break;
      }
    }

    // LD r16, n16
    if (lower_nibble == 0x1 && upper_nibble < 0x4) {
      switch (upper_nibble) {
        case 0x0:
          params.reg = BC;
          break;
        case 0x1:
          params.reg = DE;
          break;
        case 0x2:
          params.reg = HL;
          break;
        case 0x3:
          // TODO: LD SP
          break;
      }

      params.n16 = Memory_Bus_read_u16(&cpu->bus, cpu->pc + 1);
    }

    // LD (r16), A
    if (lower_nibble == 0x2 && upper_nibble < 0x4) {
      switch (upper_nibble) {
        case 0x0:
          params.n16 = CPU_get_combo_reg_value_by_name(cpu, BC);
          break;
        case 0x1:
          params.n16 = CPU_get_combo_reg_value_by_name(cpu, DE);
          break;
        case 0x2:
          // HL+
          params.n16 = CPU_get_combo_reg_value_by_name(cpu, HL) + 1;
          break;
        case 0x3:
          // HL-
          params.n16 = CPU_get_combo_reg_value_by_name(cpu, HL) - 1;
          break;
      }

      params.n8 = CPU_get_reg_value_by_name(cpu, A);
      params.load_into_n16 = 1;
    }

    // LD A, (a16)
    if (ix_byte == 0xFA) {
      uint16_t address = Memory_Bus_read_u16(&cpu->bus, cpu->pc + 1);

      params.reg = A;
      params.n8 = Memory_Bus_read_u8(&cpu->bus, address);
    }

    // LD (a16), A
    if (ix_byte == 0xEA) {
      uint16_t address = Memory_Bus_read_u16(&cpu->bus, cpu->pc + 1);

      params.n16 = address;
      params.n8 = CPU_get_reg_value_by_name(cpu, A);
      params.load_into_n16 = 1;
    }

    // LD A, (C)
    if (ix_byte == 0xF2) {
      uint16_t address = 0xFF + CPU_get_reg_value_by_name(cpu, C);

      params.n8 = Memory_Bus_read_u8(&cpu->bus, address);
      params.reg = A;
    }

    // LD (C), A
    if (ix_byte == 0xE2) {
      uint16_t address = 0xFF + CPU_get_reg_value_by_name(cpu, C);

      params.n16 = address;
      params.n8 = CPU_get_reg_value_by_name(cpu, A);
      params.load_into_n16 = 1;
    }

    // LDH A, (a8)
    if (ix_byte == 0xF0) {
      uint16_t address = 0xFF + Memory_Bus_read_u8(&cpu->bus, cpu->pc + 1);

      // TODO: Do we want to just set the value into n8?
      params.n8 = Memory_Bus_read_u8(&cpu->bus, address);
      params.reg = A;
    }

    // LDH (a8), A
    if (ix_byte == 0xF0) {
      uint16_t address = 0xFF + Memory_Bus_read_u8(&cpu->bus, cpu->pc + 1);

      params.n16 = address;
      params.n8 = CPU_get_reg_value_by_name(cpu, A);
      params.load_into_n16 = 1;
    }

    // LD SP, n16
    if (ix_byte == 0x31) {
      params.n16 = Memory_Bus_read_u16(&cpu->bus, cpu->pc + 1);
      params.load_into_sp = 1;
    }

    // LD SP, HL
    if (ix_byte == 0xF9) {
      params.n16 = CPU_get_combo_reg_value_by_name(cpu, HL);
      params.load_into_sp = 1;
    }

    // LD HL, SP + r8
    if (ix_byte == 0xF8) {
      params.reg = HL;
      params.n16 = cpu->sp + Memory_Bus_read_r8(&cpu->bus, cpu->pc + 1);
      // TODO: Set HC and C bits based on overflows!
    }

    // TODO: LD (a16), SP
    if (ix_byte == 0x08) {
    }

    // TODO: cont
  }

  switch (ix) {
    // Arithmetic instructions
    case ADD:
    case ADC:
    case SUB:
    case SBC:
    case AND:
    case XOR:
    case OR:
    case CP:
    // For instructions expecting d8 input, read it if necessary
      if (upper_nibble >= 0xC) {
        params.n8 = Memory_Bus_read_u8(&cpu->bus, cpu->pc + 1);
        break;
      }
    // CB Prefix instructions
    case RLC:
    case RRC:
    case RL:
    case RR:
    case SLA:
    case SRA:
    case SWAP:
    case SRL:
    // Bit manipulation functions
    case BIT:
    case RES:
    case SET:
      switch (lower_nibble) {
        case 0x0:
        case 0x8:
          params.reg = B;
          break;
        case 0x1:
        case 0x9:
          params.reg = C;
          break;
        case 0x2:
        case 0xA:
          params.reg = D;
          break;
        case 0x3:
        case 0xB:
          params.reg = E;
          break;
        case 0x4:
        case 0xC:
          params.reg = H;
          break;
        case 0x5:
        case 0xD:
          params.reg = L;
          break;
        case 0x6:
        case 0xE:
          params.reg = HL;
          break;
        case 0x7:
        case 0xF:
          params.reg = A;
          break;
      }
      // Set u3 bit value
      uint3_t u3;
      switch (ix) {
        case BIT:
        case RES:
        case SET:
          switch (upper_nibble) {
            case 0x4:
            case 0x8:
            case 0xC:
              if (lower_nibble < 0x7) {
                u3.value = 0;
              } else {
                u3.value = 1;
              }
              break;
            case 0x5:
            case 0x9:
            case 0xD:
              if (lower_nibble < 0x7) {
                u3.value = 2;
              } else {
                u3.value = 3;
              }
              break;
            case 0x6:
            case 0xA:
            case 0xE:
              if (lower_nibble < 0x7) {
                u3.value = 4;
              } else {
                u3.value = 5;
              }
              break;
            case 0x7:
            case 0xB:
            case 0xF:
              if (lower_nibble < 0x7) {
                u3.value = 6;
              } else {
                u3.value = 7;
              }
              break;
          }
          params.u3 = u3;
          break;
        default:
          break;
      }
    case JP:
      // if it's a jump ix, read the 16 bit value
      // 0xE9 = jp HL, so skip reading data if it's that one
      if (ix_byte != 0xE9) {
        // 0xE9 == JP HL
        params.n16 = Memory_Bus_read_u16(&cpu->bus, cpu->pc+1);
      }
      switch (ix_byte) {
        case 0xC3:
          // JP r16
          params.n8 = 1;
          break;
        case 0xC2:
          // JP NZ
          params.n8 = !Registers_get_zero_flag(&cpu->registers);
          break;
        case 0xD2:
          // JP NC
          params.n8 = !Registers_get_carry_flag(&cpu->registers);
          break;
        case 0xCA:
          // JP Z
          params.n8 = Registers_get_zero_flag(&cpu->registers);
          break;
        case 0xDA:
          // JP C
          params.n8 = Registers_get_carry_flag(&cpu->registers);
          break;
        case 0xE9:
          // JP HL
          params.n8 = 1;
          params.n16 = Registers_get_hl(&cpu->registers);
          break;
        default:
          break;
      }
    // TODO: Rework for r8
    case JR:
      switch (ix_byte) {
        case 0x18:
          params.n8 = 1;
          break;
        case 0x20:
          // NZ
          params.n8 = !Registers_get_zero_flag(&cpu->registers);
          break;
        case 0x30:
          // NC
          params.n8 = !Registers_get_carry_flag(&cpu->registers);
          break;
        case 0x28:
          // Z
          params.n8 = Registers_get_zero_flag(&cpu->registers);
          break;
        case 0x38:
          // C
          params.n8 = Registers_get_carry_flag(&cpu->registers);
          break;
        default:
          break;
      }
    case PUSH:
    case POP:
      switch (lower_nibble) {
        case C:
          params.reg = BC;
          break;
        case D:
          params.reg = DE;
          break;
        case E:
          params.reg = HL;
          break;
        case F:
          params.reg = AF;
          break;
      }
      break;
      // For CALL and RET: forward the ix_byte so that the function can decide
      // whether it's a Z/C/NZ/NC ix.
    case CALL:
       params.n16 = Memory_Bus_read_u16(&cpu->bus, cpu->pc+1);
       params.n8 = ix_byte;
       break;
    case RET:
       params.n8 = ix_byte;
       break;
    default:
      break;
  }
  return params;
}

uint8_t CPU_step(CPU *cpu) {
  uint8_t ix_byte = Memory_Bus_read_u8(&cpu->bus, cpu->pc);
  CPU_Instruction ix = CPU_Instruction_from_ix_byte(cpu, ix_byte);
  CPU_OP_Params params = CPU_OP_Params_by_ix_byte(cpu, ix, ix_byte);

    uint8_t res = CPU_execute(cpu, ix, params);

    // TODO: check if gb really terminates execution on reaching 0xFFFF
    // or if the execution wraps around to 0x0
    if(res || cpu->pc == 0xFFFF) {
    return 1;
  }

  // Increment program counter
  // TODO: Handle pc incrementing for LD instructions
  if (ix == JP || ix == JR || ix == CALL || ix == RET) {
    // Ignore for jump ixs
    // and call / ret as they may jump
    // They handle their own pc manipulation
  } else if (
      ix_byte == 0xCB 
      || (((ix_byte & 0x0F) == 0x6 || (ix_byte & 0x0F) == 0xE) && ((ix_byte & 0xF0) < 0x4 || (ix_byte & 0xF0) > 0xB))
      ) {
    // Skip over second byte of 2 byte instructions
    // Or the data of instructions that take n8 as data
    (cpu->pc) += 2;
  } else if ((ix_byte & 0x0F) == 0x1 && ((ix_byte & 0xF0) < 0x4)) {
    // Skip over two bytes of instructions that take an n16 input
    (cpu->pc) += 3;
  } else {
    // For all other instructions, move to the next byte
    (cpu->pc)++;
  }

  return 0;
}

void CPU_run(CPU *cpu, uint8_t dump) {
  uint8_t ret_signal;
  while (cpu->pc <= 0xFFFF) {
    if (dump) {
      CPU_dump(cpu);
    }

    ret_signal = CPU_step(cpu);

    if(ret_signal) {
      break;
    }
  }
}

