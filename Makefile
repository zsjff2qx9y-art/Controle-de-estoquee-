CXX ?= g++
CPPFLAGS := -Iinclude
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
SOURCES := $(wildcard src/*.cpp)
HEADERS := $(wildcard include/*.hpp)
TARGET := build/estoque

.PHONY: all run test clean
all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SOURCES) -o $@

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	python3 tests/test_integracao.py ./$(TARGET)

clean:
	rm -rf build
