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


  //mem.loadSnippets(add_two_16_bit_numbers);
  //mem.loadSnippets(fibonacci);
  mem.loadSnippets(weekday);

  cpu.reset();
  cpu.debugOn();
  //cpu.setBreakpointAddr(0x1517);
  //cpu.setBreakpointRegs(0xd0, 0x05, 0x43);
  cpu.run(-1);

  //mem.dump(0x20, 6); // for add_two_16_bit_numbers
  //mem.dump(0x00F0, 3); // for fibonacci
  //mem.dump(0x2000, 16); // for fibonacci
  return 0;
}
