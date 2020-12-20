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

//
std::vector<Snippet>  inc_stopif_greatereq {
  {0x1000, {
    LDXI,  0xFF,       // max counts
    STXZP, 0xD0,       // Save FF in D0
    LDYI,  0x02,       // increment value
//
    CLC,
    STYZP, 0xD1,       // stored in D1

    LDAI,  0x00,
//LOOP
    ADCZP, 0xD1,
    BCC,   (128 + 2 +1), // to LOOP
    INY,
    BEQ,  5,
    JMPA, 0x06, 0x10,
    NOP }
  }
};


// Testing the compare (X,Y,A) in modes (I, ZP, A)
std::vector<Snippet>  compare {
  {0x1000, {
    LDAI,  0x00,
    LDXI,  0x00,
    LDYI,  0x00,
    CPXI,  0xFF,
    CPXZP, 0x10,
    CPXA,  0x00, 0x20, // 0x2000

    CPYI,  0xFF,
    CPYZP, 0x10,
    CPYA,  0x00, 0x20, // 0x2000

    CMPI,  0xFF,
    CMPZP, 0x10,
    CMPA,  0x00, 0x20, // 0x2000
    NOP }
  }
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


//
// 'real' programs below
//


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
    BMI,   0x93,       // 18 back -> 128 + 18 + 1
    NOP                // WAS  RTS
  }}
};



// From: http://www.6502.org/source/misc/dow.htm
//
// * This routine works for any date from 1900-03-01 to 2155-12-31.
// * No range checking is done, so validate input before calling.
// * Input: Y = year (0=1900, 1=1901, ..., 255=2155)
// *        X = month (1=Jan, 2=Feb, ..., 12=Dec)
// *        A = day (1 to 31)
// *
// * Output: Weekday in A (0=Sunday, 1=Monday, ..., 6=Saturday)
//
// Check against: https://www.wolframalpha.com/input/?i=day+of+week+1967+May+2
std::vector<Snippet> weekday {
  // Data
  {0x20, {
            6,                         // TMP @ 0x20
            1,5,6,3,1,5,3,0,4,2,6,4    // DB  @ 0x21
  }},

   // Main program - load Y, X, A and call Weekday routine
   // I seem to get an off by one on this compared to:
   // 2 may 1967 - tuesday (2)
   // 20 december 2020 - sunday (0)
  {0x1000, {
            LDYI, (1967 - 1900), // 2020   (2020 - 1900)
            LDXI,  5,            // Month
            LDAI,  2,            // Day
            JSR,  0x00, 0x15,    //
            NOP,
  }},

  // Weekday() subroutine
  {0x1500, {
            CPXI, 3,         // Year starts in March to bypass
            BCS, 3,          // to lbl MARCH - leap year problem
            DEY,             // If Jan or Feb, decrement year
/* MARCH */ EORI, 0x7F,      // Invert A so carry works right
            CPYI, 200,       // Carry will be 1 if 22nd century
            ADCZX, 0x20,     // A is now day+month offset
            STAZP, 0x20,     // TMP is @ 0x20
            TYA,             // Get the year
            JSR, 0x00, 0x20, // MOD7() @ 0x2000 - Do a modulo to prevent overflow
            SBCZP, 0x20,     // TMP - Combine with day+month
            STAZP, 0x20,     // TMP
            TYA,             // Get the year again
            LSR,             // Divide it by 4
            LSR,
            CLC,             // Add it to y+m+d and fall through
            ADCZP, 0x20,
            JSR, 0x00, 0x20, // MOD7() @ 0x2000
            RTS,
  }},

  // MOD7() subroutine
  {0x2000, {
            ADCI, 7,         // Returns (A+3) modulo 7
            BCC, 128+2+1,    // MOD7        ; for A in 0..255
            RTS
  }}
};
