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

class ADCTest: public TestBase {
protected:
  void assertRegCOZN(uint8_t & reg, uint8_t res, uint8_t C, uint8_t O, uint8_t Z, uint8_t N) {
    ASSERT_EQ(reg, res);
    ASSERT_EQ(cpu->Status.bits.C, C);
    ASSERT_EQ(cpu->Status.bits.O, O);
    ASSERT_EQ(cpu->Status.bits.Z, Z);
    ASSERT_EQ(cpu->Status.bits.N, N);
  }
};


#define ADC2(opcode, reg, rval, val, res, C, O, Z, N)\
   reg = rval; \
   exec2opcmd(opcode, val);\
   assertRegCOZN(reg, res, C, O, Z, N);

// #define LDA3(opcode, reg, addr, res, Z, N)\
//   exec3opcmd(opcode, addr & 0xff, addr >> 8);\
//   assertRegZN(reg, res, Z, N);


// From: http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
TEST_F(ADCTest, ADCOverflow) {
  //cpu->debugOn();
  ADC2(ADCI, cpu->A, 0x3F, 0x40, 0x7F, 0, 0, 0, 0);
  ADC2(ADCI, cpu->A, 0x40, 0x40, 0x80, 0, 1, 0, 1);

  ADC2(ADCI, cpu->A, 0xD0, 0x90, 0x60, 1, 1, 0, 0);
}


TEST_F(ADCTest, ADCImmediate) {
  //cpu->debugOn();
  //                 ival  add   res   C  O  Z  N
  ADC2(ADCI, cpu->A, 0x00, 0x00, 0x00, 0, 0, 1, 0);
  cpu->Status.bits.C = 1;
  ADC2(ADCI, cpu->A, 0x00, 0x00, 0x01, 0, 0, 0, 0);
  ADC2(ADCI, cpu->A, 0xFE, 0x01, 0xFF, 0, 0, 0, 1);
  ADC2(ADCI, cpu->A, 0xFF, 0x01, 0x00, 1, 0, 1, 0);
  cpu->Status.bits.C = 1;
  ADC2(ADCI, cpu->A, 0xFD, 0x02, 0x00, 1, 0, 1, 0);
}

TEST_F(ADCTest, ADCZeroPage) {
  //cpu->debugOn();
  //                 ival  add   res   C  O  Z  N
  ADC2(ADCZP, cpu->A, 0x00, 0x00, 0x00, 0, 0, 1, 0);
  cpu->Status.bits.C = 1;
  ADC2(ADCZP, cpu->A, 0x00, 0x00, 0x01, 0, 0, 0, 0);
  ADC2(ADCZP, cpu->A, 0xFE, 0x01, 0xFF, 0, 0, 0, 1);
  ADC2(ADCZP, cpu->A, 0xFF, 0x01, 0x00, 1, 0, 1, 0);
  cpu->Status.bits.C = 1;
  ADC2(ADCZP, cpu->A, 0xFD, 0x02, 0x00, 1, 0, 1, 0);
}

TEST_F(ADCTest, ADCZeroPageX) {
  //cpu->debugOn();
  cpu->X = 1;
  //                  ival  add   res   C  O  Z  N
  ADC2(ADCZX, cpu->A, 0x00, 0x00, 0x01, 0, 0, 0, 0);
  cpu->Status.bits.C = 1;
  ADC2(ADCZX, cpu->A, 0x00, 0x00, 0x02, 0, 0, 0, 0);

  ADC2(ADCZX, cpu->A, 0xFD, 0x01, 0xFF, 0, 0, 0, 1);
  cpu->Status.bits.C = 0;
  ADC2(ADCZX, cpu->A, 0xFE, 0x01, 0x00, 1, 0, 1, 0);
  cpu->Status.bits.C = 0;
  ADC2(ADCZX, cpu->A, 0xFF, 0x01, 0x01, 1, 0, 0, 0);
  cpu->Status.bits.C = 1;
  ADC2(ADCZX, cpu->A, 0xFD, 0x02, 0x01, 1, 0, 0, 0);
}


TEST_F(ADCTest, AddCarry) {
  //cpu->debugOn();
  exec2opcmd(LDAI,  0);
  exec2opcmd(ADCI,  0);
  ASSERT_EQ(cpu->A, 0);
  ASSERT_EQ(cpu->Status.bits.C, 0);
  ASSERT_EQ(cpu->Status.bits.N, 0);
  ASSERT_EQ(cpu->Status.bits.Z, 1);

  for (int i = 1; i < 0x80; i++) {
    exec2opcmd(LDAI,  0);
    exec2opcmd(ADCI,  i);
    ASSERT_EQ(cpu->A, i);
    ASSERT_EQ(cpu->Status.bits.C, 0);
    ASSERT_EQ(cpu->Status.bits.N, 0);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
  }

  for (int i = 0x80; i < 0xFF; i++) {
    exec2opcmd(LDAI,  0);
    exec2opcmd(ADCI,  i);
    ASSERT_EQ(cpu->A, i);
    ASSERT_EQ(cpu->Status.bits.C, 0);
    ASSERT_EQ(cpu->Status.bits.N, 1);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
  }
}

TEST_F(ADCTest, AddCarryCarrySet) {
  //cpu->debugOn();
  exec1opcmd(CLC);
  exec2opcmd(LDAI,  255);
  exec2opcmd(ADCI,  0);
  ASSERT_EQ(cpu->A, 255);
  ASSERT_EQ(cpu->Status.bits.C, 0);
  ASSERT_EQ(cpu->Status.bits.N, 1);
  ASSERT_EQ(cpu->Status.bits.Z, 0);

  exec1opcmd(CLC);
  exec2opcmd(LDAI,  255);
  exec2opcmd(ADCI,  1);
  ASSERT_EQ(cpu->A, 0);
  ASSERT_EQ(cpu->Status.bits.C, 1);
  ASSERT_EQ(cpu->Status.bits.N, 0);
  ASSERT_EQ(cpu->Status.bits.Z, 1);

  for (int i = 2; i < 0x81; i++) {
    exec1opcmd(CLC);
    exec2opcmd(LDAI,  255);
    exec2opcmd(ADCI,  i);
    ASSERT_EQ(cpu->A, i - 1);
    ASSERT_EQ(cpu->Status.bits.C, 1);
    ASSERT_EQ(cpu->Status.bits.N, 0);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
  }

  for (int i = 0x81; i < 0xFF; i++) {
    exec1opcmd(CLC);
    exec2opcmd(LDAI,  255);
    exec2opcmd(ADCI,  i);
    ASSERT_EQ(cpu->A, i - 1);
    ASSERT_EQ(cpu->Status.bits.C, 1);
    ASSERT_EQ(cpu->Status.bits.N, 1);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
  }
}

TEST_F(ADCTest, AddCarryDecimalMode) {
    exec1opcmd(SED);
    exec2opcmd(LDAI, 0x00);
    exec2opcmd(ADCI, 0x01);
    ASSERT_EQ(cpu->A, 0x01);
    ASSERT_EQ(cpu->Status.bits.C, 0);
    exec2opcmd(ADCI, 0x01);
    ASSERT_EQ(cpu->A, 0x02);
    ASSERT_EQ(cpu->Status.bits.C, 0);

    exec2opcmd(LDAI, 0x09);
    exec2opcmd(ADCI, 0x01);
    ASSERT_EQ(cpu->A, 0x10);
    ASSERT_EQ(cpu->Status.bits.C, 0);


    exec2opcmd(LDAI, 0x98);
    exec2opcmd(ADCI, 0x01);
    ASSERT_EQ(cpu->A, 0x99);
    ASSERT_EQ(cpu->Status.bits.C, 0);
    exec2opcmd(ADCI, 0x01); // wrap from 0x99 to 0x00
    ASSERT_EQ(cpu->A, 0x00);
    ASSERT_EQ(cpu->Status.bits.C, 1);
}

TEST_F(ADCTest, SubCarryDecimalMode) {
    exec1opcmd(SED);
    exec1opcmd(SEC);
    exec2opcmd(LDAI, 0x99);
    exec2opcmd(SBCI, 0x01);
    ASSERT_EQ(cpu->A, 0x98);
    ASSERT_EQ(cpu->Status.bits.C, 1);
    exec2opcmd(SBCI, 0x01);
    ASSERT_EQ(cpu->A, 0x97);
    ASSERT_EQ(cpu->Status.bits.C, 1);

    exec2opcmd(LDAI, 0x01);
    exec2opcmd(SBCI, 0x01);
    ASSERT_EQ(cpu->A, 0x00);
    ASSERT_EQ(cpu->Status.bits.C, 1);
    exec2opcmd(SBCI, 0x01); // wrap from 0x00 to 0x99
    ASSERT_EQ(cpu->A, 0x99);
    ASSERT_EQ(cpu->Status.bits.C, 0);
}

TEST_F(ADCTest, AddCarryDecimalModeFTFail) {
  cpu->debugOn();
    exec1opcmd(SED);
    exec1opcmd(CLC);
    exec2opcmd(LDAI, 0x90);
    exec2opcmd(ADCI, 0x99);
    ASSERT_EQ(cpu->A, 0x90);
    ASSERT_EQ(cpu->Status.bits.Z, 1);
  }

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
