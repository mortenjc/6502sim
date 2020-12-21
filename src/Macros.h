// Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Implementation of ASM macros
///
/// largely inspired by https://github.com/Klaus2m5/6502_65C02_functional_tests
//===----------------------------------------------------------------------===//

#pragma once

#define REPORT_ERROR 0x00, 0x90

#define trap JSR, REPORT_ERROR

#define trap_ne BEQ, 5,\
        trap

#define tst_x(arg1) \
            PHP, \
            CPXI, arg1, \
            trap_ne, \
            PLP
