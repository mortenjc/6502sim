

PROGS = bin/sim6502
TESTPROGS = bin/cputest bin/branchtest

CFLAGS = -I src -I test --std=c++11
TESTFLAGS = -I googletest/googletest/include/ -L googletest/build/lib -lgtest

COMMONINC = src/CPU.h  src/Programs.h src/Memory.h src/Opcodes.h src/Macros.h
COMMONSRC = src/CPU.cpp src/CPUInstructions.cpp src/CPUHelpers.cpp

TESTS = bin/cputest bin/branchtest bin/ldatest

bin/sim6502: src/sim6502.cpp $(COMMONSRC) $(COMMONINC)
	g++ $(CFLAGS) src/sim6502.cpp $(COMMONSRC) -o $@

all: $(PROGS) $(TESTPROGS)


# Test targets
test: $(TESTPROGS)

gtest:
	./scripts/makegtest

bin/cputest: test/CPUTest.cpp $(COMMONSRC) $(COMMONINC)
	g++ $(CFLAGS) $(TESTFLAGS) test/CPUTest.cpp $(COMMONSRC) -o $@

bin/branchtest: test/BranchJumpTest.cpp $(COMMONSRC) $(COMMONINC)
	g++ $(CFLAGS) $(TESTFLAGS) test/BranchJumpTest.cpp $(COMMONSRC) -o $@

bin/ldatest: test/LDATest.cpp $(COMMONSRC) $(COMMONINC) test/TestBase.h
	g++ $(CFLAGS) $(TESTFLAGS) test/LDATest.cpp $(COMMONSRC) -o $@

runtest: test
	./bin/cputest
	./bin/branchtest


# Clean up
clean:
	rm -f $(PROGS) a.out

realclean: clean
	rm -fr googletest
