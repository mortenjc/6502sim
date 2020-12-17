// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief A list of 6502 assembly programs which can be loaded into sim6502
///
/// A 'program' is a vector of 'code' snippets.
/// Snippets are pairs of {address, vector<uint8_t>}.
/// So a 'program' can contain data and instructions. Snippets can be loaded
/// by Memory::loadSnippets()
///
// The program entry point must be at 0x1000
//===----------------------------------------------------------------------===//

#pragma once

#include <Memory.h>
#include <Opcodes.h>

// Teating load immediate instructions
// early test program - pre unit tests
std::vector<Snippet> ldxyi_and_dec {
  {0x1000, {
    LDXI, 0x22,       // LDX Imm 0x22
    LDYI, 0x44,       // LDY Imm 0x44
    INX,             // INX
    INX,             // INX
    INY,             // INY
    INY,             // INY
    DEY,             // DEY
    DEY,             // DEY
    DEX,             // DEX
    DEX,             // DEX
    NOP }
  }
};

// Testing the BNE (branch) instruction
// Early test program - pre unit test
std::vector<Snippet>  countdown_y_from_10 {
  {0x1000, {
    LDXI, 0x00,       // LDX Imm 0
    LDYI, 0x0A,       // LDY Imm 10
    DEY,              // DEY
    BNE, 0x82,        // BNE (-2, prev line)
    INX,              // INX
    INX,              // INX
    NOP }
  }
};

// From https://rosettacode.org/wiki/Fibonacci_sequence#6502_Assembly
// stores data at 0x2000
std::vector<Snippet> fibonacci {
  {0x1000, {
    LDAI,  0,
    STAZP, 0xF0,       // LOWER NUMBER
    LDAI,  1,
    STAZP, 0xF1,       // HIGHER NUMBER
    LDXI,  0,
    LDAZP, 0xF1,       // LOOP
    STAAX, 0x00, 0x20,
    STAZP, 0xF2,       // OLD HIGHER NUMBER
    ADCZP, 0xF0,
    STAZP, 0xF1,       // NEW HIGHER NUMBER
    LDAZP, 0xF2,
    STAZP, 0xF0,       // NEW LOWER NUMBER
    INX,
    CPXI,  0x0A,       // STOP AT FIB(10)
    BMI,   0x93,       // -20
    NOP                // WAS  RTS
  }}
};


// From https://dwheeler.com/6502/oneelkruns/asm1step.html
std::vector<Snippet> add_two_16_bit_numbers {
  // Data: two 16-bit numbers loaded at address 0x20
  {0x20, {
    0xCD, 0xAB,   // 0xABCD
    0x76, 0x98    // 0x9876   sum = 0x4443 + carry
  }},

  // Progam to add with carry
  {0x1000, {
    CLC,
    LDAZP, 0x20,
    ADCZP, 0x22,
    STAZP, 0x24,
    LDAZP, 0x21,
    ADCZP, 0x23,
    STAZP, 0x25,
    NOP
  }}
};
