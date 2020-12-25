// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief A list of 6502 assembly programs which can be loaded into sim6502
///
/// A 'program' is a vector of 'code' snippets.
/// Snippets are structs of {address, name, vector<uint8_t>}.
/// So a 'program' can contain data and instructions. Snippets can be loaded
/// by Memory::loadSnippets()
///
// The program entry point must be at 0x1000
//===----------------------------------------------------------------------===//

#pragma once

#include <Macros.h>
#include <Memory.h>
#include <Opcodes.h>

// $F0-$F1: Source address
// $F2-$F3: Destination address
// X register: Byte count
// Y register: mem offset (X -1)
// From http://prosepoetrycode.potterpcs.net/tag/6502/ with bug fixes
std::vector<Snippet> memcpy4 {
  {0x0020, "data", {0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x56, 0x78}},

  {0x00F0, "data", {0x24, 0x00, 0x20, 0x00}},

  {0x1000, "main()", {            // main program
      JSR,  0x00, 0x15,
      NOP
  }},

  {0x1500, "memcpy()", {            //memcpy subroutine
        LDYI, 0x03,
        LDXI, 0x04,
        LDAIDIX, 0xF0,  // 2 bytes, 5 cycles - lbl1
        STAIDIX, 0xF2,  // 2 bytes, 6 cycles
        DEY,
        DEX,            // 1 byte, 2 cycles
        BNE, (256-8),   // 2 bytes, 2-3 cycles - to lbl1
        RTS            // 1 byte, 6 cycles
  }}
};


// From https://dwheeler.com/6502/oneelkruns/asm1step.html
std::vector<Snippet> add16 {
  // Data: two 16-bit numbers loaded at address 0x20
  {0x20, "data", {
    0xCD, 0xAB,   // 0xABCD
    0x76, 0x98    // 0x9876   sum = 0x4443 + carry
  }},

  // Progam to add with carry
  {0x1000, "add16()", {
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


std::vector<Snippet> add32 {
  // Data: two 32-bit numbers loaded at address 0x20 and 0x24
  {0x20, "data", {
    0x01, 0xAA, 0x82, 0x0F,  // 0x0F82AA01
    0xFE, 0x55, 0x7D, 0xF0   // 0xF07D55FE  - sum 0xFFFFFFFF
  }},

  // Progam to add with carry
  {0x1000, "add32", {
    CLC,
    LDAZP, 0x20,
    ADCZP, 0x24,
    STAZP, 0x28,

    LDAZP, 0x21,
    ADCZP, 0x25,
    STAZP, 0x29,

    LDAZP, 0x22,
    ADCZP, 0x26,
    STAZP, 0x2A,

    LDAZP, 0x23,
    ADCZP, 0x27,
    STAZP, 0x2B,
    NOP
  }}
};


// Calculates Fibonacci numbers below 2^32 (F47)
// Last value is stored in 0x0028 - 0x002B
std::vector<Snippet> fibonacci32 {
  {0x0020, "data", {0x00, 0x00, 0x00, 0x00,  // F(n-2)
                    0x01, 0x00, 0x00, 0x00,  // F(n-1)
                    0x00, 0x00, 0x00, 0x00   // Fn
  }},

  {0x00F0, "data", {0x24, 0x00, 0x20, 0x00}}, // dst, src address

  {0x1000, "fibonacci32()", {             // fibonacci32()
    LDXI,       0,
    JSR,     0x00, 0x15,     // F(n-2) + F(n-1)  - lbl1

    TXA,                     // Copy F(n-1) to F(n-2), copy Fn to F(n-1)
    PHA,
    LDAI,    0x24,           // set dst and src addresses
    STAZP,   0xF0,
    LDAI,    0x20,
    STAZP,   0xF2,
    JSR,     0x00, 0x17,     // memcpy(0x0020, 0x0024, 4)
    LDAI,    0x28,           // set dst and src addresses
    STAZP,   0xF0,
    LDAI,    0x24,
    STAZP,   0xF2,
    JSR,     0x00, 0x17,     // memcpy(0x0024, 0x0028, 4)
    PLA,                     // pop A from stack
    TAX,                     // restore X (which was corrupted by this subr)

    INX,
    CPXI,  (47-1),           // STOP AT FIB 47
    BMI, (256-34),           // 34 back     - to lbl1
    NOP                      // WAS  RTS
  }},

  {0x1500, "add32()", {      // subr add32()
    CLC,
    LDAZP,   0x20,           // byte 0 (LSB)
    ADCZP,   0x24,
    STAZP,   0x28,

    LDAZP,   0x21,           // byte 1
    ADCZP,   0x25,
    STAZP,   0x29,

    LDAZP,   0x22,           // byte 2
    ADCZP,   0x26,
    STAZP,   0x2A,

    LDAZP,   0x23,           // byte 3 (MSB)
    ADCZP,   0x27,
    STAZP,   0x2B,
    RTS
  }},

  {0x1700, "memcpy4()", { // subr memcpy4()
    LDYI,    0x03,
    LDXI,    0x04,
    LDAIDIX, 0xF0,           // 2 bytes, 5 cycles - lbl1
    STAIDIX, 0xF2,           // 2 bytes, 6 cycles
    DEY,
    DEX,                     // 1 byte, 2 cycles
    BNE,  (256-8),           // 2 bytes, 2-3 cycles - to lbl1
    RTS                      // 1 byte, 6 cycles
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
  {0x20, "data", {
    6,                       // TMP @ 0x20
    1,5,6,3,1,5,3,0,4,2,6,4  // DB  @ 0x21
  }},

   // Main program - load Y, X, A and call Weekday routine
   // 2 may 1967 - tuesday (2)
   // 20 december 2020 - sunday (0)
  {0x1000, "main()", {
    LDYI, (2020 - 1900),     // 2020   (2020 - 1900)
    LDXI,  12,                // Month
    LDAI,  20,                // Day
    JSR,  0x00, 0x15,        // to weekday()
    NOP,
  }},

  // weekday() subroutine
  {0x1500, "weekday()", {
    CPXI, 3,                 // Year starts in March to bypass
    BCS, 1,                  // to lbl MARCH - leap year problem
    DEY,                     // If Jan or Feb, decrement year
    EORI, 0x7F,              // Invert A so carry works right - LBL MARCH
    CPYI, 200,               // Carry will be 1 if 22nd century
    ADCZX, 0x20,             // A is now day+month offset
    STAZP, 0x20,             // TMP is @ 0x20
    TYA,                     // Get the year
    JSR, 0x00, 0x20,         // MOD7() @ 0x2000 - Do a modulo to prevent overflow
    SBCZP, 0x20,             // TMP - Combine with day+month
    STAZP, 0x20,             // TMP
    TYA,                     // Get the year again
    LSR,                     // Divide it by 4
    LSR,
    CLC,                     // Add it to y+m+d and fall through
    ADCZP, 0x20,
    JSR, 0x00, 0x20,         // MOD7() @ 0x2000
    RTS,
  }},

  // MOD7() subroutine
  {0x2000, "MOD7()", {
      ADCI, 7,               // Returns (A+3) modulo 7
      BCC, (256-4),          // MOD7        ; for A in 0..255
      RTS
  }}
};



// Finding prime numbers using the Sieve of Eratosthenes
// From https://rosettacode.org/wiki/Sieve_of_Eratosthenes#6502_Assembly
// Seems to produce primes, but still has bugs
std::vector<Snippet> sieve {
  { 0x1000, "main", {
      LDAI, 0xD0,         // Find primes less than A
      JSR,  0x00, 0x11,   // sieve()
      NOP
  }},

  {0x1100, "sieve", {
      STAZP, 0xD0,        // ; value of n LABEL ERATOS:
      LDAI,  0x00,
      LDXI,  0x00,

      STAAX, 0x00, 0x20,  // ; populate array LABEL SETUP:
      ADCI,  0x01,
      INX,
      CPXZP, 0xD0,
      BEQ,      3,
      JMPA,  0x06, 0x11,  // to SETUP

      LDXI,  0x02,        // LABEL SET:

      LDAAX, 0x00, 0x20,  // ; find non-zero - LABEL SIEVE:
      INX,
      CPXZP, 0xD0,
      BEQ,     23,        // SIEVED
      CMPI,  0x00,
      BEQ,  (256-12),     // SIEVE
      STAZP, 0xD1,        // ; current prime
      CLC,                // LABEL MARK:
      ADCZP, 0xD1,
      TAY,
      LDAI,  0x00,
      STAAY, 0x00, 0x20,
      TYA,
      CMPZP, 0xD0,
      BCS,   (256-28),    // SIEVE
      JMPA,  0x23, 0x11,  // MARK
      LDXI,  0x01,        // LABEL:SIEVED
      LDYI,  0x00,

      INX,                // LABEL:COPY
      CPXZP, 0xD0,
      BCS,     14,        // TO:COPIED
      LDAAX, 0x00, 0x20,
      CMPI,  0x00,
      BEQ,  (256-12),     // TO:COPY
      STAAY, 0x00, 0x30,
      INY,
      JMPA,  0x38, 0x11,  // TO:COPY
      TYA,                // LABEL:COPIED: ; how many found
      RTS
  }}
};




// From http://www.6502.org/source/integers/ummodfix/ummodfix.htm
// Work in progress
// ZP vars start @ 0x20
#define VARN 0x20
#define CARRY VARN + 7
std::vector<Snippet> div32 {
  {0x0020, "data", {
    0x01, 0x00,            // divisor
    0x00, 0x00, 0xFF, 0xFF,// dividend
    0x00, 0x00             // scratchpad, carry
  }},

  {0x1000, "div32", {
    SEC,                   //  LABEL:START Detect overflow or /0 condition.
    LDAZP,    VARN + 2,    // Divisor must be more than high cell of dividend.  To
    SBCZP,    VARN,        // find out, subtract divisor from high cell of dividend//
    LDAZP,    VARN + 3,    // if carry flag is still set at the end, the divisor was
    SBCZP,    VARN + 1,    // not big enough to avoid overflow. This also takes care
    BCS,      45,          // to oflo$   // of any /0 condition.  Branch if overflow or /0 error.
                           // We will loop 16 times// but since we shift the dividend
    LDXI,     0x11,        // over at the same time as shifting the answer in, the
                           // operation must start AND finish with a shift of the
                           // low cell of the dividend (which ends up holding the
                           // quotient), so we start with 17 (11H) in X.
    ROLZP,    VARN + 4,    // LABEL:LOOP Move low cell of dividend left one bit, also shifting
    ROLZP,    VARN + 5,    // answer in. The 1st rotation brings in a 0, which later
                           // gets pushed off the other end in the last rotation.
    DEX,
    BEQ,      46,          // to end$ - Branch to the end if finished.

    ROLZP,    VARN + 2,    // Shift high cell of dividend left one bit, also
    ROLZP,    VARN + 3,    // shifting next bit in from high bit of low cell.
    LDAI,     0x00,        //
    STAZP,    CARRY,       // Zero old bits of CARRY so subtraction works right.
    ROLZP,    CARRY,       // Store old high bit of dividend in CARRY.  (For STZ
                           // one line up, NMOS 6502 will need LDA #0, STA CARRY.)
    SEC,                   // See if divisor will fit into high 17 bits of dividend
    LDAZP,    VARN + 2,    // by subtracting and then looking at carry flag.
    SBCZP,    VARN,        // First do low byte.
    STAZP,    VARN + 6,    // Save difference low byte until we know if we need it.
    LDAZP,    VARN + 3,    //
    SBCZP,    VARN + 1,    // Then do high byte.
    TAY,                   // Save difference high byte until we know if we need it.
    LDAZP,    CARRY,       // Bit 0 of CARRY serves as 17th bit.
    SBCI,     0x00,        // Complete the subtraction by doing the 17th bit before
    BCC,      256 - 35,    // to loop - determining if the divisor fit into the high 17 bits
                           // of the dividend.  If so, the carry flag remains set.
    LDAZP,    VARN + 6,    // If divisor fit into dividend high 17 bits, update
    STAZP,    VARN + 2,    // dividend high cell to what it would be after
    STYZP,    VARN + 3,    // subtraction.
    BCS,      256 - 43,    // to loop - Always branch.  NMOS 6502 could use BCS here.

    LDAI,     0xFF,        // LABEL:oflo$ - If overflow occurred, put FF
    STAZP,    VARN + 2,    // in remainder low byte
    STAZP,    VARN + 3,    // and high byte,
    STAZP,    VARN + 4,    // and in quotient low byte
    STAZP,    VARN + 5,    // and high byte.
    RTS                    // LABEL:end$
    }}
};
