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
/// Individual opcodes are implemented in a big switch statement
/// in handleInstruction (CPU.cpp)
/// Support for line-by-line disassembly and register output
//===----------------------------------------------------------------------===//

#pragma once

#include <Memory.h>
#include <Opcodes.h>
#include <cassert>
#include <cstdint>
#include <string>

class CPU {
public:
  const uint16_t SPBase{0x0100}; // Stack Pointer base address
  const uint16_t power_on_reset_addr{0xFFFC};

  // CPU registers
  uint8_t A, X, Y, S;
  uint16_t PC;
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
  Opcode instset[256];

  // Program behaviour - debug print and breakpoints
  bool running{true};       ///< set to false when illegal/unimplemented inst.
  bool debugPrint{false};   ///< whether to print disassembly and registers
  bool bpAddrCheck{false};  ///< check for breakpoint on address?
  bool bpRegCheck{false};   ///< check for breakpoint on registers?
  uint16_t bpAddr;          ///< break point PC address
  uint8_t bpA, bpX, bpY;    ///< break point register values
  uint64_t instructions{0}; ///< instruction count


  // Load instructions into array, reset cpu registers
  CPU(Memory & memory);

  // Enables disassembly and register printing
  void debugOn() { debugPrint = true; }


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


  // fetsh-execute loop until instruction count, break point or exception
  void run(unsigned int n) ;

  // Reset CPU - clear registers, set program counter
  void reset();

  // SP address calculation
  uint16_t getSPAddr() { return SPBase + S; }

  //
  uint8_t getInstruction() { return mem.readByte(PC); }

  // This is where the action is
  bool handleInstruction(uint8_t instruction);


  // Updates Zero and Negative flags based on value
  void updateStatusZN(uint8_t value) {
    Status.bits.Z = 0;
    Status.bits.N = 0;
    if (value == 0) {
      Status.bits.Z = 1;
    }
    if (value & 0x80) {
      Status.bits.N = 1;
    }
  }


  int getNumOpcodes() { return Opcodes.size(); }


private:
  // returns the number of operands for opcode (to adjust PC)
  int operands(AMode mode);

  void printRegisters();
  void disAssemble(uint16_t addr, Opcode opc, uint8_t byte, uint8_t byte2, uint16_t word);

  bool bpCheck() {
    if (bpAddrCheck and bpRegCheck) {
      if ((PC >= bpAddr) and (bpA == A) and (bpX == X) and (bpY == Y)) {
        return true;
      }
    } else if (bpAddrCheck) {
      if (PC >= bpAddr) {
        return true;
      }
    } else if (bpRegCheck) {
      if ((bpA == A) and (bpX == X) and (bpY == Y)) {
        return true;
      }
    }
    return false;
  }

  void updateCompare(uint8_t & reg, uint8_t value) {
    Status.bits.C = 0;
    Status.bits.N = 0;
    Status.bits.Z = 0;
    unsigned int tmp = reg - value;
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

  // Common helpers for similar opcodes
  int addcarry(uint8_t & reg, uint8_t val);
  int subcarry(uint8_t & unused, uint8_t M);
  int16_t jumpRelative(uint8_t val);

  void transfer(uint8_t & src, uint8_t & dst) {
    dst = src;
    updateStatusZN(dst);
  }

  static int load(CPU * cpu, uint8_t & reg) {
    cpu->updateStatusZN(reg);
    return 0;
  }

  static int inc(CPU * cpu, uint8_t & reg) {
    reg = reg + 1;
    cpu->updateStatusZN(reg);
    return 0;
  }

  static int dec(CPU * cpu, uint8_t & reg) {
    reg = reg - 1;
    cpu->updateStatusZN(reg);
    return 0;
  }

  static int na(CPU * cpu, uint8_t & reg) {
    printf("Not implemented\n");
    exit(0);
    return 0;
  }

  // https://www.masswerk.at/6502/6502_instruction_set.html
  std::vector<Opcode> Opcodes {
    {PHP,   "PHP", Implied,   na},
    {ORAI,  "ORA", Immediate, na},

    {BPL,   "BPL", Relative,  na},
    {CLC,   "CLC", Implied,   na},

    {JSR,   "JSR", Absolute,  na},
    {BITZP, "BIT", ZeroPage,  na},
    {PLP,   "PLP", Implied,   na},
    {ANDI,  "AND", Immediate, na},
    {BITA,  "BIT", Absolute,  na},

    {BMI,   "BMI", Relative,  na},
    {SEC,   "SEC", Implied,   na},

    {PHA,   "PHA", Implied,   na},
    {EORI,  "EOR", Immediate, na},
    {LSR,   "LSR", Implied,   na},
    {JMPA,  "JMP", Absolute,  na},

    {RTS,   "RTS", Implied,   na},
    {ADCZP, "ADC", ZeroPage,  na},
    {PLA,   "PLA", Implied,   na},
    {ADCI,  "ADC", Immediate, na},
    {JMPI,  "JMP", Indirect,  na},
    {ADCA,  "ADC", Absolute,  na},

    {ADCZX, "ADC", ZeroPageX, na},
    {ADCAY, "ADC", AbsoluteY, na},
    {ADCAX, "ADC", AbsoluteX, na},

    {STYZP, "STY", ZeroPage,  na},
    {STAZP, "STA", ZeroPage,  na},
    {STXZP, "STX", ZeroPage,  na},
    {DEY,   "DEY", Implied,   dec},
    {TXA,   "TXA", Implied,   na},
    {STYA,  "STY", Absolute,  na},
    {STXA,  "STX", Absolute,  na},

    {BCC,   "BCC", Relative,  na},
    {STAIDIX, "STA", IndirectIndexed,  na},
    {STYZX, "STY", ZeroPageX, na},
    {STXZY, "STX", ZeroPageY, na},
    {TYA,   "TYA", Implied,   na},
    {STAAY, "STA", AbsoluteY, na},
    {TXS,   "TXS", Implied,   na},
    {STAAX, "STA", AbsoluteX, na},

    {LDYI,  "LDY", Immediate, load},
    {LDAIXID, "LDA", IndexedIndirect, load},
    {LDXI,  "LDX", Immediate, load},
    {LDYZP, "LDY", ZeroPage,  load},
    {LDAZP, "LDA", ZeroPage,  load},
    {LDXZP, "LDX", ZeroPage,  load},
    {TAY,   "TAY", Implied,   na},
    {LDAI,  "LDA", Immediate, load},
    {TAX,   "TAX", Implied,   na},
    {LDYA,  "LDY", Absolute,  load},
    {LDAA,  "LDA", Absolute,  load},
    {LDXA,  "LDX", Absolute,  load},

    {BCS,   "BCS", Relative,  load},
    {LDAIDIX, "LDA", IndirectIndexed, load},
    {LDYZX, "LDY", ZeroPageX, load},
    {LDAZX, "LDA", ZeroPageX, load},
    {LDXZY, "LDX", ZeroPageY, load},
    {LDAAY, "LDA", AbsoluteY, load},
    {TSX,   "TSX", Implied,   na},
    {LDYAX, "LDY", AbsoluteX, load},
    {LDAAX, "LDA", AbsoluteX, load},
    {LDXAY, "LDX", AbsoluteY, load},

    {CPYI,  "CPY", Immediate, na},
    {CPYZP, "CPY", ZeroPage,  na},
    {CMPZP, "CMP", ZeroPage,  na},
    {INY,   "INY", Implied,   inc},
    {CMPI,  "CMP", Immediate, na},
    {DEX,   "DEX", Implied,   dec},
    {CPYA,  "CPY", Absolute,  na},
    {CMPA,  "CMP", Absolute,  na},

    {BNE,   "BNE", Relative,  na},
    {CLD,   "CLD", Implied,   na},

    {CPXI,  "CPX", Immediate, na},
    {CPXZP, "CPX", ZeroPage,  na},
    {SBCZP, "SBC", ZeroPage,  na},
    {INX,   "INX", Implied,   inc},
    {SBCI,  "SBC", Immediate, na},
    {SBCA,  "SBC", Absolute,  na},
    {INCA,  "INC", Absolute,  na},
    {NOP,   "NOP", Implied,   na},
    {CPXA,  "CPX", Absolute,  na},

    {BEQ,   "BEQ", Relative,  na},
    {SBCZX, "SBC", ZeroPageX, na},
    {SED,   "SED", Implied,   na},
    {SBCAY, "SBC", AbsoluteY, na},
    {SBCAX, "SBC", AbsoluteX, na}
  };

};
