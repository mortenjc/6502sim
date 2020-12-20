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
#include <Opcodes.h>
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
  int subcarry(uint8_t & unused, uint8_t M);

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
    {ORAI,  "ORA", Immediate, na},
    {BPL,   "BPL", Relative,  na},
    {CLC,   "CLC", Implicit,  na},
    {JSR,   "JSR", Absolute,  na},
    {BITZP, "BIT", ZeroPage,  na},
    {ANDI,  "AND", Immediate, na},
    {BITA,  "BIT", Absolute,  na},
    {BMI,   "BMI", Relative,  na},
    {SEC,   "SEC", Implicit,  na},
    {EORI,  "EOR", Immediate, na},
    {LSR,   "LSR", Implicit,  na},
    {JMPA,  "JMP", Absolute,  na},
    {RTS,   "RTS", Implicit,  na},
    {ADCZP, "ADC", ZeroPage,  na},
    {ADCI,  "ADC", Immediate, na},
    {ADCA,  "ADC", Absolute,  na},
    {ADCZX, "ADC", ZeroPageX, na},
    {ADCAY, "ADC", AbsoluteY, na},
    {ADCAX, "ADC", AbsoluteX, na},
    {STYZP, "STY", ZeroPage,  na},
    {STAZP, "STA", ZeroPage,  na},
    {STXZP, "STX", ZeroPage,  na},
    {DEY,   "DEY", Implicit,  dec},
    {TXA,   "TXA", Implicit,  na},
    {STYA,  "STY", Absolute,  na},
    {STXA,  "STX", Absolute,  na},
    {BCC,   "BCC", Relative,  na},
    {STYZX, "STY", ZeroPageX, na},
    {STXZY, "STX", ZeroPageY, na},
    {TYA,   "TYA", Implicit,  na},
    {STAAY, "STA", AbsoluteY, na},
    {STAAX, "STA", AbsoluteX, na},
    {LDYI,  "LDY", Immediate, load},
    {LDXI,  "LDX", Immediate, load},
    {LDYZP, "LDY", ZeroPage,  load},
    {LDAZP, "LDA", ZeroPage,  load},
    {LDXZP, "LDX", ZeroPage,  load},
    {TAY,   "TAY", Implicit,  na},
    {LDAI,  "LDA", Immediate, load},
    {TAX,   "TAX", Implicit,  na},
    {LDYA,  "LDY", Absolute,  load},
    {LDAA,  "LDA", Absolute,  load},
    {LDXA,  "LDX", Absolute,  load},
    {BCS,   "BCS", Relative,  load},
    {LDAZX, "LDA", ZeroPageX, load},
    {LDXZY, "LDX", ZeroPageY, load},
    {LDAAY, "LDA", AbsoluteY, load},
    {LDYAX, "LDY", AbsoluteX, load},
    {LDAAX, "LDA", AbsoluteX, load},
    {LDXAY, "LDX", AbsoluteY, load},
    {CPYI,  "CPY", Immediate, na},
    {CPYZP, "CPY", ZeroPage,  na},
    {CMPZP, "CMP", ZeroPage,  na},
    {INY,   "INY", Implicit,  inc},
    {CMPI,  "CMP", Immediate, na},
    {DEX,   "DEX", Implicit,  dec},
    {CPYA,  "CPY", Absolute,  na},
    {CMPA,  "CMP", Absolute,  na},
    {BNE,   "BNE", Relative,  na},
    {CLD,   "CLD", Implicit,  na},
    {CPXI,  "CPX", Immediate, na},
    {CPXZP, "CPX", ZeroPage,  na},
    {SBCZP, "SBC", ZeroPage,  na},
    {INX,   "INX", Implicit,  inc},
    {SBCI,  "SBC", Immediate, na},
    {SBCA,  "SBC", Absolute,  na},
    {INCA,  "INC", Absolute,  na},
    {NOP,   "NOP", Implicit,  na},
    {CPXA,  "CPX", Absolute,  na},
    {BEQ,   "BEQ", Relative,  na},
    {SBCZX, "SBC", ZeroPageX, na},
    {SED,   "SED", Implicit,  na},
    {SBCAY, "SBC", AbsoluteY, na},
    {SBCAX, "SBC", AbsoluteX, na}
  };

};
