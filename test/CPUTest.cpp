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

class CPUTest: public TestBase {};


TEST_F(CPUTest, Flags) {
  cpu->Status.mask = 0;
  cpu->Status.bits.C = 1;
  ASSERT_EQ(cpu->Status.mask, 1);
  cpu->Status.bits.Z = 1;
  ASSERT_EQ(cpu->Status.mask, 1 + 2);
  cpu->Status.bits.I = 1;
  ASSERT_EQ(cpu->Status.mask, 1 + 2 + 4);
  cpu->Status.bits.D = 1;
  ASSERT_EQ(cpu->Status.mask, 1 + 2 + 4 + 8 );
  cpu->Status.bits.B = 1;
  ASSERT_EQ(cpu->Status.mask, 1 + 2 + 4 + 8 + 16);
  cpu->Status.bits.O = 1;
  ASSERT_EQ(cpu->Status.mask, 1 + 2 + 4 + 8 + 16 + 64);
  cpu->Status.bits.N = 1;
  ASSERT_EQ(cpu->Status.mask, 1 + 2 + 4 + 8 + 16 + 64 + 128);
}

TEST_F(CPUTest, FlagsAO) {
  cpu->Status.mask = 0;
  cpu->Status.bits.B = 1;
  cpu->Status.bits.r = 1;
  ASSERT_EQ(cpu->Status.mask, 0x30);
}

TEST_F(CPUTest, IncX) {
  mem.writeByte(0x1000, INX);

  for (uint16_t i = 1; i < 256; i++) {
    cpu->PC = 0x1000;
    uint8_t inst = cpu->getInstruction();
    ASSERT_EQ(inst, INX);
    cpu->handleInstruction(inst);
    ASSERT_EQ(i, cpu->X);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
    if (cpu->X & 0x80) {
      ASSERT_EQ(cpu->Status.bits.N, 1);
    } else {
      ASSERT_EQ(cpu->Status.bits.N, 0);
    }
  }

  // Handle wrap to zero
  cpu->PC = 0x1000;
  uint8_t inst = cpu->getInstruction();
  ASSERT_EQ(inst, INX);
  cpu->handleInstruction(inst);
  ASSERT_EQ(0, cpu->X);
  ASSERT_EQ(cpu->Status.bits.Z, 1);
}


TEST_F(CPUTest, IncY) {
  mem.writeByte(0x1000, INY);
  for (uint16_t i = 1; i < 256; i++) {
    cpu->PC = 0x1000;
    uint8_t inst = cpu->getInstruction();
    ASSERT_EQ(inst, INY);
    cpu->handleInstruction(inst);
    ASSERT_EQ(i, cpu->Y);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
    if (cpu->Y & 0x80) {
      ASSERT_EQ(cpu->Status.bits.N, 1);
    } else {
      ASSERT_EQ(cpu->Status.bits.N, 0);
    }
  }

  // Handle wrap to zero
  cpu->PC = 0x1000;
  uint8_t inst = cpu->getInstruction();
  ASSERT_EQ(inst, INY);
  cpu->handleInstruction(inst);
  ASSERT_EQ(0, cpu->Y);
  ASSERT_EQ(cpu->Status.bits.Z, 1);
}



TEST_F(CPUTest, ClearSetCarry) {
  exec1opcmd(SEC);
  ASSERT_EQ(cpu->Status.bits.C, 1);
  exec1opcmd(CLC);
  ASSERT_EQ(cpu->Status.bits.C, 0);
}

TEST_F(CPUTest, AndOrXorBit) {
  // AND
  exec2opcmd(LDAI,  0xAA);
  exec2opcmd(ANDI,  0xFF);
  ASSERT_EQ(cpu->A, 0xAA);
  ASSERT_EQ(cpu->Status.bits.N, 1);
  ASSERT_EQ(cpu->Status.bits.Z, 0);

  exec2opcmd(LDAI,  0xAA);
  exec2opcmd(ANDI,  0xAA);
  ASSERT_EQ(cpu->A, 0xAA);
  ASSERT_EQ(cpu->Status.bits.N, 1);
  ASSERT_EQ(cpu->Status.bits.Z, 0);

  exec2opcmd(LDAI,  0xAA);
  exec2opcmd(ANDI,  0x55);
  ASSERT_EQ(cpu->A, 0x00);
  ASSERT_EQ(cpu->Status.bits.N, 0);
  ASSERT_EQ(cpu->Status.bits.Z, 1);

  // OR
  exec2opcmd(LDAI,  0xAA);
  exec2opcmd(ORAI,  0x55);
  ASSERT_EQ(cpu->A, 0xFF);
  ASSERT_EQ(cpu->Status.bits.N, 1);
  ASSERT_EQ(cpu->Status.bits.Z, 0);

  exec2opcmd(LDAI,  0xAA);
  exec2opcmd(ORAI,  0xFF);
  ASSERT_EQ(cpu->A, 0xFF);
  ASSERT_EQ(cpu->Status.bits.N, 1);
  ASSERT_EQ(cpu->Status.bits.Z, 0);

  exec2opcmd(LDAI,  0x00);
  exec2opcmd(ORAI,  0x00);
  ASSERT_EQ(cpu->A, 0x00);
  ASSERT_EQ(cpu->Status.bits.N, 0);
  ASSERT_EQ(cpu->Status.bits.Z, 1);

  // EOR

  exec2opcmd(LDAI,  0xAA);
  exec2opcmd(EORI,  0x55);
  ASSERT_EQ(cpu->A, 0xFF);
  ASSERT_EQ(cpu->Status.bits.N, 1);
  ASSERT_EQ(cpu->Status.bits.Z, 0);

  exec2opcmd(LDAI,  0x55);
  exec2opcmd(EORI,  0x55);
  ASSERT_EQ(cpu->A, 0x00);
  ASSERT_EQ(cpu->Status.bits.N, 0);
  ASSERT_EQ(cpu->Status.bits.Z, 1);

  ASSERT_EQ(0xFF, mem.readByte(0xFF));
  exec2opcmd(LDAI,  0xFF);
  exec2opcmd(BITZP, 0xFF);
  ASSERT_EQ(cpu->Status.bits.N, 1);
  ASSERT_EQ(cpu->Status.bits.O, 1);
  ASSERT_EQ(cpu->Status.bits.Z, 0);
}



TEST_F(CPUTest, CompareX) {
  exec2opcmd(LDXI,  0x01);
  exec2opcmd(CPXI,  0xFF);
  ASSERT_EQ(cpu->X, 0x01);
  ASSERT_EQ(cpu->Status.bits.C, 0);
  ASSERT_EQ(cpu->Status.bits.Z, 0);
  ASSERT_EQ(cpu->Status.bits.N, 0);

  exec2opcmd(LDXI,  0x80);
  exec2opcmd(CPXI,  0x80);
  ASSERT_EQ(cpu->X, 0x80);
  ASSERT_EQ(cpu->Status.bits.C, 1);
  ASSERT_EQ(cpu->Status.bits.Z, 1);
  ASSERT_EQ(cpu->Status.bits.N, 0);

  exec2opcmd(LDXI,  0x80);
  exec2opcmd(CPXI,  0x01);
  ASSERT_EQ(cpu->X, 0x80);
  ASSERT_EQ(cpu->Status.bits.C, 1);
  ASSERT_EQ(cpu->Status.bits.Z, 0);
  ASSERT_EQ(cpu->Status.bits.N, 0);


  exec2opcmd(LDXI,  0x81);
  exec2opcmd(CPXI,  0x01);
  ASSERT_EQ(cpu->X, 0x81);
  ASSERT_EQ(cpu->Status.bits.C, 1);
  ASSERT_EQ(cpu->Status.bits.Z, 0);
  ASSERT_EQ(cpu->Status.bits.N, 1);
}


TEST_F(CPUTest, ShiftsROL) {
  exec2opcmd(ROLZP, 0x00);
  ASSERT_EQ(0x00, mem.readByte(0x00));
  exec2opcmd(ROLZP, 0x01);
  ASSERT_EQ(0x02, mem.readByte(0x01));
  exec2opcmd(ROLZP, 0x20);
  ASSERT_EQ(0x40, mem.readByte(0x20));

  ASSERT_EQ(mem.readByte(0x80), 0x80);
  exec2opcmd(ROLZP, 0x80);
  ASSERT_EQ(0x00, mem.readByte(0x80));
  ASSERT_EQ(cpu->Status.bits.C, 1);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
