// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Rudimentary VIC-20 emulation
///
/// Based on the 6502 CPU simulator and available VIC20 ROMs
/// The user can write BASIC commands but no line editing capabilities
/// are yet available.
//===----------------------------------------------------------------------===//

#include <ncurses.h>
#include <CPU.h>
#include <Memory.h>
#include <vic20/vic20.h>

Memory mem;
CPU cpu(mem);

class Window {
public:
  Window(int X, int Y);

  ~Window();

  bool getChar(int & read);

  WINDOW *win;
private:
  int ch;

};

Window::Window(int X, int Y) {
  initscr();
  (void) cbreak();		// take input chars one at a time, no wait for \n
  (void) noecho();		// don't echo input
  refresh();

  win = newwin(Y, X, 0, 0);
  nodelay(win, TRUE); // don't 'hang' in getch()
  scrollok(win, TRUE);
  keypad(win, TRUE);
  wmove(win, 0, 0);
}


bool Window::getChar(int & ch) {
  ch = wgetch(win);
  if (ch == ERR) {
    return false;
  }

  return true;
}

Window::~Window() {
  endwin();
}

void printScreen(WINDOW * win, int X, int Y) {
  char buffer[1024];
  for (int y = 0; y < Y; y++) {
    wmove(win, y, 0);
    int i = 0;
    for (int x = 0; x < X; x++) {
      uint16_t addr = 0x0400 + y*X + x;
      buffer[i++] = getChar(mem.readByte(addr));
    }
    buffer[i++] = 0;
    wprintw(win, "%s", buffer);
  }
  // Move cursor to where VIC thinks it is
  wmove(win, mem.readByte(0xD6), mem.readByte(0xD3));
}


bool handleKey(int ch) {
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

int main(int argc, char *argv[]) {
  int ch;

  mem.loadBinaryFile("src/c64/roms/c64.bin", 0xA000);
  cpu.reset(0x0000);
  //cpu.debugOn();
  //cpu.setTraceAddr(config.traceAddr);

  Window vic(41,25);
  auto win = vic.win;
  while (1) {
    cpu.clearInstructionCount();
    cpu.run(100000);

    mem.writeByte(0xD012, 0);

    printScreen(win, 40,25);
    wrefresh(win);
    usleep(10000);

    if (vic.getChar(ch)) { // a key was pressed
      if (handleKey(ch)) {
        return 0;
      }
    }
  }
}
