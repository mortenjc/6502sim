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
#include <pet/Hooks.h>

Memory mem;
CPU cpu(mem);

int main(int argc, char *argv[]) {
  int ch;

  mem.loadBinaryFile("src/pet/vic20/vic20rom.bin", 0x8000);
  cpu.reset(0x0000);
  //cpu.debugOn();
  //cpu.setTraceAddr(config.traceAddr);

  Hooks vic(mem, 23,24);

  while (1) {
    cpu.clearInstructionCount();
    cpu.run(100000);

    vic.printScreen(22, 23, 0x1000);
    usleep(10000);

    if (vic.getChar(ch)) { // a key was pressed
      if (vic.handleKey(ch)) {
        return 0;
      }
    }
  }
}