#include "test.h"
#include "registers.h"
#include <stdint.h>
#include <stdio.h>

void print_bin(uint8_t n) {
  for (int i = 7; i >= 0; i -= 1) {
    uint8_t bit = (n >> i) & 1;
    printf("%d", bit);
  }
}

test_verdict expect_equals(uint8_t it, uint8_t expected) {
  return it == expected ? PASS : FAIL;
}

void print_test_failures(test_result result) {
  if (result.expected_registers.a != result.actual_registers.a) {
    printf("Expected A to be: ");
    print_bin(result.expected_registers.a);
    printf("\n");
    printf("             was: ");
    print_bin(result.actual_registers.a);
    printf("\n");
  }

  if (result.expected_registers.b != result.actual_registers.b) {
    printf("Expected B to be: ");
    print_bin(result.expected_registers.b);
    printf("\n");
    printf("             was: ");
    print_bin(result.actual_registers.b);
    printf("\n");
  }

  if (result.expected_registers.c != result.actual_registers.c) {
    printf("Expected C to be: ");
    print_bin(result.expected_registers.c);
    printf("\n");
    printf("             was: ");
    print_bin(result.actual_registers.c);
    printf("\n");
  }

  if (result.expected_registers.d != result.actual_registers.d) {
    printf("Expected D to be: ");
    print_bin(result.expected_registers.d);
    printf("\n");
    printf("             was: ");
    print_bin(result.actual_registers.d);
    printf("\n");
  }

  if (result.expected_registers.e != result.actual_registers.e) {
    printf("Expected E to be: ");
    print_bin(result.expected_registers.e);
    printf("\n");
    printf("             was: ");
    print_bin(result.actual_registers.e);
    printf("\n");
  }

  if (result.expected_registers.h != result.actual_registers.h) {
    printf("Expected H to be: ");
    print_bin(result.expected_registers.h);
    printf("\n");
    printf("             was: ");
    print_bin(result.actual_registers.h);
    printf("\n");
  }

  if (result.expected_registers.l != result.actual_registers.l) {
    printf("Expected L to be: ");
    print_bin(result.expected_registers.l);
    printf("\n");
    printf("             was: ");
    print_bin(result.actual_registers.l);
    printf("\n");
  }

  if (result.expected_registers.f != result.actual_registers.f) {
    printf("Expected F to be: ");
    print_bin(result.expected_registers.f);
    printf("\n");
    printf("             was: ");
    print_bin(result.actual_registers.f);
  }
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
      print_test_failures(result);
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

  failed += expect_equals(params.cpu.registers.a, params.expected_registers.a);
  failed += expect_equals(params.cpu.registers.b, params.expected_registers.b);
  failed += expect_equals(params.cpu.registers.c, params.expected_registers.c);
  failed += expect_equals(params.cpu.registers.d, params.expected_registers.d);
  failed += expect_equals(params.cpu.registers.e, params.expected_registers.e);
  failed += expect_equals(params.cpu.registers.h, params.expected_registers.h);
  failed += expect_equals(params.cpu.registers.l, params.expected_registers.l);
  failed += expect_equals(params.cpu.registers.f, params.expected_registers.f);

  test_result result = {
    .verdict = failed == 0 ? PASS : FAIL,
    .actual_registers = params.cpu.registers,
    .expected_registers = params.expected_registers
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
      .expected_registers = {
	.a = 10,
	.b = 6,
	.f = 0b00000000,
      }
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
      .expected_registers = {
	.a = 3,
	.b = 255,
	.f = 0b00110000,
      }
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
      .expected_registers = {
	.a = 0,
	.b = 128,
	.f = 0b10010000,
      }
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
      .expected_registers = {
	.a = 40,
	.c = 12,
	.f = 0b00100000,
      }
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
      .expected_registers = {
	.a = 172,
	.d = 72,
	.f = 0b00000000,
      }
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
      .expected_registers = {
	.a = 125,
	.e = 94,
	.f = 0b00100000,
      }
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
      .expected_registers = {
	.a = 10,
	.b = 4,
	.f = 0b00000000,
      }
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
      .expected_registers = {
	.a = 11,
	.b = 4,
	.f = 0b00000000,
      }
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
      .expected_registers = {
	.a = 3,
	.b = 4,
	.f = 0b00110000,
      }
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
      .expected_registers = {
	.a = 0,
	.b = 128,
	.f = 0b10010000,
      }
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
      .expected_registers = {
	.a = 40,
	.c = 12,
	.f = 0b00100000,
      }
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
      .expected_registers = {
	.a = 172,
	.d = 72,
	.f = 0b00000000,
      }
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
      .expected_registers = {
	.a = 125,
	.e = 94,
	.f = 0b00100000,
      }
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
      .expected_registers = {
	.a = 4,
	.b = 254,
	.f = 0b00110000,
      }
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
      .expected_registers = {
	.a = 0,
	.b = 127,
	.f = 0b10110000,
      }
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
      .expected_registers = {
	.a = 41,
	.c = 12,
	.f = 0b00100000,
      }
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
      .expected_registers = {
	.a = 173,
	.d = 72,
	.f = 0b00000000,
      }
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
      .expected_registers = {
	.a = 126,
	.e = 94,
	.f = 0b00100000,
      }
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
      .expected_registers = {
	.a = 2,
	.b = 4,
	.f = 0b01000000,
      }
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
      .expected_registers = {
	.a = 251,
	.b = 4,
	.f = 0b01110000,
      }
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
      .expected_registers = {
	.a = 0,
	.b = 128,
	.f = 0b11000000,
      }
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
      .expected_registers = {
	.a = 16,
	.c = 12,
	.f = 0b01000000,
      }
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
      .expected_registers = {
	.a = 28,
	.d = 72,
	.f = 0b01100000,
      }
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
      .expected_registers = {
	.a = 192,
	.e = 94,
	.f = 0b01010000,
      }
    },
  };

  return test_suite(run_test, 6, sub_params);
}

test_verdict run_ld_tests() {
  test_params ld_params[64] = {
    // LD B, B
    {
      .cpu = {
        .bus = {
          .memory = {0x40, 0x10},
        },
        .registers = {
	  .b = 0x42
	}
      },
      .expected_registers = {
	.b = 0x42,
      }
    },
    // LD B, C
    {
      .cpu = {
        .bus = {
          .memory = {0x41, 0x10},
        },
        .registers = {
	  .c = 0x42
	}
      },
      .expected_registers = {
	.b = 0x42,
	.c = 0x42,
      }
    },
    // LD B, D
    {
      .cpu = {
        .bus = {
          .memory = {0x42, 0x10},
        },
        .registers = {
	  .d = 0x42,
	}
      },
      .expected_registers = {
	.b = 0x42,
	.d = 0x42,
      }
    },
    // LD B, E
    {
      .cpu = {
        .bus = {
          .memory = {0x43, 0x10},
        },
        .registers = {
	  .e = 0x42
	}
      },
      .expected_registers = {
	.b = 0x42,
	.e = 0x42
      }
    },
    // LD B, H
    {
      .cpu = {
        .bus = {
          .memory = {0x44, 0x10},
        },
        .registers = {
	  .h = 0x42
	}
      },
      .expected_registers = {
	.b = 0x42,
	.h = 0x42,
      }
    },
    // LD B, L
    {
      .cpu = {
        .bus = {
          .memory = {0x45, 0x10},
        },
        .registers = {
	  .l = 0x42
	}
      },
      .expected_registers = {
	.b = 0x42,
	.l = 0x42,
      }
    },
    // LD B, (HL)
    {
      .cpu = {
        .bus = {
          .memory = {0x00, 0x46, 0x10},
        },
        .registers = {
	  .b = 0x42,
	  // Points to 0x00 -> the value is 0 -- NOP
	  .h = 0x0,
	  .l = 0x0,
	}
      },
      .expected_registers = {
	.b = 0x0,
	.h = 0x0,
	.l = 0x0,
      }
    },
    // LD B, A
    {
      .cpu = {
        .bus = {
          .memory = {0x47, 0x10},
        },
        .registers = {
	  .a = 0x42
	}
      },
      .expected_registers = {
	.b = 0x42,
	.a = 0x42,
      }
    },

    // LD C, B
    {
      .cpu = {
        .bus = {
          .memory = {0x48, 0x10},
        },
        .registers = {
	  .b = 0x42
	}
      },
      .expected_registers = {
	.b = 0x42,
	.c = 0x42,
      }
    },
    // LD C, C
    {
      .cpu = {
        .bus = {
          .memory = {0x49, 0x10},
        },
        .registers = {
	  .c = 0x42
	}
      },
      .expected_registers = {
	.c = 0x42,
      }
    },
    // LD C, D
    {
      .cpu = {
        .bus = {
          .memory = {0x4A, 0x10},
        },
        .registers = {
	  .d = 0x42,
	}
      },
      .expected_registers = {
	.c = 0x42,
	.d = 0x42,
      }
    },
    // LD C, E
    {
      .cpu = {
        .bus = {
          .memory = {0x4B, 0x10},
        },
        .registers = {
	  .e = 0x42
	}
      },
      .expected_registers = {
	.c = 0x42,
	.e = 0x42
      }
    },
    // LD C, H
    {
      .cpu = {
        .bus = {
          .memory = {0x4C, 0x10},
        },
        .registers = {
	  .h = 0x42
	}
      },
      .expected_registers = {
	.c = 0x42,
	.h = 0x42,
      }
    },
    // LD C, L
    {
      .cpu = {
        .bus = {
          .memory = {0x4D, 0x10},
        },
        .registers = {
	  .l = 0x42
	}
      },
      .expected_registers = {
	.c = 0x42,
	.l = 0x42,
      }
    },
    // LD C, (HL)
    {
      .cpu = {
        .bus = {
          .memory = {0x00, 0x4E, 0x10},
        },
        .registers = {
	  .c = 0x42,
	  // Points to 0x00 -> the value is 0 -- NOP
	  .h = 0x0,
	  .l = 0x0,
	}
      },
      .expected_registers = {
	.c = 0x0,
	.h = 0x0,
	.l = 0x0,
      }
    },
    // LD C, A
    {
      .cpu = {
        .bus = {
          .memory = {0x4F, 0x10},
        },
        .registers = {
	  .a = 0x42
	}
      },
      .expected_registers = {
	.c = 0x42,
	.a = 0x42,
      }
    },
    // LD D, B
    {
      .cpu = {
        .bus = {
          .memory = {0x50, 0x10},
        },
        .registers = {
	  .b = 0x42
	}
      },
      .expected_registers = {
	.b = 0x42,
	.d = 0x42,
      }
    },
    // LD D, C
    {
      .cpu = {
        .bus = {
          .memory = {0x51, 0x10},
        },
        .registers = {
	  .c = 0x42
	}
      },
      .expected_registers = {
	.d = 0x42,
	.c = 0x42,
      }
    },
    // LD D, D
    {
      .cpu = {
        .bus = {
          .memory = {0x52, 0x10},
        },
        .registers = {
	  .d = 0x42,
	}
      },
      .expected_registers = {
	.d = 0x42,
      }
    },
    // LD D, E
    {
      .cpu = {
        .bus = {
          .memory = {0x53, 0x10},
        },
        .registers = {
	  .e = 0x42
	}
      },
      .expected_registers = {
	.d = 0x42,
	.e = 0x42
      }
    },
    // LD D, H
    {
      .cpu = {
        .bus = {
          .memory = {0x54, 0x10},
        },
        .registers = {
	  .h = 0x42
	}
      },
      .expected_registers = {
	.d = 0x42,
	.h = 0x42,
      }
    },
    // LD D, L
    {
      .cpu = {
        .bus = {
          .memory = {0x55, 0x10},
        },
        .registers = {
	  .l = 0x42
	}
      },
      .expected_registers = {
	.d = 0x42,
	.l = 0x42,
      }
    },
    // LD D, (HL)
    {
      .cpu = {
        .bus = {
          .memory = {0x00, 0x56, 0x10},
        },
        .registers = {
	  .d = 0x42,
	  // Points to 0x00 -> the value is 0 -- NOP
	  .h = 0x0,
	  .l = 0x0,
	}
      },
      .expected_registers = {
	.d = 0x0,
	.h = 0x0,
	.l = 0x0,
      }
    },
    // LD D, A
    {
      .cpu = {
        .bus = {
          .memory = {0x57, 0x10},
        },
        .registers = {
	  .a = 0x42
	}
      },
      .expected_registers = {
	.d = 0x42,
	.a = 0x42,
      }
    },

    // LD E, B
    {
      .cpu = {
        .bus = {
          .memory = {0x58, 0x10},
        },
        .registers = {
	  .b = 0x42
	}
      },
      .expected_registers = {
	.b = 0x42,
	.e = 0x42,
      }
    },
    // LD E, C
    {
      .cpu = {
        .bus = {
          .memory = {0x59, 0x10},
        },
        .registers = {
	  .c = 0x42
	}
      },
      .expected_registers = {
	.c = 0x42,
	.e = 0x42,
      }
    },
    // LD E, D
    {
      .cpu = {
        .bus = {
          .memory = {0x5A, 0x10},
        },
        .registers = {
	  .d = 0x42,
	}
      },
      .expected_registers = {
	.e = 0x42,
	.d = 0x42,
      }
    },
    // LD E, E
    {
      .cpu = {
        .bus = {
          .memory = {0x5B, 0x10},
        },
        .registers = {
	  .e = 0x42
	}
      },
      .expected_registers = {
	.e = 0x42
      }
    },
    // LD E, H
    {
      .cpu = {
        .bus = {
          .memory = {0x5C, 0x10},
        },
        .registers = {
	  .h = 0x42
	}
      },
      .expected_registers = {
	.e = 0x42,
	.h = 0x42,
      }
    },
    // LD E, L
    {
      .cpu = {
        .bus = {
          .memory = {0x5D, 0x10},
        },
        .registers = {
	  .l = 0x42
	}
      },
      .expected_registers = {
	.e = 0x42,
	.l = 0x42,
      }
    },
    // LD E, (HL)
    {
      .cpu = {
        .bus = {
          .memory = {0x00, 0x5E, 0x10},
        },
        .registers = {
	  .e = 0x42,
	  // Points to 0x00 -> the value is 0 -- NOP
	  .h = 0x0,
	  .l = 0x0,
	}
      },
      .expected_registers = {
	.e = 0x0,
	.h = 0x0,
	.l = 0x0,
      }
    },
    // LD E, A
    {
      .cpu = {
        .bus = {
          .memory = {0x5F, 0x10},
        },
        .registers = {
	  .a = 0x42
	}
      },
      .expected_registers = {
	.e = 0x42,
	.a = 0x42,
      }
    },
    // LD H, B
    {
      .cpu = {
        .bus = {
          .memory = {0x60, 0x10},
        },
        .registers = {
	  .b = 0x42
	}
      },
      .expected_registers = {
	.b = 0x42,
	.h = 0x42,
      }
    },
    // LD H, C
    {
      .cpu = {
        .bus = {
          .memory = {0x61, 0x10},
        },
        .registers = {
	  .c = 0x42
	}
      },
      .expected_registers = {
	.h = 0x42,
	.c = 0x42,
      }
    },
    // LD H, D
    {
      .cpu = {
        .bus = {
          .memory = {0x62, 0x10},
        },
        .registers = {
	  .d = 0x42,
	}
      },
      .expected_registers = {
	.d = 0x42,
        .h = 0x42,
      }
    },
    // LD H, E
    {
      .cpu = {
        .bus = {
          .memory = {0x63, 0x10},
        },
        .registers = {
	  .e = 0x42
	}
      },
      .expected_registers = {
	.h = 0x42,
	.e = 0x42
      }
    },
    // LD H, H
    {
      .cpu = {
        .bus = {
          .memory = {0x64, 0x10},
        },
        .registers = {
	  .h = 0x42
	}
      },
      .expected_registers = {
	.h = 0x42,
      }
    },
    // LD H, L
    {
      .cpu = {
        .bus = {
          .memory = {0x65, 0x10},
        },
        .registers = {
	  .l = 0x42
	}
      },
      .expected_registers = {
	.h = 0x42,
	.l = 0x42,
      }
    },
    // LD H, (HL)
    {
      .cpu = {
        .bus = {
          .memory = {0x00, 0x66, 0x10},
        },
        .registers = {
	  // Points to 0x00 -> the value is 0 -- NOP
	  .h = 0x0,
	  .l = 0x0,
	}
      },
      .expected_registers = {
	.h = 0x0,
	.l = 0x0,
      }
    },
    // LD H, A
    {
      .cpu = {
        .bus = {
          .memory = {0x67, 0x10},
        },
        .registers = {
	  .a = 0x42
	}
      },
      .expected_registers = {
	.h = 0x42,
	.a = 0x42,
      }
    },
    // LD L, B
    {
      .cpu = {
        .bus = {
          .memory = {0x68, 0x10},
        },
        .registers = {
	  .b = 0x42
	}
      },
      .expected_registers = {
	.b = 0x42,
	.l = 0x42,
      }
    },
    // LD L, C
    {
      .cpu = {
        .bus = {
          .memory = {0x69, 0x10},
        },
        .registers = {
	  .c = 0x42
	}
      },
      .expected_registers = {
	.c = 0x42,
	.l = 0x42,
      }
    },
    // LD L, D
    {
      .cpu = {
        .bus = {
          .memory = {0x6A, 0x10},
        },
        .registers = {
	  .d = 0x42,
	}
      },
      .expected_registers = {
	.l = 0x42,
	.d = 0x42,
      }
    },
    // LD L, E
    {
      .cpu = {
        .bus = {
          .memory = {0x6B, 0x10},
        },
        .registers = {
	  .e = 0x42
	}
      },
      .expected_registers = {
	.e = 0x42,
	.l = 0x42
      }
    },
    // LD L, H
    {
      .cpu = {
        .bus = {
          .memory = {0x6C, 0x10},
        },
        .registers = {
	  .h = 0x42
	}
      },
      .expected_registers = {
	.l = 0x42,
	.h = 0x42,
      }
    },
    // LD L, L
    {
      .cpu = {
        .bus = {
          .memory = {0x6D, 0x10},
        },
        .registers = {
	  .l = 0x42
	}
      },
      .expected_registers = {
	.l = 0x42,
      }
    },
    // LD L, (HL)
    {
      .cpu = {
        .bus = {
          .memory = {0x00, 0x00, 0x6E, 0x10},
        },
        .registers = {
	  // Points to 0x01 -> the value is 0 -- NOP
	  .h = 0x0,
	  .l = 0x1,
	}
      },
      .expected_registers = {
	.h = 0x0,
	.l = 0x0,
      }
    },
    // LD L, A
    {
      .cpu = {
        .bus = {
          .memory = {0x6F, 0x10},
        },
        .registers = {
	  .a = 0x42
	}
      },
      .expected_registers = {
	.l = 0x42,
	.a = 0x42,
      }
    },

    // LD A, B
    {
      .cpu = {
        .bus = {
          .memory = {0x78, 0x10},
        },
        .registers = {
	  .b = 0x42
	}
      },
      .expected_registers = {
	.a = 0x42,
	.b = 0x42,
      }
    },
    // LD A, C
    {
      .cpu = {
        .bus = {
          .memory = {0x79, 0x10},
        },
        .registers = {
	  .c = 0x42
	}
      },
      .expected_registers = {
	.c = 0x42,
	.a = 0x42,
      }
    },
    // LD A, D
    {
      .cpu = {
        .bus = {
          .memory = {0x7A, 0x10},
        },
        .registers = {
	  .d = 0x42,
	}
      },
      .expected_registers = {
	.a = 0x42,
	.d = 0x42,
      }
    },
    // LD A, E
    {
      .cpu = {
        .bus = {
          .memory = {0x7B, 0x10},
        },
        .registers = {
	  .e = 0x42
	}
      },
      .expected_registers = {
	.e = 0x42,
	.a = 0x42
      }
    },
    // LD A, H
    {
      .cpu = {
        .bus = {
          .memory = {0x7C, 0x10},
        },
        .registers = {
	  .h = 0x42
	}
      },
      .expected_registers = {
	.a = 0x42,
	.h = 0x42,
      }
    },
    // LD A, L
    {
      .cpu = {
        .bus = {
          .memory = {0x7D, 0x10},
        },
        .registers = {
	  .l = 0x42
	}
      },
      .expected_registers = {
	.a = 0x42,
	.l = 0x42,
      }
    },
    // LD A, (HL)
    {
      .cpu = {
        .bus = {
          .memory = {0x00, 0x7E, 0x10},
        },
        .registers = {
          .a = 0x42,
	  // Points to 0x01 -> the value is 0 -- NOP
	  .h = 0x0,
	  .l = 0x0,
	}
      },
      .expected_registers = {
        .a = 0x00,
	.h = 0x0,
	.l = 0x0,
      }
    },
    // LD A, A
    {
      .cpu = {
        .bus = {
          .memory = {0x7F, 0x10},
        },
        .registers = {
	  .a = 0x42
	}
      },
      .expected_registers = {
	.a = 0x42,
      }
    },

    // LD (HL), B
    {
      .cpu = {
        .bus = {
          .memory = {0x70, 0x00, 0x00, 0x00, 0x00, 0x41},
        },
        .registers = {
	  .b = 0x10,
	  .c = 0x69,
          .h = 0x00,
          .l = 0x05,
	}
      },
      .expected_registers = {
	.b = 0x10,
        .c = 0x69,
        .l = 0x05,
      }
    },
    // LD (HL), C
    {
      .cpu = {
        .bus = {
          .memory = {0x71, 0x00, 0x00, 0x00, 0x00, 0x41},
        },
        .registers = {
	  .b = 0x00,
	  .c = 0x10,
          .h = 0x00,
          .l = 0x05,
	}
      },
      .expected_registers = {
	.b = 0x00,
        .c = 0x10,
        .l = 0x05,
      }
    },

    // TODO: Test ALL the LD instructions, as significant shenanigans happen in that ix processing
  };

  return test_suite(run_test, 60, ld_params);
}

test_verdict test_cpu() {
  long tests_failed = 0;
  tests_failed += run_add_tests();
  tests_failed += run_adc_tests();
  tests_failed += run_sub_tests();
  tests_failed += run_ld_tests();

  return tests_failed > 0 ? FAIL : PASS;
}

