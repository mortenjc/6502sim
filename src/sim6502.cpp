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


int main(int argc, char * argv[])
{
  Memory mem;
  CPU cpu(mem);

  printf("implemented opcodes: %d\n", cpu.getNumOpcodes());
  mem.reset();


  //mem.loadSnippets(add32);
  //mem.loadSnippets(memcpy4);
  mem.loadSnippets(fibonacci32);
  //mem.loadSnippets(weekday);

  cpu.reset();
  cpu.debugOn();
  mem.dump(0x20, 8); // for add16/32
  mem.dump(0xF0, 4); // for add16/32
  //cpu.setBreakpointAddr(0x1517);
  //cpu.setBreakpointRegs(0xd0, 0x05, 0x43);
  cpu.run(-1);

  mem.dump(0x20, 12); // for add16/32
  //mem.dump(0x00F0, 3); // for fibonacci
  //mem.dump(0x2000, 16); // for fibonacci
  return 0;
}
