// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Hook for keyboard and screen interactions
///
//===----------------------------------------------------------------------===//

#include <ncurses.h>
#include <Memory.h>


class Hooks {
public:
  Hooks(Memory & memory, int X, int Y);

  ~Hooks();

  void printScreen(int X, int Y, uint16_t addr);

  bool getChar(int & read);
  bool handleKey(int ch);
  char charToAscii(uint8_t charcode);

  WINDOW *win;
private:
  int ch;
  Memory & mem;
};
