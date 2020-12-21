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


// Just check that stuff is initialized correctly
TEST_F(LDATest, Constructor) {
  ASSERT_EQ(cpu->Status.mask, 0);
  ASSERT_EQ(cpu->A, 0);
  ASSERT_EQ(cpu->X, 0);
  ASSERT_EQ(cpu->Y, 0);
  ASSERT_EQ(mem.readByte(45), 45);
  ASSERT_EQ(mem.readByte(0x20FF), 0xFF);
}


#define LDA2(opcode, reg, val, res, Z, N)\
   exec2opcmd(opcode, val);\
   assertRegZN(reg, res, Z, N);

#define LDA3(opcode, reg, addr, res, Z, N)\
  exec3opcmd(opcode, addr & 0xff, addr >> 8);\
  assertRegZN(reg, res, Z, N);


//
TEST_F(LDATest, LDAImmediate) {
  //cpu->debugOn();
  LDA2(LDAI, cpu->A, 0x00, 0x00, 1, 0);
  LDA2(LDAI, cpu->A, 0x42, 0x42, 0, 0);
  LDA2(LDAI, cpu->A, 0x7F, 0x7F, 0, 0);
  LDA2(LDAI, cpu->A, 0x80, 0x80, 0, 1);
  LDA2(LDAI, cpu->A, 0x81, 0x81, 0, 1);
  LDA2(LDAI, cpu->A, 0xFF, 0xFF, 0, 1);
}

TEST_F(LDATest, LDXImmediate) {
  //cpu->debugOn();
  LDA2(LDXI, cpu->X, 0x00, 0x00, 1, 0);
  LDA2(LDXI, cpu->X, 0x42, 0x42, 0, 0);
  LDA2(LDXI, cpu->X, 0x7F, 0x7F, 0, 0);
  LDA2(LDXI, cpu->X, 0x80, 0x80, 0, 1);
  LDA2(LDXI, cpu->X, 0x81, 0x81, 0, 1);
  LDA2(LDXI, cpu->X, 0xFF, 0xFF, 0, 1);
}

TEST_F(LDATest, LDYImmediate) {
  //cpu->debugOn();
  LDA2(LDYI, cpu->Y, 0x00, 0x00, 1, 0);
  LDA2(LDYI, cpu->Y, 0x42, 0x42, 0, 0);
  LDA2(LDYI, cpu->Y, 0x7F, 0x7F, 0, 0);
  LDA2(LDYI, cpu->Y, 0x80, 0x80, 0, 1);
  LDA2(LDYI, cpu->Y, 0x81, 0x81, 0, 1);
  LDA2(LDYI, cpu->Y, 0xFF, 0xFF, 0, 1);
}

//
TEST_F(LDATest, LDAZeroPage) {
  //cpu->debugOn();
  LDA2(LDAZP, cpu->A, 0x00, 0x00, 1, 0);
  LDA2(LDAZP, cpu->A, 0x42, 0x42, 0, 0);
  LDA2(LDAZP, cpu->A, 0x7F, 0x7F, 0, 0);
  LDA2(LDAZP, cpu->A, 0x80, 0x80, 0, 1);
  LDA2(LDAZP, cpu->A, 0x81, 0x81, 0, 1);
  LDA2(LDAZP, cpu->A, 0xFF, 0xFF, 0, 1);
}

TEST_F(LDATest, LDXZeroPage) {
  //cpu->debugOn();
  LDA2(LDXZP, cpu->X, 0x00, 0x00, 1, 0);
  LDA2(LDXZP, cpu->X, 0x42, 0x42, 0, 0);
  LDA2(LDXZP, cpu->X, 0x7F, 0x7F, 0, 0);
  LDA2(LDXZP, cpu->X, 0x80, 0x80, 0, 1);
  LDA2(LDXZP, cpu->X, 0x81, 0x81, 0, 1);
  LDA2(LDXZP, cpu->X, 0xFF, 0xFF, 0, 1);
}

TEST_F(LDATest, LDYZeroPage) {
  //cpu->debugOn();
  LDA2(LDYZP, cpu->Y, 0x00, 0x00, 1, 0);
  LDA2(LDYZP, cpu->Y, 0x42, 0x42, 0, 0);
  LDA2(LDYZP, cpu->Y, 0x7F, 0x7F, 0, 0);
  LDA2(LDYZP, cpu->Y, 0x80, 0x80, 0, 1);
  LDA2(LDYZP, cpu->Y, 0x81, 0x81, 0, 1);
  LDA2(LDYZP, cpu->Y, 0xFF, 0xFF, 0, 1);
}

TEST_F(LDATest, LDAZeroPageX) {
  //cpu->debugOn();
  cpu->X = 1;
  LDA2(LDAZX, cpu->A, 0x00, 0x01, 0, 0);
  LDA2(LDAZX, cpu->A, 0x42, 0x43, 0, 0);
  LDA2(LDAZX, cpu->A, 0x7E, 0x7F, 0, 0);
  LDA2(LDAZX, cpu->A, 0x7F, 0x80, 0, 1);
  LDA2(LDAZX, cpu->A, 0x80, 0x81, 0, 1);
  LDA2(LDAZX, cpu->A, 0xFE, 0xFF, 0, 1);
  LDA2(LDAZX, cpu->A, 0xFF, 0x00, 1, 0);
}

TEST_F(LDATest, LDXZeroPageY) {
  //cpu->debugOn();
  cpu->Y = 1;
  LDA2(LDXZY, cpu->X, 0x00, 0x01, 0, 0);
  LDA2(LDXZY, cpu->X, 0x42, 0x43, 0, 0);
  LDA2(LDXZY, cpu->X, 0x7E, 0x7F, 0, 0);
  LDA2(LDXZY, cpu->X, 0x7F, 0x80, 0, 1);
  LDA2(LDXZY, cpu->X, 0x80, 0x81, 0, 1);
  LDA2(LDXZY, cpu->X, 0xFE, 0xFF, 0, 1);
  LDA2(LDXZY, cpu->X, 0xFF, 0x00, 1, 0);
}

TEST_F(LDATest, LDYZeroPageX) {
  //cpu->debugOn();
  cpu->X = 1;
  LDA2(LDYZX, cpu->Y, 0x00, 0x01, 0, 0);
  LDA2(LDYZX, cpu->Y, 0x42, 0x43, 0, 0);
  LDA2(LDYZX, cpu->Y, 0x7E, 0x7F, 0, 0);
  LDA2(LDYZX, cpu->Y, 0x7F, 0x80, 0, 1);
  LDA2(LDYZX, cpu->Y, 0x80, 0x81, 0, 1);
  LDA2(LDYZX, cpu->Y, 0xFE, 0xFF, 0, 1);
  LDA2(LDYZX, cpu->Y, 0xFF, 0x00, 1, 0);
}

TEST_F(LDATest, LDAAbsolute) {
  //cpu->debugOn();
  LDA3(LDAA, cpu->A, 0x2000, 0x00, 1, 0);
  LDA3(LDAA, cpu->A, 0x2042, 0x42, 0, 0);
  LDA3(LDAA, cpu->A, 0x207F, 0x7F, 0, 0);
  LDA3(LDAA, cpu->A, 0x2080, 0x80, 0, 1);
  LDA3(LDAA, cpu->A, 0x20FE, 0xFE, 0, 1);
  LDA3(LDAA, cpu->A, 0x20FF, 0xFF, 0, 1);
}

TEST_F(LDATest, LDXAbsolute) {
  //cpu->debugOn();
  LDA3(LDXA, cpu->X, 0x2000, 0x00, 1, 0);
  LDA3(LDXA, cpu->X, 0x2042, 0x42, 0, 0);
  LDA3(LDXA, cpu->X, 0x207F, 0x7F, 0, 0);
  LDA3(LDXA, cpu->X, 0x2080, 0x80, 0, 1);
  LDA3(LDXA, cpu->X, 0x20FE, 0xFE, 0, 1);
  LDA3(LDXA, cpu->X, 0x20FF, 0xFF, 0, 1);
}

TEST_F(LDATest, LDYAbsolute) {
  //cpu->debugOn();
  LDA3(LDYA, cpu->Y, 0x2000, 0x00, 1, 0);
  LDA3(LDYA, cpu->Y, 0x2042, 0x42, 0, 0);
  LDA3(LDYA, cpu->Y, 0x207F, 0x7F, 0, 0);
  LDA3(LDYA, cpu->Y, 0x2080, 0x80, 0, 1);
  LDA3(LDYA, cpu->Y, 0x20FE, 0xFE, 0, 1);
  LDA3(LDYA, cpu->Y, 0x20FF, 0xFF, 0, 1);
}

TEST_F(LDATest, LDAAbsoluteX) {
  //cpu->debugOn();
  cpu->X = 2;
  LDA3(LDAAX, cpu->A, 0x2000, 0x02, 0, 0);
  LDA3(LDAAX, cpu->A, 0x2042, 0x44, 0, 0);
  LDA3(LDAAX, cpu->A, 0x207D, 0x7F, 0, 0);
  LDA3(LDAAX, cpu->A, 0x207E, 0x80, 0, 1);
  LDA3(LDAAX, cpu->A, 0x1FFE, 0x00, 1, 0);
  LDA3(LDAAX, cpu->A, 0x1FFF, 0x01, 0, 0);
}

TEST_F(LDATest, LDXAbsoluteY) {
  //cpu->debugOn();
  cpu->Y = 2;
  LDA3(LDXAY, cpu->X, 0x2000, 0x02, 0, 0);
  LDA3(LDXAY, cpu->X, 0x2042, 0x44, 0, 0);
  LDA3(LDXAY, cpu->X, 0x207D, 0x7F, 0, 0);
  LDA3(LDXAY, cpu->X, 0x207E, 0x80, 0, 1);
  LDA3(LDXAY, cpu->X, 0x1FFE, 0x00, 1, 0);
  LDA3(LDXAY, cpu->X, 0x1FFF, 0x01, 0, 0);
}

TEST_F(LDATest, LDYAbsoluteX) {
  //cpu->debugOn();
  cpu->X = 2;
  LDA3(LDYAX, cpu->Y, 0x2000, 0x02, 0, 0);
  LDA3(LDYAX, cpu->Y, 0x2042, 0x44, 0, 0);
  LDA3(LDYAX, cpu->Y, 0x207D, 0x7F, 0, 0);
  LDA3(LDYAX, cpu->Y, 0x207E, 0x80, 0, 1);
  LDA3(LDYAX, cpu->Y, 0x1FFE, 0x00, 1, 0);
  LDA3(LDYAX, cpu->Y, 0x1FFF, 0x01, 0, 0);
}

TEST_F(LDATest, LDAAbsoluteY) {
  //cpu->debugOn();
  cpu->Y = 2;
  LDA3(LDAAY, cpu->A, 0x2000, 0x02, 0, 0);
  LDA3(LDAAY, cpu->A, 0x2042, 0x44, 0, 0);
  LDA3(LDAAY, cpu->A, 0x207D, 0x7F, 0, 0);
  LDA3(LDAAY, cpu->A, 0x207E, 0x80, 0, 1);
  LDA3(LDAAY, cpu->A, 0x1FFE, 0x00, 1, 0);
  LDA3(LDAAY, cpu->A, 0x1FFF, 0x01, 0, 0);
}

TEST_F(LDATest, IndexIndirect) {
  //cpu->debugOn();
  mem.writeWord(0x20, 0x2000);
  mem.writeWord(0x22, 0x2020);
  mem.writeWord(0x24, 0x2042);
  cpu->X = 0;
  LDA2(LDAIXID, cpu->A, 0x20, 0x00, 1, 0);
  LDA2(LDAIXID, cpu->A, 0x22, 0x20, 0, 0);
  cpu->X = 2;
  LDA2(LDAIXID, cpu->A, 0x20, 0x20, 0, 0);
  LDA2(LDAIXID, cpu->A, 0x22, 0x42, 0, 0);
}

TEST_F(LDATest, IndirectIndex) {
  //cpu->debugOn();
  mem.writeWord(0x20, 0x2000);
  mem.writeWord(0x22, 0x2020);
  mem.writeWord(0x24, 0x2042);
  cpu->Y = 0;
  LDA2(LDAIDIX, cpu->A, 0x20, 0x00, 1, 0);
  LDA2(LDAIDIX, cpu->A, 0x22, 0x20, 0, 0);
  cpu->Y = 0x20;
  LDA2(LDAIDIX, cpu->A, 0x20, 0x20, 0, 0);
  cpu->Y = 0x22;
  LDA2(LDAIDIX, cpu->A, 0x22, 0x42, 0, 0);
}



int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
