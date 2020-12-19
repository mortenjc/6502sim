// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief List of implemented 6502 opcodes
///
//===----------------------------------------------------------------------===//

#pragma once

#define BPL   0x10
#define CLC   0x18
#define JSR   0x20

#define ANDI  0x29
#define BMI   0x30
#define SEC   0x38
#define JMPA  0x4C

#define RTS   0x60
#define ADCZP 0x65
#define ADCI  0x69
#define ADCA  0x6D

#define STAZP 0x85
#define STXZP 0x86
#define DEY   0x88
#define STYA  0x8C
#define STXA  0x8E
#define TYA   0x98
#define STAAY 0x99
#define STAAX 0x9D

#define LDYI  0xA0
#define LDXI  0xA2
#define LDYZP 0xA4
#define LDAZP 0xA5
#define LDXZP 0xA6
#define TAY   0xA8
#define LDAI  0xA9
#define LDAAY 0xB9
#define LDYAX 0xBC
#define LDAAX 0xBD

#define CPYI  0xC0
#define CPYZP 0xC4
#define CMPZP 0xC5
#define INY   0xC8
#define CMPI  0xC9
#define DEX   0xCA
#define CPYA  0xCC
#define CMPA  0xCD
#define BNE   0xD0
#define CLD   0xD8
#define CPXI  0xE0
#define CPXZP 0xE4
#define INX   0xE8
#define INCA  0xEE
#define NOP   0xEA
#define CPXA  0xEC

#define BEQ   0xF0
#define SED   0xF8
