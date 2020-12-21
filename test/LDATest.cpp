// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Unit tests for 6502 simulator.
///
//===----------------------------------------------------------------------===//

#include <TestBase.h>
#include <Memory.h>
#include <CPU.h>
#include <Opcodes.h>

class LDATest: public TestBase {
protected:
  void assertRegZN(uint8_t & reg, uint8_t val, uint8_t Z, uint8_t N) {
    ASSERT_EQ(reg, val);
    ASSERT_EQ(cpu->Status.bits.Z, Z);
    ASSERT_EQ(cpu->Status.bits.N, N);
  }
};


TEST_F(LDATest, Constructor) {
  ASSERT_EQ(mem.readByte(45), 45);
  ASSERT_EQ(mem.readByte(0x20FF), 0xFF);
}


// Test that popping from top of stack wraps to bottom
TEST_F(LDATest, Immediate) {
  cpu->debugOn();
  exec2opcmd(LDAI, 0x00);
  assertRegZN(cpu->A, 0x00, 1, 0);

  exec2opcmd(LDAI, 0x42);
  assertRegZN(cpu->A, 0x42, 0, 0);

  exec2opcmd(LDAI, 0x7F);
  assertRegZN(cpu->A, 0x7F, 0, 0);

  exec2opcmd(LDAI, 0x80);
  assertRegZN(cpu->A, 0x80, 0, 1);

  exec2opcmd(LDAI, 0x81);
  assertRegZN(cpu->A, 0x81, 0, 1);

  exec2opcmd(LDAI, 0xFF);
  assertRegZN(cpu->A, 0xFF, 0, 1);
}

// Uses the fact that we initialize zeropage with its address
TEST_F(LDATest, ZeroPage) {
  cpu->debugOn();
  exec2opcmd(LDAZP, 0x00);
  assertRegZN(cpu->A, 0x00, 1, 0);

  exec2opcmd(LDAZP, 0x42);
  assertRegZN(cpu->A, 0x42, 0, 0);

  exec2opcmd(LDAZP, 0x7F);
  assertRegZN(cpu->A, 0x7F, 0, 0);

  exec2opcmd(LDAZP, 0x80);
  assertRegZN(cpu->A, 0x80, 0, 1);

  exec2opcmd(LDAZP, 0x81);
  assertRegZN(cpu->A, 0x81, 0, 1);

  exec2opcmd(LDAZP, 0xFF);
  assertRegZN(cpu->A, 0xFF, 0, 1);
}



int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
