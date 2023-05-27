#include "test.h"
#include "registers.h"
#include <stdint.h>
#include <stdio.h>

void dump_bin(uint8_t n) {
  for (int i = 7; i >= 0; i -= 1) {
    uint8_t bit = (n >> i) & 1;
    printf("%d", bit);
  }
}

test_verdict expect_equals(uint8_t it, uint8_t expected) {
  return it == expected ? PASS : FAIL;
}

void dump(test_result result) {
  printf("Expected A to be: ");
  dump_bin(result.expected_a_val);
  printf("\n");
  printf("             was: ");
  dump_bin(result.actual_a_val);
  printf("\n");
  printf("Expected F to be: ");
  dump_bin(result.expected_flags);
  printf("\n");
  printf("             was: ");
  dump_bin(result.actual_flags);
}

test_verdict test_suite(test_result (*test_fn)(test_params), int argc, test_params *argv) {
  int tests_failed = 0;
  printf(" --- RUNNING TEST SUITE --- \n");
  for (int i = 0; i < argc; i++) {
    test_result result;
    result = test_fn(argv[i]);
    if (result.verdict == PASS) {
      printf(".");
    } else {
      tests_failed++;
      printf("\n --- TEST %d FAILED --- \n", i);
      dump(result);
      printf("\n");
    }
  }
  // FIXME: move somewhere else?
  printf("\n");

  if (tests_failed > 0) {
    return FAIL;
  }

  return PASS;
}

test_result run_test(test_params params) {
  CPU_run(&params.cpu, 0);

  uint8_t failed = 0;
  failed += expect_equals(params.cpu.registers.a, params.expected_a_val);
  failed += expect_equals(params.cpu.registers.f, params.expected_flags);

  test_result result = {
    .verdict = failed == 0 ? PASS : FAIL,
    .actual_flags = params.cpu.registers.f,
    .actual_a_val = params.cpu.registers.a,
    .expected_flags = params.expected_flags,
    .expected_a_val = params.expected_a_val,
  };

  return result;
}

// Test groups

test_verdict run_add_tests() {
  test_params add_params[6] = {
    {
      .cpu = {
        .bus = {
          .memory = {0x80, 0x10},
        },
        .registers = {
          .a = 4,
          .b = 6,
        }
      },
      .expected_a_val = 10,
      .expected_flags = 0b00000000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x80, 0x10},
        },
        .registers = {
          .a = 4,
          .b = 255,
        }
      },
      .expected_a_val = 3,
      .expected_flags = 0b00110000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x80, 0x10},
        },
        .registers = {
          .a = 128,
          .b = 128,
        }
      },
      .expected_a_val = 0,
      .expected_flags = 0b10010000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x81, 0x10},
        },
        .registers = {
          .a = 28,
          .c = 12,
        }
      },
      .expected_a_val = 40,
      .expected_flags = 0b00100000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x82, 0x10},
        },
        .registers = {
          .a = 100,
          .d = 72,
        }
      },
      .expected_a_val = 172,
      .expected_flags = 0b00000000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x83, 0x10},
        },
        .registers = {
          .a = 31,
          .e = 94,
        }
      },
      .expected_a_val = 125,
      .expected_flags = 0b00100000,
    },
  };

  return test_suite(run_test, 6, add_params);
}


test_verdict run_adc_tests() {
  test_params add_params[12] = {
    { 
      .cpu = {
        .bus = {
          .memory = {0x88, 0x10},
        },
        .registers = {
          .a = 6,
          .b = 4,
        }
      },
      .expected_a_val = 10,
      .expected_flags = 0b00000000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x88, 0x10},
        },
        .registers = {
          .a = 6,
          .b = 4,
          .f = 0b00010000,
        }
      },
      .expected_a_val = 11,
      .expected_flags = 0b00000000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x88, 0x10},
        },
        .registers = {
          .a = 255,
          .b = 4,
        }
      },
      .expected_a_val = 3,
      .expected_flags = 0b00110000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x88, 0x10},
        },
        .registers = {
          .a = 128,
          .b = 128,
        }
      },
      .expected_a_val = 0,
      .expected_flags = 0b10010000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x89, 0x10},
        },
        .registers = {
          .a = 28,
          .c = 12,
        }
      },
      .expected_a_val = 40,
      .expected_flags = 0b00100000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x8A, 0x10},
        },
        .registers = {
          .a = 100,
          .d = 72,
        }
      },
      .expected_a_val = 172,
      .expected_flags = 0b00000000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x8B, 0x10},
        },
        .registers = {
          .a = 31,
          .e = 94,
        }
      },
      .expected_a_val = 125,
      .expected_flags = 0b00100000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x88, 0x10},
        },
        .registers = {
          .a = 5,
          .b = 254,
          .f = 0b00010000,
        }
      },
      .expected_a_val = 4,
      .expected_flags = 0b00110000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x88, 0x10},
        },
        .registers = {
          .a = 128,
          .b = 127,
          .f = 0b00010000,
        }
      },
      .expected_a_val = 0,
      .expected_flags = 0b10110000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x89, 0x10},
        },
        .registers = {
          .a = 28,
          .c = 12,
          .f = 0b00010000,
        }
      },
      .expected_a_val = 41,
      .expected_flags = 0b00100000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x8A, 0x10},
        },
        .registers = {
          .a = 100,
          .d = 72,
          .f = 0b00010000,
        }
      },
      .expected_a_val = 173,
      .expected_flags = 0b00000000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x8B, 0x10},
        },
        .registers = {
          .a = 31,
          .e = 94,
          .f = 0b00010000,
        }
      },
      .expected_a_val = 126,
      .expected_flags = 0b00100000,
    },
  };

  return test_suite(run_test, 12, add_params);
}

test_verdict run_sub_tests() {
  test_params sub_params[6] = {
    {
      .cpu = {
        .bus = {
          .memory = {0x90, 0x10},
        },
        .registers = {
          .a = 6,
          .b = 4,
        }
      },
      .expected_a_val = 2,
      .expected_flags = 0b01000000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x90, 0x10},
        },
        .registers = {
          .a = 0,
          .b = 4,
        }
      },
      .expected_a_val = 251,
      .expected_flags = 0b01110000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x90, 0x10},
        },
        .registers = {
          .a = 128,
          .b = 128,
        }
      },
      .expected_a_val = 0,
      .expected_flags = 0b11000000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x91, 0x10},
        },
        .registers = {
          .a = 28,
          .c = 12,
        }
      },
      .expected_a_val = 16,
      .expected_flags = 0b01000000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x92, 0x10},
        },
        .registers = {
          .a = 100,
          .d = 72,
        }
      },
      .expected_a_val = 28,
      .expected_flags = 0b01100000,
    },
    {
      .cpu = {
        .bus = {
          .memory = {0x93, 0x10},
        },
        .registers = {
          .a = 31,
          .e = 94,
        }
      },
      .expected_a_val = 192,
      .expected_flags = 0b01010000,
    },
  };

  return test_suite(run_test, 6, sub_params);
}

test_verdict test_cpu() {
  long tests_failed = 0;
  tests_failed += run_add_tests();
  tests_failed += run_adc_tests();
  tests_failed += run_sub_tests();

  return tests_failed > 0 ? FAIL : PASS;
}

