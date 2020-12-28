// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief 6502 CPU emulator - handleInstruction() methos
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

#include <CPU.h>
#include <Memory.h>

bool CPU::handleInstruction(uint8_t opcode) {
  uint16_t addr = PC;
  uint8_t byte = mem.readByte(PC + 1);
  uint8_t byte2 = mem.readByte(PC + 2);
  uint16_t word = mem.readWord(PC + 1);
  auto & Opc = instset[opcode];

  disAssemble(addr, Opc, byte, byte2, word);
  PC += operands(Opc.mode); // Works for all but jump instructions?

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

     case CLV: // Clear Overflow
       Status.bits.O = 0;
       break;

     case SED: // Set Decimal
       Status.bits.D = 1;
       break;

     case CLINT: // Clear Interrupt
       Status.bits.I = 0;
       break;

     case SEI: // Set Interrupt
       Status.bits.I = 1;
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

    case LDAIXID: // Indexed, Indirect ?
      A = mem.readByte(mem.readWord(uint8_t(byte + X)));
      Opc.pf(this, A);
      break;

    case LDAIDIX: {// Indirect, Indexed ?
      uint16_t addr = mem.readWord(byte) + Y;
      A = mem.readByte(addr);
      Opc.pf(this, A);
      }
      break;

    case STAZP:
      mem.writeByte(byte, A);
      break;

    case STAZX:
      mem.writeByte(uint8_t(byte + X), A);
      break;

    case STAA:
      mem.writeByte(word, A);
      break;

    case STAAX:
      mem.writeByte(word + X, A);
      break;

    case STAAY:
      mem.writeByte(word + Y, A);
      break;

    case STAIXID: // Indexed, Indirect
      mem.writeByte(mem.readWord(uint8_t(byte + X)), A);
      break;

    case STAIDIX: // Indirect, Indexed
      mem.writeByte(mem.readWord(byte) + Y, A);
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
      X = mem.readByte((byte + Y) % 256);
      assert(((byte+Y) % 256) == (uint8_t(byte + Y)));
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
      mem.writeByte(uint8_t(byte + Y), X);
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
      Y = mem.readByte(uint8_t(byte + X));
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
      mem.writeByte(uint8_t(byte + X), Y);
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
      addcarry(A, mem.readByte(uint8_t(byte + X)));
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

    case ADCIXID: // Add with carry - Indexed, indirect
      addcarry(A, mem.readByte(mem.readWord(uint8_t(byte + X))));
      break;

    case ADCIDIX: // Add with carry - Indexed, indirect
      addcarry(A, mem.readByte(mem.readWord(byte) + Y));
      break;

    // Subtract
    case SBCI: // Subtract with carry - Imm
      subcarry(A, byte);
      break;

    case SBCZP: // Subtract with carry - ZP
      subcarry(A, mem.readByte(byte));
      break;

    case SBCZX: // Subtract with carry - ZP,X
      subcarry(A, mem.readByte(uint8_t(byte + X)));
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

    case SBCIXID: // Add with carry - Indexed, indirect
      subcarry(A, mem.readByte(mem.readWord(uint8_t(byte + X))));
      break;

    case SBCIDIX: // Add with carry - Indexed, indirect
      subcarry(A, mem.readByte(mem.readWord(byte) + Y));
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
      Opc.pf(this, Y);
      break;

    case INCZP: { // INC memory zeropage
      uint8_t val = mem.readByte(byte);
      val++;
      mem.writeByte(byte, val);
      updateStatusZN(val);
    }
    break;

    case INCZX: { // INC memory zeropage
      uint8_t val = mem.readByte(uint8_t(byte + X));
      val++;
      mem.writeByte(uint8_t(byte + X), val);
      updateStatusZN(val);
    }
    break;

    case INCA: { // INC memory absolute
      uint8_t val = mem.readByte(word);
      val++;
      mem.writeByte(word, val);
      updateStatusZN(val);
    }
    break;

    case INCAX: { // INC memory absolute
      uint8_t val = mem.readByte(word + X);
      val++;
      mem.writeByte(word + X, val);
      updateStatusZN(val);
    }
    break;


    case DECZP: { // DEC memory zeropage
      uint8_t val = mem.readByte(byte);
      val--;
      mem.writeByte(byte, val);
      updateStatusZN(val);
    }
    break;

    case DECZX: { // DEC memory zeropage
      uint8_t val = mem.readByte(uint8_t(byte + X));
      val--;
      mem.writeByte(uint8_t(byte + X), val);
      updateStatusZN(val);
    }
    break;

    case DECA: { // DEC memory absolute X
      uint8_t val = mem.readByte(word);
      val--;
      mem.writeByte(word, val);
      updateStatusZN(val);
    }
    break;

    case DECAX: { // DEC memory absolute X
      uint8_t val = mem.readByte(word + X);
      val--;
      mem.writeByte(word + X, val);
      updateStatusZN(val);
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

    case CMPZX: // Compare A register zero-page
      updateCompare(A, mem.readByte(uint8_t(byte + X)));
      break;

    case CMPA: // Compare A register Absolute
      updateCompare(A, mem.readByte(word));
      break;

    case CMPAX: // Compare A register AbsoluteY
      updateCompare(A, mem.readByte(word + X));
      break;

    case CMPAY: // Compare A register AbsoluteY
      updateCompare(A, mem.readByte(word + Y));
      break;

    case CMPIXID: // Compare A Indexed, indirect
    updateCompare(A, mem.readByte(mem.readWord(uint8_t(byte + X))));
    break;

    case CMPIDIX: // Compare A Indirect,indexed
    updateCompare(A, mem.readByte(mem.readWord(byte) + Y));
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
        PC += jumpRelative(byte);
      }
      break;

    case BEQ: // Branch if Zero
      if (Status.bits.Z == 1) {
        PC += jumpRelative(byte);
      }
      break;

    case BPL: // Branch if positive
      if (Status.bits.N == 0) {
        PC += jumpRelative(byte);
      }
      break;

    case BMI: // Branch if negative
      if (Status.bits.N == 1) {
        PC += jumpRelative(byte);
      }
    break;

    case BCC: // Branch if carry clear
      if (Status.bits.C == 0) {
        PC += jumpRelative(byte);
      }
    break;

    case BCS: // Branch if carry set
      if (Status.bits.C == 1) {
        PC += jumpRelative(byte);
      }
    break;

    case BVC: // Branch if overflow clear
      if (Status.bits.O == 0) {
        PC += jumpRelative(byte);
      }
    break;

    case BVS: // Branch if overflow set
      if (Status.bits.O == 1) {
        PC += jumpRelative(byte);
      }
    break;


    /// Group: Jumps & Calls (Complete)
    case JSR:
        PC--;
        mem.writeWord(getSPAddr() - 1, PC);
        S -= 2;
        PC = word;
        break;

    case JMPA: // Jump absolute
      PC = word;
      break;

    case JMPI: { // Jump indirect
        PC = mem.readWord(word);
      }
      break;

    case RTS:
      S+=2;
      PC = mem.readWord(getSPAddr() - 1) + 1;
      break;


    //
    // Logical
    case ANDI:
      A = A & byte;
      updateStatusZN(A);
      break;

    case ANDZP:
      andMem(byte);
      break;

    case ANDZX:
      andMem(uint8_t(byte + X));
      break;

    case ANDA:
      andMem(word);
      break;

    case ANDAX:
      andMem(word + X);
      break;

    case ANDAY:
      andMem(word + Y);
      break;

    case ANDIXID:
      andMem(mem.readWord(uint8_t(byte + X)));
      break;

    case ANDIDIX:
      andMem(mem.readWord(byte) + Y);
      break;

    case ORAI:
      A = A | byte;
      updateStatusZN(A);
      break;

    case ORAZP:
      oraMem(byte);
      break;

    case ORAZX:
      oraMem(uint8_t(byte + X));
      break;

    case ORAA:
      oraMem(word);
      break;

    case ORAAX:
      oraMem(word + X);
      break;

    case ORAAY:
      oraMem(word + Y);
      break;

    case ORAIXID:
      oraMem(mem.readWord(uint8_t(byte + X)));
      break;

    case ORAIDIX:
      oraMem(mem.readWord(byte) + Y);
      break;


    case EORI:
      A = A ^ byte;
      updateStatusZN(A);
      break;

    case EORZP:
      eorMem(byte);
      break;

    case EORZX:
      eorMem(uint8_t(byte + X));
      break;

    case EORA:
      eorMem(word);
      break;

    case EORAX:
      eorMem(word + X);
      break;

    case EORAY:
      eorMem(word + Y);
      break;

    case EORIXID:
      eorMem(mem.readWord(uint8_t(byte + X)));
      break;

    case EORIDIX:
      eorMem(mem.readWord(byte) + Y);
      break;

    case BITZP: {
      uint8_t M = mem.readByte(byte);
      uint8_t test = A & M;
      updateStatusZN(test);
      Status.bits.N = (M & 0x80) >> 7;
      Status.bits.O = (M & 0x40) >> 6;
    }
    break;

    case BITA: {
      uint8_t M = mem.readByte(word);
      uint8_t test = A & M;
      updateStatusZN(test);
      Status.bits.N = (M & 0x80) >> 7;
      Status.bits.O = (M & 0x40) >> 6;
    }
    break;


    // Shifts
    case ASLACC: {
        Status.bits.C = (A >> 7);
        A = A << 1;
        updateStatusZN(A);
      }
      break;

    case ASLZP:
      asl(byte);
      break;


    case ASLZX:
      asl(uint8_t(byte + X));
      break;

    case ASLA:
      asl(word);
      break;

    case ASLAX:
      asl(word + X);
      break;

    case LSR:
      Status.bits.C = A & 0x01;
      A = A >> 1;
      updateStatusZN(A);
      break;

    case LSRZP:
      lsr(byte);
      break;

    case LSRZX:
      lsr(uint8_t(byte + X));
      break;

    case LSRA:
      lsr(word);
      break;

    case LSRAX:
      lsr(word + X);
      break;

    case ROLACC: {
        uint8_t oldCarry = Status.bits.C;
        Status.bits.C = (A >> 7); // bit 7 -> Carry
        A = A << 1;
        A = A + oldCarry;
        updateStatusZN(A);
      }
      break;

    case ROLZP:
      rol(byte);
      break;

    case ROLZX:
      rol(uint8_t(byte + X));
      break;

    case ROLA:
      rol(word);
      break;

    case ROLAX:
      rol(word + X);
      break;

    case RORACC: {
        uint8_t oldCarry = Status.bits.C;
        Status.bits.C = (A &0x01); // bit 0 -> Carry
        A = A >> 1;
        if (oldCarry) {
          A = A + 128;
        }
        updateStatusZN(A);
      }
      break;

    case RORZP:
      ror(byte);
      break;

    case RORZX:
      ror(uint8_t(byte + X));
      break;

    case RORA:
      ror(word);
      break;

    case RORAX:
      ror(word + X);
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

    // Stack operations
    case TSX: // Transfer Stack Pointer to X
      X = S;
      updateStatusZN(X);
      break;

    case TXS: // Transfer X to Stack Pointer
      S = X;
      break;

    case PHA:
      mem.writeByte(getSPAddr(), A);
      S--;
      break;

    case PHP: {
        uint8_t oldB = Status.bits.B;
        uint8_t oldr = Status.bits.r;
        Status.bits.B = 1;
        Status.bits.r = 1;
        mem.writeByte(getSPAddr(), Status.mask);
        Status.bits.B = oldB;
        Status.bits.r = oldr;
        S--;
      }
      break;

    case PLA:
      S++;
      A = mem.readByte(getSPAddr());
      updateStatusZN(A);
      break;

    case PLP:
      S++;
      Status.mask = mem.readByte(getSPAddr());
      break;


    // System functions
    case NOP:
      break;

    case BRK: {
        PC++;
        stackPush(PC >> 8);
        stackPush(PC & 0xFF);
        uint8_t oldB = Status.bits.B;
        uint8_t oldr = Status.bits.r;
        Status.bits.B = 1;
        Status.bits.r = 1;
        stackPush(Status.mask);
        Status.bits.B = oldB;
        Status.bits.r = oldr;
        PC = mem.readWord(0xFFFE);
        Status.bits.I = 1;
      }
      break;

    case RTI:
      Status.mask = stackPop();
      PC = stackPop();
      PC += stackPop() << 8;
      break;

    case 0xFF:  // Commands that are invalid
      running = false;
      //if (not debugPrint)
        printf("$%02x", opcode);
      break;

    default:
      Opc.pf(this, X);
      running = false;
      printf("unimplemented command ($%02x) exiting...\n", Opc.opcode);
      break;
  }

  printRegisters();

  if (debugPrint) {
    printf("\n");
    // mem.dump(0x400, 16);
  }

  if (PC == trcAddr) {
    debugOn();
  }

  if (addr == PC) {
    printf("loop detected (PC: %04X), exiting ...\n", PC);
    running = false;
  }

  return running;
}
