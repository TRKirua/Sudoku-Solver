./sudoku_ai
rm ../data/newgrid/*.txt
cp ../data/images/*.txt ../data/newgrid/
rm -r ../data/images/*.txt
./Sudokun $1
cat $1.result