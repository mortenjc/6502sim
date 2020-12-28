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

  mem.loadBinaryFile("src/pet/c64/c64.bin", 0xA000);
  cpu.reset(0x0000);
  //cpu.debugOn();
  //cpu.setTraceAddr(config.traceAddr);

  Hooks c64(mem, 41,26);
  while (1) {
    cpu.clearInstructionCount();
    cpu.run(100000);

    mem.writeByte(0xD012, 0);

    c64.printScreen(40, 25, 0x400);
    usleep(10000);

    if (c64.getChar(ch)) { // a key was pressed
      if (c64.handleKey(ch)) {
        return 0;
      }
    }
  }
}
