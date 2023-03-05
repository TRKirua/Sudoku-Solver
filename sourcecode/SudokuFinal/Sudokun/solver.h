#define SOLVER_H

#include <stdio.h>
#include <stdlib.h>

#define SIZE 9

void text_to_array(char* path, int grid[SIZE][SIZE]);
void array_to_text(int grid[SIZE][SIZE], char* path);
int sudoku_solver(int grid[SIZE][SIZE]);
int already_in_line(int x, int val, int grid[SIZE][SIZE]);
int already_in_column(int y, int val, int grid[SIZE][SIZE]);
int already_in_square(int x, int y, int val, int grid[SIZE][SIZE]);