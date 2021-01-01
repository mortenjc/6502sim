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
#include <pet/gfx.h>


class Hooks {
public:
  Hooks(Memory & memory, int X, int Y, bool Debug);

  ~Hooks();

  void printScreen(int X, int Y, uint16_t addr, bool drawPixmap);
  void plotChar(uint8_t ch, int X, int Y, uint16_t charaddr);

  bool getChar(int & read);
  bool handleKey(int ch);
  char charToAscii(uint8_t charcode);

  WINDOW *win;
private:
  int ch;
  Memory & mem;
  uint8_t Xres; // width in characters
  uint8_t Yres; // height in characters
  gfx::GFX * gfxp;
};
