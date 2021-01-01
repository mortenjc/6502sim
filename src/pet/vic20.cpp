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

  mem.loadBinaryFile("src/pet/vic20/kernal.DKB_901486-07.bin", 0xE000);
  mem.loadBinaryFile("src/pet/vic20/vic20basic.bin", 0xC000);
  mem.loadBinaryFile("src/pet/vic20/characters.DK_901460-03.bin", 0x8000);
  cpu.reset(0x0000);
  //cpu.debugOn();
  //cpu.setTraceAddr(config.traceAddr);

  Hooks sys(mem, 23,24, false);

  while (1) {
    cpu.clearInstructionCount();
    cpu.run(100000);

    // just to align lines for vic20 and c64

    sys.printScreen(22, 23, 0x1000, true);
    usleep(10000);

    if (sys.getChar(ch)) { // a key was pressed
      if (sys.handleKey(ch)) {
        return 0;
      }
    }
  }
}
