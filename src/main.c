#include "test.h"

int main (int argc, char *argv[]) {
  return test_cpu();

  /*
  CPU cpu = {
    .pc = 0,
    .bus = {
      .memory = {0x80, 0x80, 0x91, 0x10},
    },
    .registers = {
      .a = 5,
      .b = 4,
      .c = 1,
    }
  };

  CPU cpu = {
    .registers = {
      .a = 0b11010110,
      .b = 20,
      .c = 3,
      .d = 0,
      .e = 0xAB,
      .f = 0b00010000
    },
  };

  printf("Testing BIT:\n");
  printf("%d\n", cpu.registers.d);
  CPU_OP_Params p = {.reg = D, .u3 = {.value = 0b0}};
  CPU_execute(&cpu, BIT, p);
  printf("test result: %d\n", Registers_get_zero_flag(&cpu.registers));

  printf("Testing RES:\n");
  printf("%d\n", cpu.registers.d);
  CPU_execute(&cpu, RES, p);
  printf("%d\n", cpu.registers.d);

  printf("Testing SET:\n");
  printf("%d\n", cpu.registers.d);
  CPU_execute(&cpu, SET, p);
  printf("%d\n", cpu.registers.d);

  CPU_execute(&cpu, BIT, p);
  printf("test result: %d\n", Registers_get_zero_flag(&cpu.registers));

  printf("Testing SWAP:\n");
  printf("%x\n", cpu.registers.e);
  CPU_OP_Params swap_params = {.reg = E};
  CPU_execute(&cpu, SWAP, swap_params);
  printf("%x\n", cpu.registers.e);

  */
}
