CXXFLAGS+=--std=c++0x `pkg-config --cflags suil-0` 

all: main

main: main.o
