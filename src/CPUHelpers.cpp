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
  unsigned int tmp = reg + val + Status.bits.C;
  Status.bits.O = 0; // Clear overflow
  Status.bits.N = 0;
  Status.bits.Z = 0;
  Status.bits.C = 0;

  if ((tmp & 0xFF) == 0x00)
    Status.bits.Z = 1;

  if (Status.bits.D) { // DECIMAL MODE
    if (((reg & 0xF) + (val & 0xF) + Status.bits.C) > 9)
      tmp += 6;

    if (tmp & 0x80)
      Status.bits.N = 1;
    if (!((reg ^ val) & 0x80) && ((reg ^ tmp) & 0x80)) {
      Status.bits.O = 1;
    }
    if (tmp > 0x99) {
      tmp += 96;
    }

    if (tmp > 0x99) {
      Status.bits.C = 1;  // Set carry
    }

  } else { // NORMAL (?) MODE
    if (tmp & 0x80)
      Status.bits.N = 1;

    if (!((reg ^ val) & 0x80) && ((reg ^ tmp) & 0x80)) {
      Status.bits.O = 1;
    }

    if (tmp > 255) {
      Status.bits.C = 1; // Set carry
    }
  }

  reg = tmp & 0xFF;

  return 0;
}




int CPU::subcarry(uint8_t & unused, uint8_t M) {
  unsigned int tmp = A - M - (Status.bits.C ^ 1);

  Status.bits.N = 0;
  if (tmp & 0x80)
    Status.bits.N = 1;

  Status.bits.Z = 0;
  if ((tmp & 0xFF) == 0x00)
    Status.bits.Z = 1;

  Status.bits.O = 0;
  if (((A ^ tmp) & 0x80) && ((A ^ M) & 0x80))
    Status.bits.O =  1;

	if (Status.bits.D == 1) {
		if ( ((A & 0x0F) - (Status.bits.C ^ 1)) < (M & 0x0F))
      tmp -= 6;
		if (tmp > 0x99) {
			tmp -= 0x60;
		}
	} 

  Status.bits.C = 0;
  if (tmp < 0x100)
    Status.bits.C = 1;

	A = (tmp & 0xFF);
  return 0;
}


void CPU::ror(uint16_t addr) {
  uint8_t val = mem.readByte(addr);
  uint8_t oldCarry = Status.bits.C;
  Status.bits.C = (val & 0x01); // bit 0 -> Carry
  val = val >> 1;
  if (oldCarry) {
    val = val + 128;
  }
  mem.writeByte(addr, val);
  updateStatusZN(val);
}


void CPU::rol(uint16_t addr) {
  uint8_t val = mem.readByte(addr);
  uint8_t oldCarry = Status.bits.C;
  Status.bits.C = (val >> 7); // bit 7 -> Carry
  val = val << 1;
  val = val + oldCarry;
  mem.writeByte(addr, val);
  updateStatusZN(val);
}


void CPU::lsr(uint16_t addr) {
  uint8_t val = mem.readByte(addr);
  Status.bits.C = val & 0x01;
  val = val >> 1;
  mem.writeByte(addr, val);
  updateStatusZN(val);
}

void CPU::asl(uint16_t addr) {
  uint8_t val = mem.readByte(addr);
  Status.bits.C = (val >> 7);
  val = val << 1;
  mem.writeByte(addr, val);
  updateStatusZN(val);
}

void CPU::andMem(uint16_t addr) {
  uint8_t val = mem.readByte(addr);
  A = A & val;
  updateStatusZN(A);
}

void CPU::eorMem(uint16_t addr) {
  uint8_t val = mem.readByte(addr);
  A = A ^ val;
  updateStatusZN(A);
}

void CPU::oraMem(uint16_t addr) {
  uint8_t val = mem.readByte(addr);
  A = A | val;
  updateStatusZN(A);
}
