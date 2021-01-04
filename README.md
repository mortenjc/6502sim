# 6502sim
A 6502 CPU simulator (emulator?) project

All 6502 opcodes are implemented (see Opcodes.h)


## Building
Building the unit tests for the simulator requires google test.

    > make gtest
    > make

## Running
The main program is sim6502

You can currently choose between loading a binary image and booting it
at a specified address or running one of a few built-in programs. Program
execution starts at address 0x1000 unless changed with the **-b** option.
Debug print (disassembly) is enabled with the **-d** option. If not
enabled it can be enabled based on Program Counter (PC) value with **-t**.

    > ./bin/sim6502 [-l filename] [-p program] [-b bootaddr] [-d] [-t traceaddr]

## Unit tests
A few unit tests have been created for the early bring-up and specific opcode
debugging.

    > make runtest

## Functional tests
The simulator passes all the 6502 functional tests from
https://github.com/Klaus2m5/6502_65C02_functional_tests which tests all
instructions and addressing modes. The functional tests are GPL We have included
one binary file here (6502_functional_test.bin) which is available under the
GPL3 license.

To run the functional tests manually

    > ./bin/sim6502 -l data/6502_functional_test.bin -b 0x400 [-d]

or automatically:
    > ./bin/sim6502

This takes a long time with debug enabled but < 1s without on my MacBook.

Upon errors (currently there are none) the functional tests enters a loop which
is detected and causes the simulation to stop. The PC can then be inspected. Please
refer to
https://github.com/Klaus2m5/6502_65C02_functional_tests/blob/master/bin_files/6502_functional_test.lst
to identify which test is currently failing.

## VIC-20 and Commodore 64
Based on the roms in src/pet/vic20 and src/pet/c64 I was able to build bootable
rom images for the VIC-20 and COmmodore 64 computers. They *do* run but have very
primitive IO support.

The character-mode screen is rendered pretty much as it would be in the real system,
and the same holds for keyboard presses. But no attempt has been made for making
this CPU-cycle accurate. There is not support for other graphics modes or sound.

ROMs were downloaded from: http://www.zimmers.net/anonftp/pub/cbm/firmware/computers/

## Running
    > ./bin/c64
    > ./bin/vic20

![images/c64screen.png]

## Dependencies
CLI11 headers were included from https://github.com/CLIUtils/CLI11
Google tests can be cloned and built from Makefile
