// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Unit tests for 6502 simulator.
///
//===----------------------------------------------------------------------===//

#include <TestBase.h>

class CPUTest: public TestBase { };

// Test that popping from top of stack wraps to bottom
TEST_F(CPUTest, ReturnSubroutine) {
  cpu->debugOn();
  ASSERT_EQ(cpu->getSPAddr(), 0x1FF);
  exec1opcmd(RTS);
  ASSERT_EQ(cpu->getSPAddr(), 0x101);
}

// Test thah pushing to full stack wraps to top
TEST_F(CPUTest, JumpSubroutineStackFull) {
  cpu->debugOn();
  mem.writeByte(0x2000, RTS);
  mem.writeByte(0x1003, NOP);
  cpu->S = 1;
  ASSERT_EQ(cpu->getSPAddr(), 0x101);
  exec3opcmd(JSR, 0x00, 0x20);
  ASSERT_EQ(cpu->getSPAddr(), 0x1FF);
  ASSERT_EQ(cpu->PC, 0x2000);
}

TEST_F(CPUTest, JumpSubroutine) {
  cpu->debugOn();
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


TEST_F(CPUTest, JumpAbsandInd) {
  cpu->debugOn();
  exec3opcmd(JMPA, 0x00, 0x20);
  ASSERT_EQ(cpu->getSPAddr(), 0x1FF);
  ASSERT_EQ(cpu->PC, 0x2000);

  mem.writeByte(0x2000, 0x00);
  mem.writeByte(0x2001, 0x30); // 0x3000 (for JMPI)
  exec3opcmd(JMPI, 0x00, 0x20);
  ASSERT_EQ(0x00, mem.readByte(0x2000));
  ASSERT_EQ(0x30, mem.readByte(0x2001));
  ASSERT_EQ(cpu->getSPAddr(), 0x1FF);
  ASSERT_EQ(cpu->PC, 0x3000);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
