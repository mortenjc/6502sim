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

#pragma once

#include <Memory.h>
#include <cstdint>
#include <string>

class CPU {
public:
  enum AMode { Implicit, Accumulator, Immediate,
               ZeroPage, ZeroPageX, ZeroPageY,
               Relative, Absolute,  AbsoluteX, AbsoluteY,
               Indirect, IndexedIndirect, IndirectIndexed};

  struct Opcode {
   uint8_t opcode;
   std::string mnem;
   AMode mode;
   int (*pf)(CPU * cpu, uint8_t & reg);
  };

  const uint16_t stack_pointer_addr{0x01FF};
  const uint16_t power_on_reset_addr{0xFFFC};

  Opcode instset[256];

  bool running{true};
  bool debugPrint{false};

  uint8_t A, X, Y;
  uint16_t PC;
  uint16_t SP;
  union {
    struct {
      uint8_t C : 1;
      uint8_t Z : 1;
      uint8_t I : 1;
      uint8_t D : 1;
      uint8_t B : 1;
      uint8_t O : 1;
      uint8_t N : 1;
    } bits;
    uint8_t mask;
  } Status;
  Memory & mem;


  CPU(Memory & memory) : mem(memory) {
    for (int i = 0; i < 256; i++) {
      instset[i] = {0xFF, "---", Implicit};
    }
    for (auto & opc : Opcodes) {
      instset[opc.opcode] = opc;
    }
  };


  void debugOn() { debugPrint = true; }
  void debug();
  void disAssemble(Opcode opc, uint8_t byte, uint16_t word);

  int operands(AMode mode);



  void run(unsigned int n) {
    while (running and (n > 0)) {
      uint8_t instruction = getInstruction();
      handleInstruction(instruction);
      n--;
    }
  }


  // Reset CPU - clear registers, set program counter
  void reset();


  //
  uint8_t getInstruction() {
    return mem.readByte(PC);
  }



  // This is where the action is
  bool handleInstruction(uint8_t instruction);


  // Updates Zero and Negative flags based on value
  void updateStatus(uint8_t value) {
    Status.bits.Z = 0;
    Status.bits.N = 0;
    if (value == 0) {
      Status.bits.Z = 1;
    }
    if (value & 0x80) {
      Status.bits.N = 1;
    }
  }

void updateCompare(uint8_t & reg, uint8_t value) {
  if (reg > value) {
    if ((reg - value) & 0x80) {
      Status.bits.N = 1;
    }
  } else if (reg == value) {
    Status.bits.Z = 1;
    Status.bits.C = 1;
  } else if (reg < value) {
    if ((reg - value) & 0x80) {
      Status.bits.N = 1;
    }
  }
}



private:

  int addcarry(uint8_t & reg, uint8_t val);

  int16_t jumpRelative(uint8_t val);



  static int load(CPU * cpu, uint8_t & reg) {
    cpu->updateStatus(reg);
    return 0;
  }

  static int inc(CPU * cpu, uint8_t & reg) {
    reg = reg + 1;
    cpu->updateStatus(reg);
    return 0;
  }

  static int dec(CPU * cpu, uint8_t & reg) {
    reg = reg - 1;
    cpu->updateStatus(reg);
    return 0;
  }

  // static int add(CPU * cpu, uint8_t & reg) {
  //   reg = reg + 1;
  //   cpu->updateStatus(reg);
  //   return 0;
  // }

  static int na(CPU * cpu, uint8_t & reg) {
    printf("Not implemented\n");
    exit(0);
    return 0;
  }

  // https://www.masswerk.at/6502/6502_instruction_set.html
  std::vector<Opcode> Opcodes {
    {0x10, "BPL", Relative,  na},
    {0x18, "CLC", Implicit,  na},
    {0x20, "JSR", Absolute,  na},
    {0x29, "AND", Immediate, na},
    {0x30, "BMI", Relative,  na},
    {0x38, "SEC", Implicit,  na},
    {0x4C, "JMP", Absolute,  na},
    {0x60, "RTS", Implicit,  na},
    {0x65, "ADC", ZeroPage,  na},
    {0x69, "ADC", Immediate, na},
    {0x6D, "ADC", Absolute,  na},
    {0x85, "STA", ZeroPage,  na},
    {0x86, "STX", ZeroPage,  na},
    {0x88, "DEY", Implicit,  dec},
    {0x8C, "STY", Absolute,  na},
    {0x8E, "STX", Absolute,  na},
    {0x98, "TYA", Implicit,  na},
    {0x99, "STA", AbsoluteY, na},
    {0x9D, "STA", AbsoluteX, na},
    {0xA0, "LDY", Immediate, load},
    {0xA2, "LDX", Immediate, load},
    {0xA4, "LDY", ZeroPage,  load},
    {0xA5, "LDA", ZeroPage,  load},
    {0xA6, "LDX", ZeroPage,  load},
    {0xA8, "TAY", Implicit,  na},
    {0xA9, "LDA", Immediate, load},
    {0xB9, "LDA", AbsoluteY, load},
    {0xBC, "LDY", AbsoluteX, load},
    {0xBD, "LDA", AbsoluteX, load},
    {0xC0, "CPY", Immediate, na},
    {0xC4, "CPY", ZeroPage,  na},
    {0xC5, "CMP", ZeroPage,  na},
    {0xC8, "INY", Implicit,  inc},
    {0xC9, "CMP", Immediate, na},
    {0xCA, "DEX", Implicit,  dec},
    {0xCC, "CPY", Absolute,  na},
    {0xCD, "CMP", Absolute,  na},
    {0xD0, "BNE", Relative,  na},
    {0xD8, "CLD", Implicit,  na},
    {0xE0, "CPX", Immediate, na},
    {0xE4, "CPX", ZeroPage,  na},
    {0xE8, "INX", Implicit,  inc},
    {0xEE, "INC", Absolute,  na},
    {0xEA, "NOP", Implicit,  na},
    {0xEC, "CPX", Absolute,  na},
    {0xF0, "BEQ", Relative,  na},
    {0xF8, "SED", Implicit,  na}
  };

};
