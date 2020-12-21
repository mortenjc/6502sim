

PROGS = bin/sim6502 bin/cputest bin/branchtest

CFLAGS = -I src --std=c++11
TESTFLAGS = -I googletest/googletest/include/ -L googletest/build/lib -lgtest

COMMONINC = src/CPU.h  src/Programs.h src/Memory.h src/Opcodes.h src/Macros.h
COMMONSRC = src/CPU.cpp src/CPUInstructions.cpp src/CPUHelpers.cpp

TESTS = bin/cputest bin/branchtest

all: $(PROGS)

gtest:
	./scripts/makegtest

runtest: $(TESTS)
	./bin/cputest
	./bin/branchtest

bin/sim6502: src/sim6502.cpp $(COMMONSRC) $(COMMONINC)
	g++ $(CFLAGS) src/sim6502.cpp $(COMMONSRC) -o $@


# Tests
bin/cputest: test/CPUTest.cpp $(COMMONSRC) $(COMMONINC)
	g++ $(CFLAGS) $(TESTFLAGS) test/CPUTest.cpp $(COMMONSRC) -o $@

bin/branchtest: test/BranchJumpTest.cpp $(COMMONSRC) $(COMMONINC)
	g++ $(CFLAGS) $(TESTFLAGS) test/BranchJumpTest.cpp $(COMMONSRC) -o $@


clean:
	rm -f $(PROGS) a.out

realclean: clean
	rm -fr googletest
