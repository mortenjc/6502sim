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

void CPU::debug(std::string opcode, uint8_t val, uint16_t addr, AMode mode) {
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

  if (mode == None) {
    printf("%s\n", opcode.c_str());
  } else if (mode == Immediate) {
    printf("%s #$%02x\n", opcode.c_str(), val);
  } else if (mode == ZeroPage) {
    printf("%s $%02x (0x%02x)\n", opcode.c_str(), addr, val);
  } else if (mode == Absolute) {
    printf("%s $%04x (0x%02x)\n", opcode.c_str(), addr, val);
  } else if (mode == AbsoluteX) {
    printf("%s $%04x,X (0x%02x)\n", opcode.c_str(), addr, val);
  }
}


bool CPU::handleInstruction(uint8_t instruction) {
  int16_t pcinc{1};

  switch (instruction) {

    // Compare
    case CMPI: // Compare A register immediate
      pcinc = compare("CMP", A, CPU::Immediate);
      break;

    case CMPZP: // Compare A register zero-page
      pcinc = compare("CMP", A, CPU::ZeroPage);
      break;

    case CMPA: // Compare A register Absolute
      pcinc = compare("CMP", A, CPU::Absolute);
      break;

    case CPXI: // Compare X register immediate
      pcinc = compare("CPX", X, CPU::Immediate);
      break;

    case CPXZP: // Compare X register zero-page
      pcinc = compare("CPX", X, CPU::ZeroPage);
      break;

    case CPXA: // Compare X register Absolute
      pcinc = compare("CPX", X, CPU::Absolute);
      break;

    case CPYI: // Compare Y register immediate
      pcinc = compare("CPY", Y, CPU::Immediate);
      break;

    case CPYZP: // Compare Y register zero-page
      pcinc = compare("CPY", Y, CPU::ZeroPage);
      break;

    case CPYA: // Compare Y register Absolute
      pcinc = compare("CPY", Y, CPU::Absolute);
      break;



    // CLEAR/SET flags
    case CLC: // CLC - CLear Carry
      Status.bits.C = 0;
      debug("CLC", 0, 0, CPU::None);
      break;

    case SEC: // Set Carry
      Status.bits.C = 1;
      debug("SEC", 0, 0, CPU::None);
      break;

    case CLD: // CLear Decimal
      Status.bits.D = 0;
      debug("CLD", 0, 0, CPU::None);
      break;

    case SED: // Set Decimal
      Status.bits.D = 1;
      debug("SED", 0, 0, CPU::None);
      break;

    /// ADD SUBTRACT
    case ADCI: // Add with carry - immediate
      pcinc = addcarry("ADC", A, CPU::Immediate);
      break;

    case ADCZP: // Add with carry - zero page
      pcinc = addcarry("ADC", A, CPU::ZeroPage);
      break;

    case ADCA: // Add with carry - Absolute
      pcinc = addcarry("ADC", A, CPU::Absolute);
      break;


    // LOAD + STORE

    // LDA
    case LDAI: // LDA Imm
      pcinc = load("LDA", A, CPU::Immediate);
      break;

    case LDAZP: // LDA Zero Page
      pcinc = load("LDA", A, CPU::ZeroPage);
      break;

    case LDAAX: // LDA absolute,X
      pcinc = load("LDA", A, CPU::AbsoluteX);
      break;

    // LDX
    case LDXI: // LDX Imm
      pcinc = load("LDX", X, CPU::Immediate);
      break;

    case LDXZP: // LDX Zero Page
      pcinc = load("LDX", X, CPU::ZeroPage);
      break;

    // LDY
    case LDYI: // LDY Imm
      pcinc = load("LDY", Y, CPU::Immediate);
      break;

    case LDYZP: // LDY Zero Page
      pcinc = load("LDY", Y, CPU::ZeroPage);
      break;

    case LDYAX: // LDY absolute,X
      pcinc = load("LDY", Y, CPU::AbsoluteX);
      break;


    // STA
    case STAZP: { // STA Zero Page
      uint8_t zp = mem.readByte(PC + 1);
      mem.writeByte(zp, A);
      pcinc = 2;
      debug("STA", A, zp, CPU::ZeroPage);
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

    // STX
    case STXZP: { // STX Zero Page
      uint8_t zp = mem.readByte(PC + 1);
      mem.writeByte(zp, X);
      pcinc = 2;
      debug("STX", X, zp, CPU::ZeroPage);
    }
    break;

    case STXA: { // STX Absolute
      uint16_t addr = mem.readWord(PC + 1);
      mem.writeByte(addr, X);
      pcinc = 3;
      debug("STX", X, addr, CPU::Absolute);
    }
    break;

    // STY
    case STYA: { // STY Absolute
      uint16_t addr = mem.readWord(PC + 1);
      mem.writeByte(addr, Y);
      pcinc = 3;
      debug("STY", Y, addr, CPU::Absolute);
      //printf("STY (A 0x%04x) 0x%02x\n", addr, Y);
    }
    break;


    // INC and DEC
    case INX: // INX
      X++;
      updateStatus(X);
      debug("INX", 0, 0, CPU::None);
      break;

    case INY: // INY
      Y++;
      updateStatus(Y);
      debug("INY", 0, 0, CPU::None);
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

    case DEX: // DEX
      X--;
      updateStatus(X);
      debug("DEX", 0, 0, CPU::None);
    break;

    case DEY: // DEY
      Y--;
      updateStatus(Y);
      debug("DEY", 0, 0, CPU::None);
    break;

    // BRANCHES
    case BMI: // Branch if negative (MInus)
      pcinc = branch("BMI", Status.bits.N, 1);
      break;

    case BNE: // BNE - Branch if not Zero
      pcinc = branch("BNE", Status.bits.Z, 0);
      break;

    case BEQ: // BEQ - Branch if Zero
      pcinc = branch("BEQ", Status.bits.Z, 1);
      break;

    case BPL: // Branch if positive
      pcinc = branch("BPL", Status.bits.N, 0);
      break;

    case RTS: { // Return from Subroutine
      uint16_t newaddr = mem.readWord(SP) + 1;
      SP += 2;
      pcinc = 0;
      debug("RTS", 0, 0, CPU::None);
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

    case TAY:
      Y = A;
      updateStatus(Y);
      debug("TAY", 0, 0, CPU::None);
      break;

    case TYA:
      A = Y;
      updateStatus(A);
      debug("TYA", 0, 0, CPU::None);
      break;

    /// MISC
    case NOP: // No operation
      debug("NOP", 0, 0, CPU::None);
      break;

    default:
      debug();
      printf("0x%02x\nexiting...\n", instruction);
      running = false;
  }
  PC += pcinc;
  return running;
}
