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


  mem.loadBinaryFile("src/pet/vic20/kernal.DKB_901486-07.bin", 0xE000);
  mem.loadBinaryFile("src/pet/vic20/vic20basic.bin", 0xC000);
  mem.loadBinaryFile("src/pet/vic20/characters.DK_901460-03.bin", 0x8000);
  cpu.reset(0x0000);

  if (Debug)
    cpu.debugOn();
  //cpu.setTraceAddr(config.traceAddr);

  Hooks sys(cpu, mem, 23,24, Debug);
  int printscr = 5;
  while (1) {
    cpu.clearInstructionCount();
    cpu.run(10000);



    if (not Debug) {
      printscr--;
      sys.printScreen(22, 23, 0x1000, printscr == 0);
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
