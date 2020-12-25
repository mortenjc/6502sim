# 6502sim
A 6502 CPU simulator project

All 6502 opcodes are implemented (see Opcodes.h)


## Building
Building the unit tests for the simulator requires google test.

    make gtest
    make

## Running
The main program is sim6502

You can currently choose between loading a binary image and booting it
at a specified address or running one of a few built-in programs. Program
execution starts at address 0x1000 unless changed with the **-b** option.
Debug print (disassembly) is enabled with the **-d** option. If not
enabled it can be enabled based on Program Counter (PC) value with **-t**.

    ./bin/sim6502 [-l filename] [-p program] [-b bootaddr] [-d] [-t traceaddr]

## Unit tests
A few unit tests have been created for the early bring-up and specific opcode
debugging.

    make runtest

## Functional tests
The simulator passes all the 6502 functional tests from
https://github.com/Klaus2m5/6502_65C02_functional_tests which tests all instructions
and addressing modes. The functional tests are GPL
We have included one binary file here (6502_functional_test.bin)
which is available under the GPL3 license.

To run the functional tests:

    ./bin/sim6502 -l data/6502_functional_test.bin -b 0x400 [-d]

This takes a long time with debug enabled but < 1s without on my macbook.


## Dependencies
CLI11 headers were included from https://github.com/CLIUtils/CLI11
Google tests can be cloned and built from Makefile
