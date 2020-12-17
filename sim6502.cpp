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
  //mem.loadSnippets(add_two_16_bit_numbers);
  mem.loadSnippets(fibonacci);

  cpu.reset();

  while (cpu.running ) {
    uint8_t instruction = cpu.getInstruction();
    cpu.handleInstruction(instruction);
  }

  //mem.dump(0x20, 6); // for add_two_16_bit_numbers

  return 0;
}
