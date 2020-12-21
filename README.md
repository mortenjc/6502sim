# 6502sim
A 6502 CPU simulator project

So far only a subset (~80) of the opcodes are implemented (see Opcodes.h).
The simulator has support for jumps and branches, loads and stores,
increment and decrement, add and subtract with carry, set and clear flags,
stack push operations, register transfer, logial operators and more.

There is support for loading code into memory before execution (Memory.h and
sim6502.cpp) and google test.


## Building
Building the unit tests for the simulator requires google test.

    make gtest
    make

## Unit tests

    make runtest
