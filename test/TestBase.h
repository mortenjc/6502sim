// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Base class for unit tests
///
//===----------------------------------------------------------------------===//

#pragma once

#include <gtest/gtest.h>
#include <CPU.h>
#include <Memory.h>


class TestBase : public ::testing::Test {
protected:
  Memory mem;
  CPU * cpu;

  void SetUp( ) {
    mem.reset();
    for (int i = 0; i < 256; i++) {
     mem.writeByte(i, i);           // ZP  : 0, 1, 2, 3, 4
     mem.writeByte(256 + i, 0xAA);  // To detect ZP wrap problems
     mem.writeByte(0x2000 + i, i);  // 2000: 0, 1, 2, 3, 4
    }
    cpu = new CPU(mem);
    cpu->reset(0x0000);
  }

  void TearDown( ) { }

public:
  void exec(uint8_t opcode) {
    cpu->PC = 0x1000;
    uint8_t inst = cpu->getInstruction();
    ASSERT_EQ(inst, opcode);
    bool running = cpu->handleInstruction(inst);
    ASSERT_TRUE(running);
  }

  void exec1opcmd(uint8_t opcode) {
    mem.writeByte(0x1000, opcode);
    exec(opcode);
  }

  void exec2opcmd(uint8_t opcode, uint8_t val) {
    mem.writeByte(0x1000, opcode);
    mem.writeByte(0x1001, val);
    exec(opcode);
  }

  void exec3opcmd(uint8_t opcode, uint8_t lo, uint8_t hi) {
    mem.writeByte(0x1000, opcode);
    mem.writeByte(0x1001, lo);
    mem.writeByte(0x1002, hi);
    exec(opcode);
  }

};
