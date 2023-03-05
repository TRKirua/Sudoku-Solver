# Makefile

CC = gcc

CPPFLAGS = -MMD
CFLAGS = -Wall `pkg-config  --cflags gtk+-3.0 sdl`
DEPS = solver.h sudoku_ai.h
LDLIBS = -lm `pkg-config --libs gtk+-3.0 sdl SDL_image`

solver : solver.o sudoku_ai.o
	$(CC) -o solver solver.o sudoku_ai.o

clean:
	rm -f *.o
	rm -f *.d
	rm -f ${DEP}
	rm -f ${PRG}
	rm -f *.result
	rm Sudokun

# END
