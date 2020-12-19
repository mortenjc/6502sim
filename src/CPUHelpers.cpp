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
    jump = - (val - 0x80) + 1;
  } else {
    jump = val;
  }
  return jump;
}


int CPU::addcarry(uint8_t & reg, uint8_t val) {
  // Actual addcarry routine
  uint8_t tmp = reg + val + Status.bits.C;

  if ((reg + val + Status.bits.C) > 255) {
    Status.bits.C = 1; // Set carry
  } else {
    Status.bits.C = 0; // Clear carry
  }

  reg = tmp;
  updateStatus(reg);

  return 0;
}
