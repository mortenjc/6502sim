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
  mem.dump(0x0020, 6);
}


int main(int argc, char * argv[])
{
  printf("implemented opcodes: %d\n", cpu.getNumOpcodes());
  mem.reset();
  cpu.reset();

  prgFibonacci();
  //prgSieve();
  //prgWeekday();
  //prgDiv32();

  return 0;
}
