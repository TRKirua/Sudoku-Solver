# Makefile

CC = gcc

CPPFLAGS = -MMD
CFLAGS = -Wall `pkg-config  --cflags gtk+-3.0 sdl`
DEPS = sudoku_ai.h
LDLIBS = -lm `pkg-config --libs gtk+-3.0 sdl SDL_image`

all: sudoku_ai

sudoku_ai: sudoku_ai.o

clean:
	rm -f *.o
	rm -f *.d
	rm -f ${DEP}
	rm -f ${PRG}
	rm sudoku_ai

# END