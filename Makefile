CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Wpedantic

SOURCES = src/matrix.cpp src/syndrome.cpp src/gallager.cpp src/report.cpp
HEADERS = src/ldpc.h

.PHONY: all test clean

all: ldpc

ldpc: src/main.cpp $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) src/main.cpp $(SOURCES) -o $@

tests/run_tests: tests/tests.cpp $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -Isrc tests/tests.cpp $(SOURCES) -o $@

test: tests/run_tests
	./tests/run_tests

clean:
	rm -f ldpc tests/run_tests
