# 6502sim
A 6502 CPU simulator project

So far only a subset (~90) of the opcodes are implemented (see Opcodes.h).
The simulator has support for jumps and branches, loads and stores,
increment and decrement, add and subtract with carry, set and clear flags,
stack push operations, register transfer, logial operators and more.

There is support for loading code into memory before execution (Memory.h and
sim6502.cpp) and google test.


## Building
Building the unit tests for the simulator requires google test.

    make gtest
    make

## Running
The main program is sim6502

You can currently choose between loading a binary image and booting it
at a specified address or running one of a few built-in programs.

    ./bin/sim6502 [-l filename] [-p program] [-b boot]

## Unit tests

    make runtest

## Functional tests
There is support for loading and executing functional tests based on
the macro assembler found here:
https://github.com/Klaus2m5/6502_65C02_functional_tests

To run them

    ./bin/sim6502 -l data/6502_functional_test.bin -b 0x400

Not all tests pass at the moment, but things are improving ;-)

## Dependencies
CLI11 headers were included from https://github.com/CLIUtils/CLI11
Google tests can be cloned and built from Makefile
