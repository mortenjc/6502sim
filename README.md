# 6502sim
A 6502 CPU simulator project

So far only a subset of the opcodes are implemented (see Instructions.h) but 
some loads, stores, increments, decrements, set and clear flags, and branches
are supported. All three jumps (JSR, JMP, RTS) are implemented.

There is support for loading code (memory.h and si6502.cpp) and google
test.

## Building
Building the simulator requires google test.

    make gtest
    make

## Unit tests

    ./cputest
