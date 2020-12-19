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

  mem.reset();

  //mem.loadSnippets(ldxyi_and_dec);
  //mem.loadSnippets(countdown_y_from_10);
  //mem.loadSnippets(compare);
  //mem.loadSnippets(add_two_16_bit_numbers);
  //mem.loadSnippets(fibonacci);
  mem.loadSnippets(inc_stopif_greatereq);

  //mem.loadSnippets(sieve_of_eratosthenes);

  cpu.reset();
  cpu.debugOn();
  //cpu.setBreakpointAddr(0x1023);
  cpu.run(-1);

  //mem.dump(0x20, 6); // for add_two_16_bit_numbers
  //mem.dump(0x00F0, 3); // for fibonacci
  //mem.dump(0x2000, 16); // for fibonacci
  // mem.dump(0x2000, 16); // for sieve
  // mem.dump(0x20F0, 16); // for sieve
  // mem.dump(0x3000, 16); // for sieve
  // mem.dump(0x30F0, 16); // for sieve
  return 0;
}
