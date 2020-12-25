// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief List of implemented 6502 opcodes
///
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>
#include <string>

class CPU;

enum AMode { Implied,  Accumulator, Immediate,
             ZeroPage, ZeroPageX, ZeroPageY,
             Relative, Absolute,  AbsoluteX, AbsoluteY,
             Indirect, IndexedIndirect, IndirectIndexed};

struct Opcode {
uint8_t opcode;
std::string mnem;
AMode mode;
int (*pf)(CPU * cpu, uint8_t & reg);
};

#define BRK     0x00
#define ASLZP   0x06
#define PHP     0x08
#define ORAI    0x09
#define ASLACC  0x0A
#define ASLA    0x0E

#define BPL     0x10
#define ASLZX   0x16
#define CLC     0x18
#define ASLAX   0x1E

#define JSR     0x20
#define BITZP   0x24
#define ROLZP   0x26
#define PLP     0x28
#define ANDI    0x29
#define ROLACC  0x2A
#define BITA    0x2C
#define ROLA    0x2E

#define BMI     0x30
#define ROLZX   0x36
#define SEC     0x38
#define ROLAX   0x3E

#define RTI     0x40
#define LSRZP   0x46
#define PHA     0x48
#define EORI    0x49
#define LSR     0x4A
#define JMPA    0x4C
#define LSRA    0x4E

#define BVC     0x50
#define LSRZX   0x56
#define CLINT   0x58  // namespace clash with CLI11
#define LSRAX   0x5E

#define RTS     0x60
#define ADCZP   0x65
#define RORZP   0x66
#define PLA     0x68
#define ADCI    0x69
#define RORACC  0x6A
#define JMPI    0x6C
#define ADCA    0x6D
#define RORA    0x6E

#define BVS     0x70
#define ADCZX   0x75
#define RORZX   0x76
#define SEI     0x78
#define ADCAY   0x79
#define ADCAX   0x7D
#define RORAX   0x7E

#define STAIXID 0x81
#define STYZP   0x84
#define STAZP   0x85
#define STXZP   0x86
#define DEY     0x88
#define TXA     0x8A
#define STYA    0x8C
#define STAA    0x8D
#define STXA    0x8E

#define BCC     0x90
#define STAIDIX 0x91
#define STYZX   0x94
#define STAZX   0x95
#define STXZY   0x96
#define TYA     0x98
#define STAAY   0x99
#define TXS     0x9A
#define STAAX   0x9D

#define LDYI    0xA0
#define LDAIXID 0xA1
#define LDXI    0xA2
#define LDYZP   0xA4
#define LDAZP   0xA5
#define LDXZP   0xA6
#define TAY     0xA8
#define LDAI    0xA9
#define TAX     0xAA
#define LDYA    0xAC
#define LDAA    0xAD
#define LDXA    0xAE

#define BCS     0xB0
#define LDAIDIX 0xB1
#define LDYZX   0xB4
#define LDAZX   0xB5
#define LDXZY   0xB6
#define CLV     0xB8
#define LDAAY   0xB9
#define TSX     0xBA
#define LDYAX   0xBC
#define LDAAX   0xBD
#define LDXAY   0xBE

#define CPYI    0xC0
#define CMPIXID 0xC1
#define CPYZP   0xC4
#define CMPZP   0xC5
#define INY     0xC8
#define CMPI    0xC9
#define DEX     0xCA
#define CPYA    0xCC
#define CMPA    0xCD

#define BNE     0xD0
#define CMPIDIX 0xD1
#define CMPZX   0xD5
#define CLD     0xD8
#define CMPAY   0xD9
#define CMPAX   0xDD

#define CPXI    0xE0
#define CPXZP   0xE4
#define SBCZP   0xE5
#define INX     0xE8
#define SBCI    0xE9
#define SBCA    0xED
#define INCA    0xEE
#define NOP     0xEA
#define CPXA    0xEC

#define BEQ     0xF0
#define SBCZX   0xF5
#define SED     0xF8
#define SBCAY   0xF9
#define SBCAX   0xFD
