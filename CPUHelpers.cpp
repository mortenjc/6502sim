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


int CPU::branch(std::string opcode, uint8_t flag, int8_t onoff) {
  int pcinc = 2;
  uint8_t val = mem.readByte(PC + 1);
  if (flag == onoff) {
      if (val & 0x80) {
        pcinc = - (val - 0x80) + 1;
      } else {
        pcinc = val;
      }
  }
  debug(opcode, val, 0xFFFF, CPU::Immediate);
  return pcinc;
}

int CPU::addcarry(std::string opcode, uint8_t & reg, AMode mode) {
  uint8_t val;
  int pcinc = 2;
  uint16_t addr{0xFFFF};

  // Get value (I, ZP, A)
  if (mode == Immediate) {
    val = mem.readByte(PC + 1);
  } else if (mode == ZeroPage) {
    addr = mem.readByte(PC + 1);
    val = mem.readByte(addr);
  } else if (mode == Absolute) {
    addr = mem.readWord(PC + 1);
    val = mem.readByte(addr);
    pcinc = 3;
  } else {
    printf("Unsupported mode, exiting ...\n");
    exit(0);
  }

  // Actual addcarry routine
  uint8_t tmp = reg + val + Status.bits.C;
  //printf("\nA + zpval + C == 0x%02x + 0x%02x + %d\n", A, zpval, Status.bits.C);
  if ((reg + val + Status.bits.C) > 255) {
    Status.bits.C = 1; // Set carry
  } else {
    Status.bits.C = 0; // Clear carry
  }

  reg = tmp;
  updateStatus(reg);
  pcinc = 2;
  debug("ADC", val, addr, mode);
  return pcinc;
}


int CPU::compare(std::string opcode, uint8_t & reg, AMode mode) {
  Status.bits.C = 0;
  Status.bits.Z = 0;
  Status.bits.N = 0;
  uint8_t val;
  int pcinc = 2;
  uint16_t addr{0xFFFF};

  // Get value (I, ZP, A)
  if (mode == Immediate) {
    val = mem.readByte(PC + 1);
  } else if (mode == ZeroPage) {
    addr = mem.readByte(PC + 1);
    val = mem.readByte(addr);
  } else if (mode == Absolute) {
    addr = mem.readWord(PC + 1);
    val = mem.readByte(addr);
    pcinc = 3;
  } else {
    printf("Unsupported mode, exiting ...\n");
    exit(0);
  }

  // The actual comparison feature
  //printf("debug: (reg, val, reg-val) 0x%02x, 0x%02x, 0x%02x\n", reg, val, (uint8_t)(256 + reg - val));
  if (reg > val) {
    if ((reg - val) & 0x80) {
      Status.bits.N = 1;
    }
  } else if (reg == val) {
    Status.bits.Z = 1;
    Status.bits.C = 1;
  } else if (reg < val) {
    if ((reg - val) & 0x80) {
      Status.bits.N = 1;
    }
  }

  debug(opcode, val, addr, mode);

  return pcinc;
}


int CPU::load(std::string opcode, uint8_t & reg, AMode mode) {
  uint8_t val;
  int pcinc = 2;
  uint16_t addr{0xFFFF};

  // Get value (I, ZP, A)
  if (mode == Immediate) {
    val = mem.readByte(PC + 1);
  } else if (mode == ZeroPage) {
    addr = mem.readByte(PC + 1);
    val = mem.readByte(addr);
  } else if (mode == Absolute) {
    addr = mem.readWord(PC + 1);
    val = mem.readByte(addr);
    pcinc = 3;
  } else if (mode == AbsoluteX) {
    addr = mem.readWord(PC + 1);
    val = mem.readByte(addr + X);
    pcinc = 3;
  } else {
    printf("Unsupported mode, exiting ...\n");
    exit(0);
  }

  //
  reg = val;
  updateStatus(reg);

  debug(opcode, reg, addr, mode);
  return pcinc;
}
