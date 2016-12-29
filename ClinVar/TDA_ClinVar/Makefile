
.PHONY : all

INC = include
BIN = bin
SRC = src
OBJ = obj

CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -c -o4 -g -I$(INC)/

all : $(BIN)/principal

$(BIN)/principal : $(OBJ)/principal.o 
	$(CXX) -o $@ $<

$(OBJ)/principal.o : $(SRC)/principal.cpp $(SRC)/clinvar.cpp $(SRC)/mutacion.cpp $(SRC)/enfermedad.cpp $(INC)/mutacion.h $(INC)/enfermedad.h $(INC)/clinvar.h
	$(CXX) $(CXXFLAGS) $< -o $@
