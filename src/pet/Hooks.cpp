// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Implementation of hooks for keyboard and screen
///
//===----------------------------------------------------------------------===//

#include <ncurses.h>
#include <src/pet/Hooks.h>
#include <src/pet/gfx.h>
#include <fstream>
#include <streambuf>
#include <iostream>

std::string prgm =
"1000 REM STATE INITIALIZATION\n"
"1010 X=0\n"
"1020 Y=0\n"
"1030 DX=1\n"
"1040 DY=1\n"
"2000 REM BALL MOVEMENT\n"
"2010 POKE (1024 + Y*40 + X), 32\n"
"2020 X=X+DX\n"
"2030 Y=Y+DY\n"
"2040 IF X=40 THEN DX=-1\n"
"2050 IF X=40 THEN X=38\n"
"2060 IF X<0 THEN DX=1\n"
"2070 IF X<0 THEN X=1\n"
"2080 IF Y=25 THEN DY=-1\n"
"2090 IF Y=25 THEN Y=23\n"
"2100 IF Y<0 THEN DY=1\n"
"2110 IF Y<0 THEN Y=2\n"
"2200 POKE (1024 + Y*40 +X), 81\n"
"2210 GOTO 2000\n";


Hooks::Hooks(CPU & Cpu, Memory & memory, int X, int Y, bool Debug)
             : cpu(Cpu), mem(memory), Xres(X), Yres(Y)  {
  if (Debug)
    return;

  // X11 bitmap screen
  // Open a new window for drawing.
  gfxp = new GFX();
	gfxp->gfx_open((X-1) * 8, (Y-1) * 8, "6502 VIC2/Commodore64");

	// Set C64 colors
  gfxp->gfx_clear_color(0x35, 0x28, 0x79);
	gfxp->gfx_color(0x70,0xA4,0xB2);

  // ncurses window
  initscr();
  (void) cbreak();		// take input chars one at a time, no wait for \n
  //(void) noecho();		// don't echo input
  refresh();

  //win = newwin(Y+1, X+1, 0, 0);
  //win = newwin(0, 0, 0, 0);
  //box(win, '|', '-');
  nodelay(stdscr, TRUE); // don't 'hang' in getch()
  scrollok(stdscr, TRUE);
  //keypad(win, TRUE);
  //wmove(win, 0, 0);
}

Hooks::~Hooks() {
  endwin();
}

bool Hooks::getChar(int & ch) {
  ch = getch();
  if (ch == ERR) {
    return false;
  }
  return true;
}


void Hooks::printScreen(int X, int Y, uint16_t screenaddr, bool drawPixmap) {
  char buffer[1024];
  if (drawPixmap) {
    gfxp->gfx_clear();
  }

  for (int y = 0; y < Y; y++) {
    // wmove(win, y+1, 1);
    int i = 0;
    for (int x = 0; x < X; x++) {
      uint16_t addr = screenaddr + y*X + x;
      buffer[i++] = charToAscii(mem.readByte(addr));

      if (drawPixmap) {
        plotChar(mem.readByte(addr), x, y, 0x8000);
      }
    }
    buffer[i++] = 0;
    //wprintw(stdscr, "%s", buffer);
  }
  // Move cursor to where VIC thinks it is
  //wmove(stdscr, mem.readByte(0xD6)+1, mem.readByte(0xD3)+1);

  //wrefresh(win);
  if (drawPixmap) {
    gfxp->gfx_flush(); // bitmap screen
  }
}


void Hooks::plotChar(uint8_t ch, int X, int Y, uint16_t charromaddr) {
  uint16_t charaddr = charromaddr + 8 * ch; // addres of char's rom bitmap image

  uint16_t xoff = X * 8; // bits
  uint16_t yoff = Y * 8;

  for (int i = 0; i < 8; i++) {
    uint8_t line = mem.readByte(charaddr + i);
    if (line & 0x01)
      gfxp->gfx_point(xoff + 7, yoff + i);
    if (line & 0x02)
      gfxp->gfx_point(xoff + 6, yoff + i);
    if (line & 0x04)
      gfxp->gfx_point(xoff + 5, yoff + i);
    if (line & 0x08)
      gfxp->gfx_point(xoff + 4, yoff + i);
    if (line & 0x10)
      gfxp->gfx_point(xoff + 3, yoff + i);
    if (line & 0x20)
      gfxp->gfx_point(xoff + 2, yoff + i);
    if (line & 0x40)
      gfxp->gfx_point(xoff + 1, yoff + i);
    if (line & 0x80)
      gfxp->gfx_point(xoff + 0, yoff + i);
  }
}


// Typing a key amounts to write the key value to the buffer, then
// write the number of characters (1) in the buffer to 0xC6
void Hooks::typeKey(int key) {
  mem.writeByte(0x277, key);
  mem.writeByte(0xC6, 1);
}

void Hooks::load(std::string program) {
  for (int i = 0; i < program.size(); i++) {
    char ch = program[i];

    if (ch == '\n')
      typeKey(13);
    else
      typeKey(ch);

    cpu.clearInstructionCount();
    cpu.run(40000);
  }
}


void Hooks::loadFile() {
  std::string filename;
  wprintw(stdscr, "filename: ");
  wrefresh(stdscr);
  std::cin >> filename;
  wprintw(stdscr, "%s\n", filename.c_str());
  wrefresh(stdscr);
  std::ifstream t(filename);
  std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  load(str);
}

// These values work for my MacBook keyboard
bool Hooks::handleKey(int ch) {
  bool doexit = false;

  // wprintw(win, "%d\n", ch);
  // wrefresh(win);

  switch(ch) {
    case 261: // right arrow
      break;
    case 260: // left arrow
      break;
    case 259: // up arrow
      break;
    case 258: // down arrow
      break;
    case 127: // backspace
      typeKey(20);
      break;
    case 27: // escape
      doexit = true;
      break;
    case 10:  // enter
      typeKey(13);
      break;
    case 9: // tab
      loadFile(); // load c64 bouncing ball program
      break;

    default:
      typeKey(ch);
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
