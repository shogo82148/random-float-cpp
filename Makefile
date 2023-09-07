.PHONY: help
help:
	cat Makefile

.PHONY: all
all: random

random: random.cpp
	$(CXX) $(CXXFLAGS) -Wall -Wextra -std=c++20 -O3 -o $@ $^
