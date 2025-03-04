#ifndef GB_TEST_H
#define GB_TEST_H

#include "cpu.h"
#include <stdint.h>

typedef enum test_verdict {PASS, FAIL} test_verdict;

typedef struct test_params {
  CPU cpu;
  Registers expected_registers;
  uint8_t expected_a_val;
  uint8_t expected_flags;
} test_params;

typedef struct test_result {
  test_verdict verdict;
  Registers actual_registers;
  Registers expected_registers;
  uint8_t actual_a_val;
  uint8_t actual_flags;
  uint8_t expected_a_val;
  uint8_t expected_flags;
} test_result;

void dump_bin(uint8_t);

test_verdict test_cpu();

#endif // !GB_TEST_H

