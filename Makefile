

PROGS = sim6502 cputest

CFLAGS = -I . --std=c++11
TESTFLAGS = -I googletest/googletest/include/ -L googletest/build/lib -lgtest

all: $(PROGS)

gtest:
	./makegtest

cputest: CPUTest.cpp CPU.cpp CPUHelpers.cpp  CPU.h Memory.h
	g++ $(CFLAGS) $(TESTFLAGS) CPUTest.cpp CPU.cpp CPUHelpers.cpp   -o $@

sim6502: sim6502.cpp CPU.h CPU.cpp CPUHelpers.cpp Programs.h Memory.h Opcodes.h
	g++ $(CFLAGS) sim6502.cpp CPU.cpp CPUHelpers.cpp  -o $@

clean:
	rm -f $(PROGS) a.out

realclean: clean
	rm -fr googletest
