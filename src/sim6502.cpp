// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief 6502 CPU emulator
///
/// A wrapper for CPU and Memory classes.
/// resets CPU and Memory, loads and executes programs
//===----------------------------------------------------------------------===//

#include <cstdio>
#include <CPU.h>
#include <Memory.h>
#include <Programs.h>
#include <CLI11/include/CLI/CLI.hpp>

Memory mem;
CPU cpu(mem);

void prgFibonacci() {
    cpu.debugOn();
    mem.loadSnippets(fibonacci32);
    cpu.run(-1);
    mem.dump(0x0028,  4); // fibonacci32
}

void prgSieve() {
    cpu.debugOn();
    mem.loadSnippets(sieve);
    cpu.run(-1);
    mem.dump(0x3000, 16); // sieve
    mem.dump(0x3010, 16);
    mem.dump(0x3020, 16);
}

void prgWeekday() {
  cpu.debugOn();
  mem.loadSnippets(weekday);
  cpu.run(-1);
}

void prgDiv32() {
  cpu.debugOn();
  mem.loadSnippets(div32);
  cpu.run(-1);
  mem.dump(0x0020, 2);
  mem.dump(0x0022, 4);
  mem.dump(0x0026, 1);
  mem.dump(0x0027, 1);
}

void selectProgram(int prog) {
  switch (prog) {
    case 0:
      prgFibonacci();
      break;
    case 1:
      prgSieve();
      break;
    case 2:
      prgWeekday();
      break;
    case 3:
      prgDiv32();
      break;
    default:
      printf("Program %d is not available\n", prog);
      break;
  }
}


int main(int argc, char * argv[])
{
  CLI::App app{"6502 Simulator"};

  int programIndex{0};
  uint16_t bootAddr{0x0000};
  std::string filename = "";
  bool interactive{false};
  app.add_option("-l,--load", filename, "load binary file into memory and run");
  app.add_option("-b,--boot", bootAddr, "set CPU Program Counter");
  app.add_option("-p,--program", programIndex, "choose program to run");

  CLI11_PARSE(app, argc, argv);

  printf("implemented opcodes: %d\n", cpu.getNumOpcodes());

  mem.reset();
  if (filename != "") {
    mem.loadBinaryFile(filename, 0x0000);
    mem.dump(0x00,  16);
    mem.dump(0x200, 16);
    mem.dump(0xFFFA, 6);
    cpu.reset(bootAddr);
    cpu.debugOn();
    cpu.run(-1);

  } else {
    selectProgram(programIndex);
  }

  return 0;
}
