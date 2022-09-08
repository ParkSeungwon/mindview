CFLAG = -g -c -std=c++20 -fmax-errors=1 -fPIC
INCLUDE = -I../ -I/usr/include/jsoncpp
GTKCFLAG = $(shell pkg-config gtkmm-3.0 --cflags) #-I$(ROOT)
CC = g++
OBJ = $(patsubst %.cc, ../obj/%.o, $(wildcard *.cc))
MAIN_OBJ = $(patsubst %.cpp, ../obj/%.x, $(wildcard *.cpp))

all : $(OBJ) $(MAIN_OBJ)

../obj/%.o : %.cc 
	$(CC) $(GTKCFLAG) $(INCLUDE) $(CFLAG) $< -o $@

../obj/%.x : %.cpp
	$(CC) $(GTKCFLAG) $(INCLUDE) $(CFLAG) $< -o $@

