// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief 6502 CPU emulator
///
/// Handles program counter (PC), stack pointer (SP), the three registers
/// X,Y,A and status flags.
/// The core logic revolves around a loop of the two methods
///    getInstruction()
///    handleInstruction()
///
/// Individual opcodes are implemented in a bit switch statement
/// in handleInstruction (CPU.cpp)
/// Some debugging functionality added
//===----------------------------------------------------------------------===//

#include <cassert>
#include <CPU.h>
#include <Opcodes.h>

// Sets registers and flags to zero
// Sets Stack pointer (SP) to default (0x1FF)
// Sets program counter (PC) to value read from memory
void CPU::reset() {
  A = X = Y = 0;
  Status.mask = 0;
  PC = mem.readWord(power_on_reset_addr);
  SP = stack_pointer_addr;
}


// Prints PC, SP, registers and flags
void CPU::debug() {
  printf("0x%04x(%03x): A:%02x  X:%02x  Y:%02x ",
    PC, SP, A, X, Y);

  printf(" [%c%c%c%c%c%c%c] ",
      Status.bits.C ? 'c' : ' ' ,
      Status.bits.Z ? 'z' : ' ' ,
      Status.bits.I ? 'i' : ' ' ,
      Status.bits.D ? 'd' : ' ' ,
      Status.bits.B ? 'b' : ' ' ,
      Status.bits.O ? 'o' : ' ' ,
      Status.bits.N ? 'n' : ' ' );
}

uint8_t CPU::getInstruction() {
   return mem.readByte(PC);
}

bool CPU::handleInstruction(uint8_t instruction) {
  int16_t pcinc{1};

  switch (instruction) {

    // Compare
    case CPXI: { // Compare X register immediate
      Status.bits.C = 0;
      Status.bits.Z = 0;
      Status.bits.N = 0;
      uint8_t val = mem.readByte(PC + 1);
      if (X >= val) {
        Status.bits.C = 1;
      }
      if (X == val) {
        Status.bits.Z = 1;
      }
      if ((X - val)&0x80) {
        Status.bits.N = 1;
      }
      pcinc = 2;
      debug();
      printf("CPX (I) 0x%02x\n", val);
    }
    break;

    // CLEAR/SET flags
    case CLC: { // CLC - CLear Carry
      Status.bits.C = 0;
      debug();
      printf("CLC\n");
    }
    break;

    case SEC: { // Set Carry
      Status.bits.C = 1;
      debug();
      printf("SEC\n");
    }
    break;

    case CLD: { // CLear Decimal
      Status.bits.D = 0;
      debug();
      printf("CLD\n");
    }
    break;

    case SED: { // Set Decimal
      Status.bits.D = 1;
      debug();
      printf("SED\n");
    }
    break;

    /// ADD SUBTRACT
    case ADCZP: { // Add with carry - zero page
      uint8_t zp = mem.readByte(PC + 1);
      uint8_t zpval = mem.readByte(zp);
      uint8_t tmp = A + zpval + Status.bits.C;
      //printf("\nA + zpval + C == 0x%02x + 0x%02x + %d\n", A, zpval, Status.bits.C);
      if ((A + zpval + Status.bits.C) > 255) {
        Status.bits.C = 1; // Set carry
      } else {
        Status.bits.C = 0; // Clear carry
      }
      A = tmp;
      updateStatus(A);
      pcinc = 2;
      debug();
      printf("ADC (ZP 0x%02x) %d\n", zp, A);
    }
    break;

    //
    // LOAD + STORE
    case LDAZP: { // LDA Zero Page
      uint8_t zp = mem.readByte(PC + 1);
      A = mem.readByte(zp);
      updateStatus(A);
      pcinc = 2;
      debug();
      printf("LDA (ZP 0x%02x) %d\n", zp, A);
    }
    break;

    case LDXZP: { // LDX Zero Page
      uint8_t zp = mem.readByte(PC + 1);
      X = mem.readByte(zp);
      updateStatus(X);
      pcinc = 2;
      debug();
      printf("LDX (ZP 0x%02x) %d\n", zp, X);
    }
    break;

    case LDYZP: { // LDY Zero Page
      uint8_t zp = mem.readByte(PC + 1);
      Y = mem.readByte(zp);
      updateStatus(Y);
      pcinc = 2;
      debug();
      printf("LDY (ZP 0x%02x) %d\n", zp, Y);
    }
    break;

    case STAZP: { // STA Zero Page
      uint8_t zp = mem.readByte(PC + 1);
      mem.writeByte(zp, A);
      pcinc = 2;
      debug();
      printf("STA (ZP 0x%02x) %d\n", zp, A);
    }
    break;

    case STXZP: { // STX Zero Page
      uint8_t zp = mem.readByte(PC + 1);
      mem.writeByte(zp, X);
      pcinc = 2;
      debug();
      printf("STX (ZP 0x%02x) %d\n", zp, X);
    }
    break;

    case STXA: { // STX Absolute
      uint16_t addr = mem.readWord(PC + 1);
      mem.writeByte(addr, X);
      pcinc = 3;
      debug();
      printf("STX (A 0x%04x) %d\n", addr, X);
    }
    break;

    case STYA: { // STY Absolute
      uint16_t addr = mem.readWord(PC + 1);
      mem.writeByte(addr, Y);
      pcinc = 3;
      debug();
      printf("STY (A 0x%04x) 0x%02x\n", addr, Y);
    }
    break;

    case STAAX: { // STA Absolute,X
      uint16_t addr = mem.readWord(PC + 1);
      mem.writeByte(addr + X, A);
      pcinc = 3;
      debug();
      printf("STA (A 0x%04x),X 0x%02x\n", addr, X);
    }
    break;

    case LDAI: { // LDA Imm
      A = mem.readByte(PC + 1);
      updateStatus(A);
      pcinc = 2;
      debug();
      printf("LDA (I) 0x%02x\n", A);
    }
    break;

    case LDXI: { // LDX Imm
      X = mem.readByte(PC + 1);
      updateStatus(X);
      pcinc = 2;
      debug();
      printf("LDX (I) 0x%02x\n", X);
    }
    break;

    case LDYI: { // LDY Imm
      Y = mem.readByte(PC + 1);
      updateStatus(Y);
      pcinc = 2;
      debug();
      printf("LDY (I) 0x%02x\n", Y);
    }
    break;

    // INC and DEC
    case INX: { // INX
      X++;
      updepri(X, "INX");
    }
    break;

    case INY: { // INY
      Y++;
      updepri(Y, "INY");
    }
    break;

    case INCA: { // INC memory absolute
      uint16_t address = mem.readWord(PC + 1);
      uint8_t val = mem.readByte(address);
      val++;
      mem.writeByte(address, val);
      pcinc = 3;
      updateStatus(val);
      debug();
      printf("INC (A 0x%04x) 0x%02x", address, val);
    }
    break;

    case DEX: { // DEX
      X--;
      updepri(X, "DEX");
    }
    break;

    case DEY: { // DEY
      Y--;
      updepri(Y, "DEY");
    }
    break;

    // BRANCHES
    case BMI: { // Branch if negative (MInus)
      uint8_t val = mem.readByte(PC + 1);
      if (Status.bits.N == 1) {
          if (val & 0x80) {
            pcinc = - (val - 0x80) + 1;
          } else {
            pcinc = val;
          }
      } else {
        pcinc = 2;
      }
      debug();
      printf("BMI 0x%02x (%d)\n", val, pcinc);
    }
    break;

    case BNE: { // BNE - Branch if not Zero
      uint8_t val = mem.readByte(PC + 1);
      if (Status.bits.Z != 1) {
          if (val & 0x80) {
            pcinc = - (val - 0x80) + 1;
          } else {
            pcinc = val;
          }
      } else {
        pcinc = 2;
      }
      debug();
      printf("BNE 0x%02x (%d)\n", val, pcinc);
    }
    break;

    case BEQ: { // BEQ - Branch if Zero
      uint8_t val = mem.readByte(PC + 1);
      if (Status.bits.Z == 1) {
          if (val & 0x80) {
            pcinc = - (val - 0x80) + 1;
          } else {
            pcinc = val;
          }
      } else {
        pcinc = 2;
      }
      debug();
      printf("BEQ 0x%02x (%d)\n", val, pcinc);
    }
    break;

    case RTS: { // Return from Subroutine
      uint16_t newaddr = mem.readWord(SP) + 1;
      SP += 2;
      pcinc = 0;
      debug();
      printf("RTS\n");
      PC = newaddr;
    }
    break;

    case JSR: { // Jump to Subroutine
      uint16_t addr = mem.readWord(PC + 1);
      SP -= 2;
      mem.writeWord(SP, (PC + 2));
      debug();
      printf("JSR 0x%04x\n", addr);
      PC = addr;
      pcinc = 0;
    }
    break;

    case JMPA: { // Jump absolute
      uint16_t addr = mem.readWord(PC + 1);
      debug();
      printf("JMP 0x%04x\n", addr);
      PC = addr;
      pcinc = 0;
    }
    break;

    /// Logical
    case ANDI: { // AND Immediate
      uint8_t val = mem.readByte(PC + 1);
      A = A & val;
      updateStatus(A);
      pcinc = 2;
      debug();
      printf("AND (I) 0x%02x\n", val);
    }
    break;

    /// MISC
    case NOP: { // No operation
      debug();
      printf("NOP\n");
    }
    break;

    default:
      debug();
      printf("0x%02x\nexiting...\n", instruction);
      running = false;
  }
  PC += pcinc;
  return running;
}
