// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Memory abstraction for the 6502 emulator
///
/// This class implements a rudimentary VIC-20 simulator based on the
/// original vic-20 ROMs and some custom and reverse engineered code.
//===----------------------------------------------------------------------===//

char getChar(uint8_t charcode) {
  switch (charcode) {
    case 0x00: return '@'; break;
    case 0x01: return 'A'; break;
    case 0x02: return 'B'; break;
    case 0x03: return 'C'; break;
    case 0x04: return 'D'; break;
    case 0x05: return 'E'; break;
    case 0x06: return 'F'; break;
    case 0x07: return 'G'; break;
    case 0x08: return 'H'; break;
    case 0x09: return 'I'; break;
    case 0x0A: return 'J'; break;
    case 0x0B: return 'K'; break;
    case 0x0C: return 'L'; break;
    case 0x0D: return 'M'; break;
    case 0x0E: return 'N'; break;
    case 0x0F: return 'O'; break;
    case 0x10: return 'P'; break;
    case 0x11: return 'Q'; break;
    case 0x12: return 'R'; break;
    case 0x13: return 'S'; break;
    case 0x14: return 'T'; break;
    case 0x15: return 'U'; break;
    case 0x16: return 'V'; break;
    case 0x17: return 'W'; break;
    case 0x18: return 'X'; break;
    case 0x19: return 'Y'; break;
    case 0x1A: return 'Z'; break;
    case 0x1B: return '['; break;
    case 0x1C: return '$'; break;  // pound sign, eventually
    case 0x1D: return ']'; break;
    case 0x1E: return '|'; break;  // up arrow
    case 0x1F: return '-'; break;  // left arrow
    case 0x20: return ' '; break;
    case 0x21: return '!'; break;
    case 0x22: return '"'; break;
    case 0x23: return '#'; break;
    case 0x24: return '$'; break;
    case 0x25: return '%'; break;
    case 0x26: return '&'; break;
    case 0x27: return '\''; break;
    case 0x28: return '('; break;
    case 0x29: return ')'; break;
    case 0x2A: return '*'; break;
    case 0x2B: return '+'; break;
    case 0x2C: return ','; break;
    case 0x2D: return '-'; break;
    case 0x2E: return '.'; break;
    case 0x2F: return '/'; break;
    case 0x30: return '0'; break;
    case 0x31: return '1'; break;
    case 0x32: return '2'; break;
    case 0x33: return '3'; break;
    case 0x34: return '4'; break;
    case 0x35: return '5'; break;
    case 0x36: return '6'; break;
    case 0x37: return '7'; break;
    case 0x38: return '8'; break;
    case 0x39: return '9'; break;
    case 0x3A: return ':'; break;
    case 0x3B: return ';'; break;
    case 0x3C: return '<'; break;
    case 0x3D: return '='; break;
    case 0x3E: return '>'; break;
    case 0x3F: return '?'; break;
    default:
      //printf("unknown code %02x\n", charcode);
      return '.';
      break;
  }
}
