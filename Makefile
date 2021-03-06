# Copyright (C) 2020 Morten Jagd Christensen, LICENSE: BSD2
#

PROGS = bin/c64 bin/vic20 bin/sim6502
TESTPROGS = bin/cputest bin/branchtest bin/ldatest bin/adctest bin/sbctest

CFLAGS = -O3 -I. -I src -I test --std=c++11
TESTFLAGS = -I googletest/googletest/include/
TESTLDFLAGS = -L googletest/build/lib -lgtest

COMMONINC = src/CPU.h  src/Programs.h src/Memory.h src/Opcodes.h
COMMONOBJ = build/CPU.o build/CPUInstructions.o build/CPUHelpers.o

PETOBJ = build/gfx.o build/Hooks.o
PETCFLAGS = -I/usr/X11R6/include
PETLDFLAGS =  -L/usr/X11R6/lib -lncurses -lX11 -lm


_dummy := $(shell mkdir -p build)

all: $(PROGS)

build/sim6502.o: src/sim6502.cpp $(COMMONINC) src/Config.h
	g++ $(CFLAGS) $< -c -o $@

build/CPU.o: src/CPU.cpp $(COMMONINC)
	g++ $(CFLAGS) $< -c -o $@

build/CPUInstructions.o: src/CPUInstructions.cpp $(COMMONINC)
	g++ $(CFLAGS) $< -c -o $@

build/CPUHelpers.o: src/CPUHelpers.cpp $(COMMONINC)
	g++ $(CFLAGS) $< -c -o $@

build/Hooks.o: src/pet/Hooks.cpp $(COMMONINC) src/pet/Hooks.h src/pet/gfx.h
	g++ $(CFLAGS) $(PETCFLAGS) $< -c -o $@

build/gfx.o: src/pet/gfx.cpp $(COMMONINC) src/pet/Hooks.h src/pet/gfx.h
	g++ $(CFLAGS) $(PETCFLAGS) $< -c -o $@

bin/sim6502: build/sim6502.o $(COMMONOBJ)
	g++ $(CFLAGS) build/sim6502.o $(COMMONOBJ) -o $@

bin/vic20: src/pet/vic20.cpp  src/pet/Hooks.h src/pet/gfx.h $(COMMONOBJ) $(PETOBJ)
	g++ $(CFLAGS) $(PETCFLAGS) src/pet/vic20.cpp $(COMMONOBJ) $(PETOBJ) $(PETLDFLAGS) -o $@

bin/c64: src/pet/comm64.cpp src/pet/Hooks.h src/pet/gfx.h $(COMMONOBJ) $(PETOBJ)
	g++ $(CFLAGS) $(PETCFLAGS) src/pet/comm64.cpp $(COMMONOBJ) $(PETOBJ) $(PETLDFLAGS) -o $@

# Test targets
test: $(TESTPROGS)

gtest:
	./scripts/makegtest

bin/cputest: test/CPUTest.cpp $(COMMONINC) test/TestBase.h
	g++ $(CFLAGS) $(TESTFLAGS) test/CPUTest.cpp $(COMMONOBJ) $(TESTLDFLAGS) -o $@

bin/branchtest: test/BranchJumpTest.cpp $(COMMONOBJ) $(COMMONINC) test/TestBase.h
	g++ $(CFLAGS) $(TESTFLAGS) test/BranchJumpTest.cpp $(COMMONOBJ) $(TESTLDFLAGS) -o $@

bin/ldatest: test/LDATest.cpp $(COMMONOBJ) $(COMMONINC) test/TestBase.h
	g++ $(CFLAGS) $(TESTFLAGS) test/LDATest.cpp $(COMMONOBJ) $(TESTLDFLAGS) -o $@

bin/adctest: test/ADCTest.cpp $(COMMONOBJ) $(COMMONINC) test/TestBase.h
	g++ $(CFLAGS) $(TESTFLAGS) test/ADCTest.cpp $(COMMONOBJ) $(TESTLDFLAGS) -o $@

bin/sbctest: test/SBCTest.cpp $(COMMONOBJ) $(COMMONINC) test/TestBase.h
	g++ $(CFLAGS) $(TESTFLAGS) test/SBCTest.cpp $(COMMONOBJ) $(TESTLDFLAGS) -o $@

runtest: $(TESTPROGS)
	for test in $(TESTPROGS); do ./$$test || exit 1; done



# Clean up
clean:
	rm -fr $(PROGS) $(TESTPROGS) a.out build

realclean: clean
	rm -fr googletest
