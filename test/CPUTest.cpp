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

   // Helper for load instructions
  void load(uint8_t opcode, uint8_t value, uint8_t & reg) {
    mem.writeByte(0x1000, opcode);
    mem.writeByte(0x1001, value);
    cpu->PC = 0x1000;
    uint8_t inst = cpu->getInstruction();
    ASSERT_EQ(inst, opcode);
    cpu->handleInstruction(inst);
    ASSERT_EQ(value, reg);
  }


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




TEST_F(CPUTest, LOADI) {
  load(LDXI, 0x42, cpu->X);
  ASSERT_EQ(cpu->Status.bits.Z, 0);
  ASSERT_EQ(cpu->Status.bits.N, 0);

  load(LDXI, 0x00, cpu->X);
  ASSERT_EQ(cpu->Status.bits.Z, 1);
  ASSERT_EQ(cpu->Status.bits.N, 0);

  load(LDXI, 0x80, cpu->X);
  ASSERT_EQ(cpu->Status.bits.Z, 0);
  ASSERT_EQ(cpu->Status.bits.N, 1);

  load(LDYI, 0x42, cpu->Y);
  ASSERT_EQ(cpu->Status.bits.Z, 0);
  ASSERT_EQ(cpu->Status.bits.N, 0);

  load(LDYI, 0x00, cpu->Y);
  ASSERT_EQ(cpu->Status.bits.Z, 1);
  ASSERT_EQ(cpu->Status.bits.N, 0);

  load(LDYI, 0x80, cpu->Y);
  ASSERT_EQ(cpu->Status.bits.Z, 0);
  ASSERT_EQ(cpu->Status.bits.N, 1);
}


TEST_F(CPUTest, LOADZP) {
  load(LDAZP, 0, cpu->A);
  ASSERT_EQ(cpu->Status.bits.Z, 1);
  ASSERT_EQ(cpu->Status.bits.N, 0);

  load(LDXZP, 0, cpu->X);
  ASSERT_EQ(cpu->Status.bits.Z, 1);
  ASSERT_EQ(cpu->Status.bits.N, 0);

  load(LDYZP, 0, cpu->Y);
  ASSERT_EQ(cpu->Status.bits.Z, 1);
  ASSERT_EQ(cpu->Status.bits.N, 0);

  for (int i = 1; i < 0x80; i++) {
    load(LDAZP, i, cpu->A);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
    ASSERT_EQ(cpu->Status.bits.N, 0);

    load(LDXZP, i, cpu->X);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
    ASSERT_EQ(cpu->Status.bits.N, 0);

    load(LDYZP, i, cpu->Y);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
    ASSERT_EQ(cpu->Status.bits.N, 0);
  }

  for (int i = 0x81; i < 0x100; i++) {
    load(LDAZP, i, cpu->A);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
    ASSERT_EQ(cpu->Status.bits.N, 1);

    load(LDXZP, i, cpu->X);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
    ASSERT_EQ(cpu->Status.bits.N, 1);

    load(LDYZP, i, cpu->Y);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
    ASSERT_EQ(cpu->Status.bits.N, 1);
  }
}

TEST_F(CPUTest, LoadAbsoluteX) {
  exec2opcmd(LDXI,  0);
  exec3opcmd(LDAAX, 00, 0x20);
  ASSERT_EQ(0, cpu->A);
  ASSERT_EQ(cpu->Status.bits.N, 0);
  ASSERT_EQ(cpu->Status.bits.Z, 1);

  for (int i = 1; i < 0x80; i++) {
    exec2opcmd(LDXI,  i);
    exec3opcmd(LDAAX, 00, 0x20);
    ASSERT_EQ(i, cpu->A);
    ASSERT_EQ(cpu->Status.bits.N, 0);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
  }

  for (int i = 0x80; i < 0x100; i++) {
    exec2opcmd(LDXI,  i);
    exec3opcmd(LDAAX, 00, 0x20);
    ASSERT_EQ(i, cpu->A);
    ASSERT_EQ(cpu->Status.bits.N, 1);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
  }
}

TEST_F(CPUTest, LoadAbsoluteY) {
  exec2opcmd(LDYI,  0);
  exec3opcmd(LDAAY, 00, 0x20);
  ASSERT_EQ(0, cpu->A);
  ASSERT_EQ(cpu->Status.bits.N, 0);
  ASSERT_EQ(cpu->Status.bits.Z, 1);

  for (int i = 1; i < 0x80; i++) {
    exec2opcmd(LDYI,  i);
    exec3opcmd(LDAAY, 00, 0x20);
    ASSERT_EQ(i, cpu->A);
    ASSERT_EQ(cpu->Status.bits.N, 0);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
  }

  for (int i = 0x80; i < 0x100; i++) {
    exec2opcmd(LDYI,  i);
    exec3opcmd(LDAAY, 00, 0x20);
    ASSERT_EQ(i, cpu->A);
    ASSERT_EQ(cpu->Status.bits.N, 1);
    ASSERT_EQ(cpu->Status.bits.Z, 0);
  }
}

TEST_F(CPUTest, AddCarry) {
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

TEST_F(CPUTest, AddCarryCarrySet) {
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

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
