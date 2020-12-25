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

class SBCTest: public TestBase {
protected:
  void assertRegCOZN(uint8_t & reg, uint8_t res, uint8_t C, uint8_t O, uint8_t Z, uint8_t N) {
    ASSERT_EQ(reg, res);
    ASSERT_EQ(cpu->Status.bits.C, C);
    ASSERT_EQ(cpu->Status.bits.O, O);
    ASSERT_EQ(cpu->Status.bits.Z, Z);
    ASSERT_EQ(cpu->Status.bits.N, N);
  }
};


TEST_F(SBCTest, SBCImmediate) {
  //cpu->debugOn();
  cpu->A = 0xFF;
  exec1opcmd(SEC);
  exec2opcmd(SBCI, 0xFF);
  ASSERT_EQ(cpu->A, 0);
  ASSERT_EQ(cpu->Status.bits.N, 0);
  ASSERT_EQ(cpu->Status.bits.Z, 1);

  for (int i = 0xFE; i > 0x7F; i--) {
    cpu->A = 0xFF;
    exec1opcmd(SEC);
    exec2opcmd(SBCI, i);
    ASSERT_EQ(cpu->A, 0xFF - i);
    ASSERT_EQ(cpu->Status.bits.N, 0);
  }

  for (int i = 0x7F; i >= 0x00; i--) {
    cpu->A = 0xFF;
    exec1opcmd(SEC);
    exec2opcmd(SBCI, i);
    ASSERT_EQ(cpu->A, 0xFF - i);
    ASSERT_EQ(cpu->Status.bits.N, 1);
  }
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
