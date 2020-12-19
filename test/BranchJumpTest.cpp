// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Unit tests for 6502 simulator.
///
//===----------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <Memory.h>
#include <CPU.h>
#include <Opcodes.h>

class CPUTest: public ::testing::Test {
protected:
  Memory mem;
  CPU * cpu;

  void exec1opcmd(uint8_t opcode) {
    mem.writeByte(0x1000, opcode);
    cpu->PC = 0x1000;
    uint8_t inst = cpu->getInstruction();
    ASSERT_EQ(inst, opcode);
    bool running = cpu->handleInstruction(inst);
    ASSERT_TRUE(running);
  }

  void exec2opcmd(uint8_t opcode, uint8_t val) {
    mem.writeByte(0x1000, opcode);
    mem.writeByte(0x1001, val);
    cpu->PC = 0x1000;
    uint8_t inst = cpu->getInstruction();
    ASSERT_EQ(inst, opcode);
    bool running = cpu->handleInstruction(inst);
    ASSERT_TRUE(running);
  }

  void exec3opcmd(uint8_t opcode, uint8_t lo, uint8_t hi) {
    mem.writeByte(0x1000, opcode);
    mem.writeByte(0x1001, lo);
    mem.writeByte(0x1002, hi);
    cpu->PC = 0x1000;
    uint8_t inst = cpu->getInstruction();
    ASSERT_EQ(inst, opcode);
    bool running = cpu->handleInstruction(inst);
    ASSERT_TRUE(running);
  }


  void SetUp( ) {
    mem.reset();
    for (int i = 0; i < 256; i++) {
     mem.writeByte(i, i);         // ZP  : 0, 1, 2, 3, 4
     mem.writeByte(0x2000 + i, i);  // 2000: 0, 1, 2, 3, 4
    }
    cpu = new CPU(mem);
    cpu->reset();
  }

  void TearDown( ) { }
};


// Test that popping from top of stack wraps to bottom
TEST_F(CPUTest, ReturnSubroutine) {
  ASSERT_EQ(cpu->getSPAddr(), 0x1FF);
  exec1opcmd(RTS);
  ASSERT_EQ(cpu->getSPAddr(), 0x101);
}

// Test thah pushing to full stack wraps to top
TEST_F(CPUTest, JumpSubroutineStackFull) {
  mem.writeByte(0x2000, RTS);
  mem.writeByte(0x1003, NOP);
  cpu->S = 1;
  ASSERT_EQ(cpu->getSPAddr(), 0x101);
  exec3opcmd(JSR, 0x00, 0x20);
  ASSERT_EQ(cpu->getSPAddr(), 0x1FF);
  ASSERT_EQ(cpu->PC, 0x2000);
}

TEST_F(CPUTest, JumpSubroutine) {
  mem.writeByte(0x2000, RTS);
  mem.writeByte(0x1003, NOP);
  ASSERT_EQ(cpu->getSPAddr(), 0x1FF);
  exec3opcmd(JSR, 0x00, 0x20);
  ASSERT_EQ(cpu->getSPAddr(), 0x1FD);
  ASSERT_EQ(cpu->PC, 0x2000);

  uint8_t inst = cpu->getInstruction();
  ASSERT_EQ(inst, RTS);
  cpu->handleInstruction(inst);
  ASSERT_EQ(cpu->getSPAddr(), 0x1FF);
  ASSERT_EQ(cpu->PC, 0x1003);

  inst = cpu->getInstruction();
  ASSERT_EQ(inst, NOP);
  cpu->handleInstruction(inst);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
