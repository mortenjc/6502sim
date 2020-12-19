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

  const uint16_t SPBase{0x0100};

  uint16_t getSPAddr() {
    return SPBase + S;
  }
  const uint16_t power_on_reset_addr{0xFFFC};

  Opcode instset[256];

  // Debug and trace
  bool running{true};
  bool debugPrint{false};
  bool bpAddrCheck{false};
  uint16_t bpAddr;
  bool bpRegCheck{false};
  uint8_t bpA, bpX, bpY;

  // CPU registers
  uint8_t A, X, Y;
  uint16_t PC;
  uint8_t S{255}; // Stack pointer, address is SPbase + S
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


  // returns the number of operands for opcode (to adjust PC)
  int operands(AMode mode);

  void setBreakpointAddr(uint16_t addr) {
    bpAddrCheck = true;
    bpAddr = addr;
  }

  void setBreakpointRegs(uint8_t A, uint8_t X, uint8_t Y) {
    bpRegCheck = true;
    bpA = A;
    bpX = X;
    bpY = Y;
  }

  bool bpCheck() {
    if (bpAddrCheck and bpRegCheck) {
      if ((PC >= bpAddr) and (bpA == A) and (bpX == X) and (bpY == Y)) {
        return true;
      } else if (bpAddrCheck and not bpRegCheck) {
        if (PC >= bpAddr) {
          return true;
        }
      } else if (not bpAddrCheck and bpRegCheck) {
        if ((bpA == A) and (bpX == X) and (bpY == Y)) {
          return true;
        }
      }
    }
    return false;
  }


  void run(unsigned int n) {
    while (running and (n > 0)) {
      uint8_t instruction = getInstruction();
      handleInstruction(instruction);
      n--;

      if (bpCheck()) {
        printf("\nBREAK\n");
        return;
      }
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
  Status.bits.C = 0;
  Status.bits.N = 0;
  Status.bits.Z = 0;
  unsigned int tmp = reg - value;
  //printf("0x%04x\n", tmp);
	if (reg >= value) {
    Status.bits.C = 1;
  }
	if (tmp & 0x80) {
    Status.bits.N = 1;
  }
	if (reg == value) {
    Status.bits.Z = 1;
  }
	return;
}



private:

  int addcarry(uint8_t & reg, uint8_t val);

  void transfer(uint8_t & src, uint8_t & dst) {
    dst = src;
    updateStatus(dst);
  }

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
    {0x09, "ORA", Immediate, na},
    {0x10, "BPL", Relative,  na},
    {0x18, "CLC", Implicit,  na},
    {0x20, "JSR", Absolute,  na},
    {0x24, "BIT", ZeroPage,  na},
    {0x29, "AND", Immediate, na},
    {0x2C, "BIT", Absolute,  na},
    {0x30, "BMI", Relative,  na},
    {0x38, "SEC", Implicit,  na},
    {0x49, "EOR", Immediate, na},
    {0x4C, "JMP", Absolute,  na},
    {0x60, "RTS", Implicit,  na},
    {0x65, "ADC", ZeroPage,  na},
    {0x69, "ADC", Immediate, na},
    {0x6D, "ADC", Absolute,  na},
    {0x84, "STY", ZeroPage,  na},
    {0x85, "STA", ZeroPage,  na},
    {0x86, "STX", ZeroPage,  na},
    {0x88, "DEY", Implicit,  dec},
    {0x8A, "TXA", Implicit,  na},
    {0x8C, "STY", Absolute,  na},
    {0x8E, "STX", Absolute,  na},
    {0x90, "BCC", Relative,  na},
    {0x94, "STY", ZeroPageX, na},
    {0x96, "STX", ZeroPageY, na},
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
    {0xAA, "TAX", Implicit,  na},
    {0xAC, "LDY", Absolute,  load},
    {0xAD, "LDA", Absolute,  load},
    {0xAE, "LDX", Absolute,  load},
    {0xB5, "LDA", ZeroPageX, load},
    {0xB6, "LDX", ZeroPageY, load},
    {0xB9, "LDA", AbsoluteY, load},
    {0xBC, "LDY", AbsoluteX, load},
    {0xBD, "LDA", AbsoluteX, load},
    {0xBE, "LDX", AbsoluteY, load},
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
