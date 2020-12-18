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
  enum AMode { None, Immediate, ZeroPage, Absolute, AbsoluteX};

  const uint16_t stack_pointer_addr{0x01FF};
  const uint16_t power_on_reset_addr{0xFFFC};

  bool running{true};
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


  CPU(Memory & memory) : mem(memory) {};

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

  // print out registers and flags
  void debug();
  void debug(std::string opcode, uint8_t val, uint16_t addr, AMode mode);

private:
  // Handles CMPI, CPXI, CPYI, ZP, Absolute
  int compare(std::string opcode, uint8_t & reg, AMode mode);

  int load(std::string opcode, uint8_t & reg, AMode mode);

  int addcarry(std::string opcode, uint8_t & reg, AMode mode);

  int branch(std::string opcode, uint8_t flag, int8_t onoff);

};
