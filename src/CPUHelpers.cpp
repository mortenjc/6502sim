// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief 6502 CPU emulator - helper functions for CPU class
///
/// Some generic functions that are parametrised to implement several
/// CPU opcodes on different registers and addressing modes.
//===----------------------------------------------------------------------===//

#include <CPU.h>
#include <Memory.h>


int16_t CPU::jumpRelative(uint8_t val) {
  int16_t jump;
  if (val & 0x80) {
    jump = - (256 - val); 
  } else {
    jump = val;
  }
  return jump;
}


int CPU::addcarry(uint8_t & reg, uint8_t val) {
  Status.bits.O = 0; // Clear overflow
  Status.bits.C = 0; // Clear carry

  unsigned int tmp = reg + val + Status.bits.C;

  if (tmp > 255) {
    Status.bits.C = 1; // Set carry
  }

 if (!((reg ^ val) & 0x80) && ((reg ^ tmp) & 0x80)) {
   Status.bits.O = 1;
 }

  reg = tmp & 0xFF;
  updateStatusZN(reg);

  return 0;
}

int CPU::subcarry(uint8_t & unused, uint8_t M) {
  Status.bits.O = 0;
  Status.bits.C = 0;

  unsigned int tmp = A - M - (Status.bits.C ^ 1);

  if (((A ^ tmp) & 0x80) && ((A ^ M) & 0x80)) {
    Status.bits.O = 1;
  }

  if (tmp < 0x100) {
    Status.bits.C = 1;
  }

  A = (tmp & 0xFF);
  updateStatusZN(A);
  return 0;
}
