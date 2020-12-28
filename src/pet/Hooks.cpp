


#include <src/pet/Hooks.h>


Hooks::Hooks(Memory & memory, int X, int Y) : mem(memory)  {
  initscr();
  (void) cbreak();		// take input chars one at a time, no wait for \n
  (void) noecho();		// don't echo input
  refresh();

  win = newwin(Y+1, X+1, 0, 0);
  box(win, '|', '-');
  nodelay(win, TRUE); // don't 'hang' in getch()
  scrollok(win, TRUE);
  keypad(win, TRUE);
  wmove(win, 0, 0);
}

Hooks::~Hooks() {
  endwin();
}

bool Hooks::getChar(int & ch) {
  ch = wgetch(win);
  if (ch == ERR) {
    return false;
  }
  return true;
}


void Hooks::printScreen(int X, int Y, uint16_t screenaddr) {
  char buffer[1024];
  for (int y = 0; y < Y; y++) {
    wmove(win, y+1, 1);
    int i = 0;
    for (int x = 0; x < X; x++) {
      uint16_t addr = screenaddr + y*X + x;
      buffer[i++] = charToAscii(mem.readByte(addr));
    }
    buffer[i++] = 0;
    wprintw(win, "%s", buffer);
  }
  // Move cursor to where VIC thinks it is
  wmove(win, mem.readByte(0xD6)+1, mem.readByte(0xD3)+1);
  wrefresh(win);
}



bool Hooks::handleKey(int ch) {
  bool doexit = false;
  switch(ch) {
    case 27: // Escape on Mac
      doexit = true;
      break;

    case 10:  // Newline on Mac
      mem.writeByte(0x277, 13);
      mem.writeByte(0xC6, 1);
      break;
    // write character to VIC20 keyboard buffer
    default:
      mem.writeByte(0x277, ch);
      mem.writeByte(0xC6, 1);
      // wprintw(win, "%d", ch);
      // wrefresh(win);
      break;
  }
  return doexit;
}


char Hooks::charToAscii(uint8_t charcode) {
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
