// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Memory abstraction for the 6502 emulator
///
/// There is support for reading/writing Bytes (8bits) and Words (16bits)
/// as well as for loading data and code into memory.
//===----------------------------------------------------------------------===//

#pragma once

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>


struct Snippet {
  uint16_t address;
  std::string name;
  std::vector<uint8_t> data;
};

class Memory {
public:
  uint8_t mem[65536];

  void clear() {
    memset(mem, 0, sizeof(mem));
  }

  // Clear memory and set program start address to 0x1000
  void reset() {
    clear();
    mem[0xfffc] = 0x00;
    mem[0xfffd] = 0x10;
  }

  void load(uint16_t address, std::vector<uint8_t> & program) {
    assert(address + program.size() < 65536);
    memcpy(mem + address, program.data(), program.size());
  }

  void loadSnippets(std::vector<Snippet> & snippets) {
    for (auto & snippet : snippets) {
      printf("Loading %5zu bytes @ 0x%04x - %s\n",
             snippet.data.size(), snippet.address, snippet.name.c_str());
      load(snippet.address, snippet.data);
    }
  }

  void dump(uint16_t address, uint16_t bytes) {
    printf("%04x: ", address);
    for (int i = 0; i < bytes; i++) {
      printf("0x%02x, ", mem[address + i]);
    }
    printf("\n");
  }

  uint8_t readByte(uint16_t address) {
    return mem[address];
  }

  void writeByte(uint16_t address, uint8_t value) {
    mem[address] = value;
  }

  uint16_t readWord(uint16_t address) {
    assert(address < 0xFFFF);
    return mem[address] + mem[address + 1] * 256;
  }

  void writeWord(uint16_t address, uint16_t value) {
    assert(address < 0xFFFF);
    mem[address] = value & 0xFF;
    mem[address + 1] = value >> 8;
  }
};
