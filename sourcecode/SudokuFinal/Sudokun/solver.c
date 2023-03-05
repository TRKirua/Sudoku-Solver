#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver.h"

// Convert text file to matrice
void text_to_array(char* path, int grid[SIZE][SIZE])
{
	FILE *fp = fopen(path, "r");

	if (fp == NULL)
	{
		printf("Error: file %s do not exist", path);
	}

	int x = 0;
	int y = 0;
	char c;

	while ((c = fgetc(fp)) != EOF)
	{
		if (y == 9)
		{
			x += 1;
			y = 0;
		}

		if (c == '.')
			y += 1;
		
		else
		{
			if (c <= '9' && c > '0')
			{
				grid[x][y] = c - '0';
				y += 1;
			}
		}
	}

	fclose(fp);
}



// Principal function for solver
int sudoku_solver(int grid[SIZE][SIZE])
{
	for(int i = 0; i < SIZE; i++)
	{
		for(int j = 0; j < SIZE; j++)
		{
			if (grid[i][j] == 0)
			{
				for(int k = 1; k < 10; k++)
				{
					if (already_in_line(i, k, grid) == 1)
					{
						if (already_in_column(j, k, grid) == 1)
						{
							if (already_in_square(i, j, k, grid) == 1)
							{
								grid[i][j] = k;
								if (sudoku_solver(grid) == 0)
									return 0;

								grid[i][j] = 0;
							}
						}
					}
				}

				return 1;
			}
		}
	}

	return 0;
}

// Is the number already in the line
int already_in_line(int x, int val, int grid[SIZE][SIZE])
{
	for(int i = 0; i < 9; i++)
	{
		int n = grid[x][i];
		if (n == val)
			return 0;
	}

	return 1;
}

// Is the number already in the column
int already_in_column(int y, int val, int grid[SIZE][SIZE])
{
	for(int j = 0; j < 9; j++)
	{
		int n = grid[j][y];
		if (n == val)
			return 0;
	}

	return 1;
}

// Is the number already in the square
int already_in_square(int x, int y, int val, int grid[SIZE][SIZE])
{
	x = 3 * (x/3);
	y = 3 * (y/3);

	for(int i = 0; i < 9; i++)
	{
		int xd = x + i/3;
		int yd = y + i/3;

		if (grid[xd][yd] != 0 && grid[xd][yd] == val)
			return 0;
	}

	return 1;
}


// Convert matrice to text file and save with name "path".result
void array_to_text(int grid[SIZE][SIZE], char* path)
{
	FILE *write;
	strcat(path, ".result");
	write = fopen(path, "a");

	int acc_rows = 0;
	int acc_rows2 = 0;
	int acc_cols = 0;

	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < 9; j++)
		{
			if (acc_cols == 3)
			{
				if (j != 8)
					fprintf(write, " ");
				acc_cols = 0;

			}

			if (acc_rows == 9)
			{
				fprintf(write, "\n");
				acc_rows = 0;
			}

			if (acc_rows2 == 27)
			{
				fprintf(write, "\n");
				acc_rows2 = 0;
			}

			acc_rows++;
			acc_rows2++;
			acc_cols++;

			fprintf(write, "%d", grid[i][j]);
		}
	}

	fprintf(write, "\n");
	fclose(write);
}