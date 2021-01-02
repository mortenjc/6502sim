// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Hook for keyboard and screen interactions
///
//===----------------------------------------------------------------------===//

#pragma once 

#include <ncurses.h>
#include <CPU.h>
#include <Memory.h>
#include <pet/gfx.h>

class Hooks {
public:
  Hooks(CPU & cpu, Memory & memory, int X, int Y, bool Debug);

  ~Hooks();

  void printScreen(int X, int Y, uint16_t addr, bool drawPixmap);
  void plotChar(uint8_t ch, int X, int Y, uint16_t charaddr);

  void typeKey(int key);
  bool getChar(int & read);
  bool handleKey(int ch);
  char charToAscii(uint8_t charcode);
  void load(std::string program);


private:
  WINDOW *win;  // ncurses window
  Memory & mem;
  uint8_t Xres; // width in characters
  uint8_t Yres; // height in characters
  GFX * gfxp;   // ptr to bitmapped screen (X11)
  CPU & cpu;
};
