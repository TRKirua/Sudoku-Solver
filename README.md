# Sudoku Solver
Development of OCR (Optical Character Recognition) software to solve a sudoku grid.\
\
To download our project :
```bash
wget https://github.com/TRKirua/Sudoku-Solver/archive/refs/heads/main.zip -O "SudokuSolver.zip" &&
unzip ./"SudokuSolver.zip" &&
rm ./"SudokuSolver.zip"
```

## Usage
To run the project :\
Go to: sourcecode/SudokuFinal/Sudokun \
\
Enter the command:
```bash
chmod u+x compile.sh
```
\
Then enter:
```bash
./compile <number of the image you want to use> #Your image must be named with this form "SudokuGrid<numberOfYourImage>"
```

The GTK interface should open.

Click on the Load button and select the image you wish to use (the one that matches the number you entered earlier).

If the chosen image is not oriented properly, please add the rotation angle in the field to the right of the Rotate button, then press the same button.

Click successively on the Proceed to Extraction, Create Digit Values and Create NN Values buttons.

Exit the interface by pressing the Quit button.

The result is available in:
sourcecode/SudokuFinal/data/images

## Clean project
To clean the project :
```bash
make clean
```

## Credits :
 * [Enes Ekici](https://github.com/TRKirua)
 * [Emon Barberis](https://github.com/EmonBar)
 * [Samy Abou-al-Tout](https://github.com/locovamos)
 * [Kylian Djermoune]()
