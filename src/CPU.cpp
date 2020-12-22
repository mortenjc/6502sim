// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief 6502 CPU emulator
///
/// Handles program counter (PC), stack pointer (SP), the three registers
/// X,Y,A and status flags.
/// The core logic revolves around a loop of the two methods
///    getInstruction()
///    handleInstruction()
///
/// Individual opcodes are implemented in a bit switch statement
/// in handleInstruction (CPU.cpp)
/// Some debugging functionality added
//===----------------------------------------------------------------------===//

#include <cassert>
#include <CPU.h>
#include <Opcodes.h>


CPU::CPU(Memory & memory) : mem(memory) {
  for (int i = 0; i < 256; i++) {
    instset[i] = {0xFF, "---", Implied};
  }
  for (auto & opc : Opcodes) {
    assert(instset[opc.opcode].opcode == 0xFF);
    instset[opc.opcode] = opc;
  }
  reset();
};

// Sets registers and flags to zero
// Sets Stack pointer (SP) to default (0x1FF)
// Sets program counter (PC) to value read from memory
void CPU::reset() {
  A = X = Y = 0;
  Status.mask = 0;
  PC = mem.readWord(power_on_reset_addr);
  S = 0xFF; // SP = SPBase + S
}


void CPU::run(unsigned int n) {
  while (running and (n > 0)) {
    uint8_t instruction = getInstruction();
    handleInstruction(instruction);
    n--;

    if (bpCheck()) {
      printf("<< BREAK >>\n");
      return;
    }
  }
}

// Prints PC, SP, registers and flags
void CPU::printRegisters() {
  if (not debugPrint)
    return;

  printf(" ; 0x%04x(%03x): A:%02x  X:%02x  Y:%02x ", PC, getSPAddr(), A, X, Y);
  printf(" [%c%c%c%c%c%c%c]\n",
      Status.bits.C ? 'c' : ' ' ,
      Status.bits.Z ? 'z' : ' ' ,
      Status.bits.I ? 'i' : ' ' ,
      Status.bits.D ? 'd' : ' ' ,
      Status.bits.B ? 'b' : ' ' ,
      Status.bits.O ? 'o' : ' ' ,
      Status.bits.N ? 'n' : ' ' );
}

// Make a disassembler-like listing of the current command
void CPU::disAssemble(uint16_t addr, Opcode opc, uint8_t byte, uint8_t byte2, uint16_t word) {
  if (not debugPrint)
    return;

  int nbops = operands(opc.mode);
  if (nbops == 1) {
    printf("%04x %02x       ", addr, opc.opcode);
  } else if (nbops == 2) {
    printf("%04x %02x %02x    ", addr, opc.opcode, byte);
  } else {
    printf("%04x %02x %02x %02x ", addr, opc.opcode, byte, byte2);
  }

  printf("%s ", opc.mnem.c_str());

  switch (opc.mode) {
    case IndexedIndirect:
      printf("($%02X,X)     ", byte);
      break;
    case IndirectIndexed:
      printf("($%02X),Y     ", byte);
      break;
    case Implied:
      printf("            ");
      break;
    case Accumulator:
      printf("A           ");
      break;
    case Relative: {
      int delta = (byte & 0x80) ? -(256 - byte) : (byte);
      uint16_t addr = PC + delta + 2;  // disassemble before PC increment
    }
    break;
    case ZeroPage: {
      uint8_t val = mem.readByte(byte);
      printf("$%02X(%3d)    ", byte, val);
    }
    break;
    case ZeroPageX:
      printf("$%02X,X(%3d)  ", byte, X);
      break;
    case ZeroPageY:
      printf("$%02X,Y(%d)  ", byte, Y);
      break;
    case Immediate:
      printf("#$%02X        ", byte);
      break;
    case Absolute:
      printf("$%04X       ", word);
      break;
    case AbsoluteX:
      printf("$%04X,X(%3d)", word, X);
      break;
    case AbsoluteY:
      printf("$%04X,Y(%3d)", word, Y);
      break;
    case Indirect:
      printf("($%04X)     ", word);
      break;
  }
}

int CPU::operands(AMode mode) {
  switch (mode) {
    case Implied:
    case Accumulator:
      return 1;
      break;

    case IndexedIndirect:
    case IndirectIndexed:
    case Relative:
    case ZeroPage:
    case ZeroPageX:
    case ZeroPageY:
    case Immediate:
      return 2;
      break;

    case Absolute:
    case AbsoluteX:
    case AbsoluteY:
    case Indirect:
      return 3;
      break;
  }
}
