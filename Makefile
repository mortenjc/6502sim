

PROGS = bin/sim6502 bin/cputest bin/branchtest

CFLAGS = -I src --std=c++11
TESTFLAGS = -I googletest/googletest/include/ -L googletest/build/lib -lgtest

all: $(PROGS)

gtest:
	./scripts/makegtest


bin/sim6502: src/sim6502.cpp src/CPU.h src/CPU.cpp src/CPUHelpers.cpp src/Programs.h src/Memory.h src/Opcodes.h
	g++ $(CFLAGS) src/sim6502.cpp src/CPU.cpp src/CPUHelpers.cpp  -o $@


# Tests
bin/cputest: test/CPUTest.cpp src/CPU.cpp src/CPUHelpers.cpp  src/CPU.h src/Memory.h
	g++ $(CFLAGS) $(TESTFLAGS) test/CPUTest.cpp src/CPU.cpp src/CPUHelpers.cpp   -o $@

bin/branchtest: test/BranchJumpTest.cpp src/CPU.cpp src/CPUHelpers.cpp  src/CPU.h src/Memory.h
	g++ $(CFLAGS) $(TESTFLAGS) test/BranchJumpTest.cpp src/CPU.cpp src/CPUHelpers.cpp   -o $@


clean:
	rm -f $(PROGS) a.out

realclean: clean
	rm -fr googletest
