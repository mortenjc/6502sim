// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Rudimentary C64 emulation
///
/// Based on the 6502 CPU simulator and available C64 ROMs
/// The user can write BASIC commands and backspace is supported. If
/// the DISPLAY environment is set then a bitmap screen is shown which supports
/// the graphical characters. If not, then a ncurses screen is used and
/// that can only show a limited number of characters and numbers.
//===----------------------------------------------------------------------===//

#include <CPU.h>
#include <pet/Hooks.h>

#ifdef Success
#undef Success
#include <CLI11/include/CLI/CLI.hpp>
#endif


int main(int argc, char *argv[]) {
  Memory mem;
  CPU cpu(mem);
  CLI::App app{"C64 Emulator"};

  bool Debug = false;
  app.add_flag("-d,--debug", Debug, "enable debug");
  CLI11_PARSE(app, argc, argv);
  int ch;

  mem.loadBinaryFile("src/pet/c64/kernal.901227-02.bin", 0xE000);
  mem.loadBinaryFile("src/pet/c64/c64_chars.bin", 0xD000);
  mem.loadBinaryFile("src/pet/c64/basic.901226-01.bin", 0xA000);
  mem.loadBinaryFile("src/pet/c64/c64_chars.bin", 0x8000);
  cpu.reset(0x0000);

  if (Debug)
    cpu.debugOn();
  //cpu.setTraceAddr(config.traceAddr);

  Hooks sys(cpu, mem, 41,26, Debug);
  int printscr = 5;
  while (1) {
    cpu.clearInstructionCount();
    cpu.run(10000);

    mem.writeByte(0xD012, 0); // video scanning

    if (not Debug) {
      printscr--;
      sys.printScreen(40, 25, 0x400, printscr == 0);
      if (printscr == 0)
        printscr = 5;
    }

    if (not Debug and sys.getChar(ch)) { // a key was pressed
      if (sys.handleKey(ch)) {
        return 0;
      }
    }
    usleep(10000);
  }
}
