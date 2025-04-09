# Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall
SRC = calculator.cpp main.cpp
EXE = main.exe

all: clean $(EXE)

$(EXE):
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXE)

clean:
	rm -f $(EXE) *.o