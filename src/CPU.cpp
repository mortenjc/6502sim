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
  S = 0xFF; // SP = SPBase + S
}

// Prints PC, SP, registers and flags
void CPU::debug() {
  if (not debugPrint)
    return;

  printf(" ; 0x%04x(%03x): A:%02x  X:%02x  Y:%02x ", PC, getSPAddr(), A, X, Y);
  printf(" [%c%c%c%c%c%c%c]\n",
      Status.bits.C ? 'c' : ' ' ,
      Status.bits.Z ? 'z' : ' ' ,
      Status.bits.I ? 'i' : ' ' ,
      Status.bits.D ? 'd' : ' ' ,
      Status.bits.B ? 'b' : ' ' ,
      Status.bits.O ? 'o' : ' ' ,
      Status.bits.N ? 'n' : ' ' );
}

// Make a disassembler-like listing of the current command
void CPU::disAssemble(Opcode opc, uint8_t byte, uint16_t word) {
  if (not debugPrint)
    return;

  int nbops = operands(opc.mode);
  if (nbops == 1) {
    printf("%04x %02x       ", PC, opc.opcode);
  } else if (nbops == 2) {
    printf("%04x %02x %02x    ", PC, opc.opcode, byte);
  } else {
    printf("%04x %02x %02x %02x ", PC, opc.opcode, byte, mem.readByte(PC+2));
  }

  printf("%s ", opc.mnem.c_str());

  switch (opc.mode) {
    case IndexedIndirect:
    case IndirectIndexed:
      break;

    case Implicit:
      printf("            ");
      break;

    case Accumulator:
      printf("A           ");
      break;

    case Relative: {
      int delta = (byte & 0x80) ? -(byte - 128) + 1 : (byte);
      uint16_t addr = PC + delta;
      printf("%04X (%4d) ", addr, delta);
    }
    break;

    case ZeroPage: {
      uint8_t val = mem.readByte(byte);
      printf("$%02X(%3d)    ", byte, val);
    }
    break;

    case ZeroPageX:
      printf("$%02X,X(%d)  ", byte, X);
      break;

    case ZeroPageY:
      printf("$%02X,Y(%d)  ", byte, Y);
      break;

    case Immediate:
      printf("#$%02X        ", byte);
      break;

    case Absolute:
      printf("$%04X       ", word);
      break;
    case AbsoluteX:
      printf("$%04X,X(%3d)", word, X);
      break;
    case AbsoluteY:
      printf("$%04X,Y(%3d)", word, Y);
      break;
    case Indirect:
      break;
  }
}

int CPU::operands(AMode mode) {
  switch (mode) {
    case IndexedIndirect:
    case IndirectIndexed:
      return 0;
      break;

    case Implicit:
    case Accumulator:
      return 1;
      break;

    case Relative:
    case ZeroPage:
    case ZeroPageX:
    case ZeroPageY:
    case Immediate:
      return 2;
      break;

    case Absolute:
    case AbsoluteX:
    case AbsoluteY:
    case Indirect:
      return 3;
      break;
  }
}


bool CPU::handleInstruction(uint8_t opcode) {
  int16_t pcinc {1};
  uint8_t byte = mem.readByte(PC + 1);
  uint16_t word = mem.readWord(PC + 1);

  auto & Opc = instset[opcode];

  disAssemble(Opc, byte, word);

  pcinc = operands(Opc.mode); // Works for all but jump instructions?

  switch (Opc.opcode) {

    //
    // Set/Clear flags
    case CLC: // CLC - CLear Carry
       Status.bits.C = 0;
       break;

     case SEC: // Set Carry
       Status.bits.C = 1;
       break;

     case CLD: // CLear Decimal
       Status.bits.D = 0;
       break;

     case SED: // Set Decimal
       Status.bits.D = 1;
       break;

    //
    // Load/Store
    case LDAI:
      A = byte;
      Opc.pf(this, A);
      break;

    case LDAZP:
      A = mem.readByte(byte);
      Opc.pf(this, A);
      break;

    case LDAZX:
      A = mem.readByte(uint8_t(byte + X));
      Opc.pf(this, A);
      break;

    case LDAA:
      A = mem.readByte(word);
      Opc.pf(this, A);
      break;

    case LDAAX:
      A = mem.readByte(word + X);
      Opc.pf(this, A);
      break;

    case LDAAY:
      A = mem.readByte(word + Y);
      Opc.pf(this, A);
      break;

    case STAZP:
      mem.writeByte(byte, A);
      break;

    case STAAX:
      mem.writeByte(word + X, A);
      break;

    case STAAY:
      mem.writeByte(word + Y, A);
      break;

    // X
    case LDXI:
      X = byte;
      Opc.pf(this, X);
      break;

    case LDXZP:
      X = mem.readByte(byte);
      Opc.pf(this, X);
      break;

    case LDXZY:
      X = mem.readByte(byte + Y);
      Opc.pf(this, X);
      break;

    case LDXA:
      X = mem.readByte(word);
      Opc.pf(this, X);
      break;

    case LDXAY:
      X = mem.readByte(word + Y);
      Opc.pf(this, X);
      break;

    case STXZP:
      mem.writeByte(byte, X);
      break;

    case STXZY:
      mem.writeByte(byte + Y, X);
      break;

    case STXA:
      mem.writeByte(word, X);
      break;


    //
    case LDYI:
      Y = byte;
      Opc.pf(this, Y);
      break;

    case LDYZP:
      Y = mem.readByte(byte);
      Opc.pf(this, Y);
      break;

    case LDYZX:
      Y = mem.readByte(byte + X);
      Opc.pf(this, Y);
      break;

    case LDYA:
      Y = mem.readByte(word);
      Opc.pf(this, Y);
      break;

    case LDYAX:
      Y = mem.readByte(word + X);
      Opc.pf(this, Y);
      break;

    case STYZP:
      mem.writeByte(byte, Y);
      break;

    case STYZX:
      mem.writeByte(byte + X, Y);
      break;

    case STYA:
      mem.writeByte(word, Y);
      break;

    //
    // Add/Subtract

    // Add
    case ADCI: // Add with carry Imm
      addcarry(A, byte);
      break;

    case ADCZP: //Add wih carry - ZP
      addcarry(A, mem.readByte(byte));
    break;

    case ADCZX: //Add wih carry - ZP,X
      addcarry(A, mem.readByte(byte + X));
    break;

    case ADCA: // Add with carry - Absolute
      addcarry(A, mem.readByte(word));
      break;

    case ADCAX: // Add with carry - Absolute,X
      addcarry(A, mem.readByte(word + X));
      break;

    case ADCAY: // Add with carry - Absolute,Y
      addcarry(A, mem.readByte(word + Y));
      break;

    // Subtract
    case SBCI: // Subtract with carry - Imm
      subcarry(A, byte);
      break;

    case SBCZP: // Subtract with carry - ZP
      subcarry(A, mem.readByte(byte));
      break;

    case SBCZX: // Subtract with carry - ZP,X
      subcarry(A, mem.readByte(byte + X));
      break;

    case SBCA: // Subtract with carry - Absolute
      subcarry(A, mem.readByte(word));
      break;

    case SBCAX: // Subtract with carry - Absolute,X
      subcarry(A, mem.readByte(word + X));
      break;

    case SBCAY: // Subtract with carry - Absolute,Y
      subcarry(A, mem.readByte(word + Y));
      break;


    //
    // Increment
    case INX:
      Opc.pf(this, X);
      break;

    case INY:
      Opc.pf(this, Y);
      break;

    case DEX:
      Opc.pf(this, X);
      break;

    case DEY:
      Opc.pf(this, X);
      break;

    case INCA: { // INC memory absolute
      uint8_t val = mem.readByte(word);
      val++;
      mem.writeByte(word, val);
      updateStatus(val);
    }
    break;


    //
    // Compare
    case CMPI:
      updateCompare(A, byte);
      break;

    case CPXI:
      updateCompare(X, byte);
      break;

    case CMPZP: // Compare A register zero-page
      updateCompare(A, mem.readByte(byte));
      break;

    case CMPA: // Compare A register Absolute
      updateCompare(A, mem.readByte(word));
      break;

    case CPXZP: // Compare X register zero-page
      updateCompare(X, mem.readByte(byte));
      break;

    case CPXA: // Compare X register Absolute
      updateCompare(X, mem.readByte(word));
      break;

    case CPYI: // Compare Y register immediate
      updateCompare(Y, byte);
      break;

    case CPYZP: // Compare Y register zero-page
      updateCompare(Y, mem.readByte(byte));
      break;

    case CPYA: // Compare Y register Absolute
      updateCompare(Y, mem.readByte(word));
      break;


    //
    // Branch/Jump/Return
    case BNE: // Branch if not Zero
      if (Status.bits.Z == 0) {
        pcinc = jumpRelative(byte);
      }
      break;

    case BEQ: // Branch if Zero
      if (Status.bits.Z == 1) {
        pcinc = jumpRelative(byte);
      }
      break;

    case BPL: // Branch if positive
      if (Status.bits.N == 0) {
        pcinc = jumpRelative(byte);
      }
      break;

    case BMI: // Branch if negative
      if (Status.bits.N == 1) {
        pcinc = jumpRelative(byte);
      }
    break;

    case BCC: // Branch if carry clear
      if (Status.bits.C == 0) {
        pcinc = jumpRelative(byte);
      }
    break;

    case BCS: // Branch if carry set
      if (Status.bits.C == 1) {
        pcinc = jumpRelative(byte);
      }
    break;

    case JSR:
        S -= 2;
        mem.writeWord(getSPAddr(), (PC + 2));
        PC = word;
        pcinc = 0;
      break;

    case JMPA: { //
      PC = mem.readWord(PC + 1);
      pcinc = 0;
    }
    break;

    case RTS:
      PC = mem.readWord(0x100 + S) + 1;
      S += 2;
      pcinc = 0;
      break;


    //
    // Logical
    case ANDI:
      A = A & byte;
      updateStatus(A);
      break;

    case ORAI:
      A = A | byte;
      updateStatus(A);
      break;

    case EORI:
      A = A ^ byte;
      updateStatus(A);
      break;

    case BITZP: {
      uint8_t M = mem.readByte(byte);
      uint8_t test = A & M;
      updateStatus(test);
      Status.bits.N = (M & 0x80) >> 7;
      Status.bits.O = (M & 0x40) >> 6;
    }
    break;

    case BITA: {
      uint8_t M = mem.readByte(word);
      uint8_t test = A & M;
      updateStatus(test);
      Status.bits.N = (M & 0x80) >> 7;
      Status.bits.O = (M & 0x40) >> 6;
    }
    break;

    // Shifts
    case LSR:
      Status.bits.C = A & 0x01;
      A = A >> 1;
      updateStatus(A);
      break;


    //
    // Register Transfers (Complete)
    case TAY:
      transfer(A, Y);
      break;

    case TYA:
      transfer(Y, A);
      break;

    case TAX:
      transfer(A, X);
      break;

    case TXA:
      transfer(X, A);
      break;


    //

    case NOP:
      break;

    case 0xFF:  // Commands that are invalid
      running = false;
      if (not debugPrint)
        printf("unregistered command ($%02x) exiting...\n", Opc.opcode);
      break;

    default:
      Opc.pf(this, X);
      running = false;
      printf(" ; unimplemented command ($%02x) exiting...\n", Opc.opcode);
      break;
  }

  PC += pcinc;

  debug();

  return running;
}