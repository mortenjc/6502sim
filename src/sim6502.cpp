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
#include <Config.h>
#include <CPU.h>
#include <Memory.h>
#include <Programs.h>
#include <CLI11/include/CLI/CLI.hpp>

Memory mem;
CPU cpu(mem);

void prgFibonacci() {
    mem.loadSnippets(fibonacci32);
    cpu.run(-1);
    mem.dump(0x0028,  4); // result: largst fib below 2^32
}

void prgSieve() {
    mem.loadSnippets(sieve);
    cpu.run(-1);
    mem.dump(0x3000, 16); // Primes
    mem.dump(0x3010, 16);
    mem.dump(0x3020, 16);
}

void prgWeekday() {
  mem.loadSnippets(weekday);
  cpu.run(-1);
}

void prgDiv32() {
  mem.loadSnippets(div32);
  cpu.run(-1);
  mem.dump(0x0020, 2);
  mem.dump(0x0022, 4);
  mem.dump(0x0026, 1);
  mem.dump(0x0027, 1);
}

void functional() {
  printf("Starting functional tests.\n");
  printf("Success if loop is detected at PC 0x3469\n");
  mem.loadBinaryFile("data/6502_functional_test.bin", 0x0000);
  cpu.reset(0x400);
  cpu.setTraceAddr(0x3469);
  cpu.run(-1);
}

void selectProgram(Sim6502::Config & cfg) {
  switch (cfg.programIndex) {
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
    case 4:
      functional();
      break;
    default:
      printf("Program %d is not available\n", cfg.programIndex);
      break;
  }
}


int main(int argc, char * argv[])
{
  CLI::App app{"6502 Simulator"};
  Sim6502::Config config;

  app.add_option("-l,--load", config.filename, "load binary file into memory and run");
  app.add_option("-a,--laddr", config.loadAddr, "strt loading at address");
  app.add_option("-b,--boot", config.bootAddr, "set CPU Program Counter");
  app.add_option("-t,--trace", config.traceAddr, "enable debug at this PC address");
  app.add_option("-p,--program", config.programIndex, "choose program to run");
  app.add_flag("-d,--debug", config.debug, "enable debug");
  CLI11_PARSE(app, argc, argv);

  mem.reset();

  if (config.debug) {
    cpu.debugOn();
  }

  if (config.filename != "") {
    mem.loadBinaryFile(config.filename, config.loadAddr);
    cpu.reset(config.bootAddr);
    cpu.setTraceAddr(config.traceAddr);
    cpu.run(-1);
  } else {
    cpu.reset(0x1000);
    selectProgram(config);
  }

  //printf("CPU instructions: %llu\n", cpu.getInstructionCount());
  return 0;
}
