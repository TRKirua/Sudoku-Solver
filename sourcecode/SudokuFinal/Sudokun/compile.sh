cd ../SudokuAI
rm -f *.o
rm -f *.d
rm sudoku_ai
make sudoku_ai
./sudoku_ai
wait 
rm ../data/newgrid/*.txt
cp ../data/images/*.txt ../data/newgrid/
rm -r ../data/images/*.txt
cd ../Sudokun 
rm -f *.result
rm Sudokun
rm -f *.o
rm -f *.d
make Sudokun 
./Sudokun $1 
cat $1.result
