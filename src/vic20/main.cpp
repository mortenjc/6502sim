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

  void printScreen(char * screen) {
    wclear(win);
    wmove(win, 0, 0);
    wprintw(win, "%s", screen);
    wrefresh(win);
  }

  bool getChar(int & read);
    WINDOW *win;
private:
  int lastch;
  int prior;
  int ch;

};

Window::Window(int X, int Y) {
  lastch = ERR;
  prior = ERR;
  initscr();
  (void) cbreak();		/* take input chars one at a time, no wait for \n */
  (void) noecho();		/* don't echo input */

  // printw("Typing any function key will disable it, but typing it twice in\n");
  // printw("a row will turn it back on (just for a demo).");
  refresh();
  //
  win = newwin(Y, X, 0, 0);
  nodelay(win, TRUE);
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

void getScreen2(WINDOW * win, int X, int Y) {
  char buffer[1024];
  for (int y = 0; y < Y; y++) {
    wmove(win, y, 0);
    int i = 0;
    for (int x = 0; x < X; x++) {
      uint16_t addr = 0x1000 + y*X + x;
      buffer[i++] = getChar(mem.readByte(addr));
    }
    buffer[i++] = 0;
    wprintw(win, "%s", buffer);
  }
}


int main(int argc, char *argv[]) {
  int ch;

  mem.loadBinaryFile("data/vic20rom.bin", 0x8000);
  cpu.reset(0x0000);
  //cpu.debugOn();
  //cpu.setTraceAddr(config.traceAddr);

  Window vic(23,24);
  auto win = vic.win;
  while (1) {
    cpu.clearInstructionCount();
    cpu.run(100000);

    getScreen2(win, 22,23);
    wmove(win, mem.readByte(0xD6), mem.readByte(0xD3));
    wrefresh(win);
    usleep(10000);


    // poll for keypress
    if (vic.getChar(ch)) {
      switch(ch) {
        case 27: // Escape on Mac
          return 0;
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
    }
  }

  return 0;
}
