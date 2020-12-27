// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Configurable options
///
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>
#include <string>

namespace Sim6502 {

class Config {
public:
  bool debug{false};          ///< trace on?
  int programIndex{0};        ///< which hardcoded program to run
  uint16_t loadAddr{0x0000};  ///< where to start loading
  uint16_t bootAddr{0x0000};  ///< where to start execution
  uint16_t traceAddr{0xFFFF}; ///< where to begin outputting trace
  std::string filename = "";  ///< for loading binary files
};

}
