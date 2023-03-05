#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <sys/stat.h>
#include "solver.c"


// Some helpfull defines 
// ======================================================
#ifndef	boolean
    #define	boolean int
#endif
#ifndef	FALSE
    #define	FALSE (0)
#endif
#ifndef	TRUE
    #define	TRUE (!FALSE)
#endif
#ifndef	ABS
    #define ABS(a) (((a) < 0) ? -(a) : (a))
#endif
#ifndef	RAND_MAX
    #define RAND_MAX 0x7fff
#endif
#ifndef	MY_DBL_MIN
    #define MY_DBL_MIN -3.40282346638528859811704183484516924e+38F
#endif
#ifndef	MY_DBL_MAX
    #define MY_DBL_MAX 3.40282346638528859811704183484516925e+38F
#endif
    

// Neural Network Dimensions
// ======================================================
#define NUM_NODES_INPUT 100
#define NUM_NODES_HIDDEN 80 //best=48 for 1/0 5200 epochs 99.9 3 bad answers
#define NUM_NODES_OUTPUT 9

// Neural Network
// ======================================================
//numHiddenNodes
double hiddenLayer[NUM_NODES_HIDDEN]; 
//numOutputs
double outputLayer[NUM_NODES_OUTPUT]; 
//numHiddenNodes
double hiddenLayerBias[NUM_NODES_HIDDEN]; 
//numOutputs
double outputLayerBias[NUM_NODES_OUTPUT]; 
//numInputs numHiddenNodes
double hiddenWeights[NUM_NODES_INPUT][NUM_NODES_HIDDEN]; 
//numHiddenNodes numOutputs
double outputWeights[NUM_NODES_HIDDEN][NUM_NODES_OUTPUT]; 

// Evaluation Data
// ======================================================
// numTestSets numInputs
int evaldata[81][100];

// Helpers and functions
// ======================================================
// Activation function and its derivative for correction
// Activation
double sigmoid(double x) { return 1 / (1 + exp(-x)); }
// Feedback
double dSigmoid(double x) { return x * (1 - x); }

double activation_floor = 0.0;
double activation_scale = 1.0;
int feed[100];

int checkIfFileExists(const char* filename){
    struct stat buffer;
    int exist = stat(filename,&buffer);
    if(exist == 0)
        return 1;
    else  
        return 0;
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp) {
        if (a_delim == *tmp) {
            count++;
            last_comma = tmp;
        }
       tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result) {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);
        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    return result;
}

void loadData(int gridnum) {

    for (int i=0; i<81; i++) {
        for (int j=0; j<100; j++) {
            evaldata[i][j] = -1;
        }
    }
    const unsigned MAX_LENGTH = 1024;
    char filename[MAX_LENGTH];

    for (int box=0; box<81; box++) {
        int ret = snprintf(filename, sizeof filename, "../data/newgrid/SudokuGrid%i_png_%i.txt",gridnum, box);
        if (checkIfFileExists(filename)) { 
            FILE *fPtr = fopen( filename, "r" );

            if ( fPtr != NULL ) 
            {
                // File exists, so we can read data for that box
                // We expect 10 lines of 10 numbers ranging from 0 to 255 per files
                char buffer[MAX_LENGTH];
                int index = 0;
                for (int line=0; line<10; line++) {
                    fgets(buffer, MAX_LENGTH, fPtr);
                    char** tokens = str_split(buffer, '\t');
                    for (int col = 0; col < 10; col++) {
                        evaldata[box][index]=atof(*(tokens+col));
                        index++;
                    }
                }
            }
        	fclose( fPtr );
        }
    }
}

void loadNetwork() {
    FILE *fPtr = fopen( "../data/1to9_network_10x10_256_100_80_9_96.52_Sudoku.txt", "r" );
    if ( fPtr != NULL ) {
        // reading line by line, max 3048 bytes
        const unsigned MAX_LENGTH = 3048;
        char buffer[MAX_LENGTH];
        int block_count=0;
        int loop = 1;
        while (loop==1) {
            fgets(buffer, MAX_LENGTH, fPtr);
            if (buffer[0]!='#') {
                // It is a value, not a comment
                block_count++;
                if (block_count==1) {
                    // it is NUM_NODES_INPUT
                } else if (block_count==2) {
                    // it is NUM_NODES_HIDDEN
                } else if (block_count==3) {
                    // it is NUM_NODES_OUTPUT
                } else if (block_count==4) {
                    // it is ACTIVATION FLOOR
                    activation_floor =  atof(buffer);
                } else if (block_count==5) {
                    // it is ACTIVATION SCALE
                    activation_scale =  atof(buffer);
                } else if (block_count==6) {
                    // it is Hidden Layer
                    int t = 0;
                    char** tokens = str_split(buffer, '\t');
                    for (int j = 0; j < NUM_NODES_HIDDEN; j++) {
                        hiddenLayerBias[j] = atof(*(tokens+t)); t++;
                        for (int k = 0; k < NUM_NODES_INPUT; k++) {
                            hiddenWeights[k][j] = atof(*(tokens+t)); t++;
                        }
                        hiddenLayer[j] = atof(*(tokens+t)); t++;;
                        if (j<NUM_NODES_HIDDEN-1) {
                            int read = 1;
                            while (read==1) {
                                fgets(buffer, MAX_LENGTH, fPtr);
                                if ((buffer[0]!='#')||(buffer[0]==0)) read=0;
                            }
                            tokens = str_split(buffer, '\t');
                            t = 0;
                        }
                    }
                } else if (block_count==7) {
                    // it is Output Layer
                    int t = 0;
                    char** tokens = str_split(buffer, '\t');
                    for (int j = 0; j < NUM_NODES_OUTPUT; j++) {
                        outputLayerBias[j] = atof(*(tokens+t)); t++;;
                        for (int k = 0; k < NUM_NODES_HIDDEN; k++) {
                            outputWeights[k][j] = atof(*(tokens+t)); t++;;
                        }
                        outputLayer[j] = atof(*(tokens+t)); t++;;
                        if (j<NUM_NODES_OUTPUT-1) {
                            int read = 1;
                            while (read==1) {
                                fgets(buffer, MAX_LENGTH, fPtr);
                                if ((buffer[0]!='#')||(buffer[0]==0)) read=0;
                            }
                            tokens = str_split(buffer, '\t');
                            t = 0;
                        }
                    }
                    // We Stop here
                    loop = 0;
                }

            }
        }
    	fclose( fPtr );
	} else {
		printf( "Error- Unable to open network file\n" );
    }

}

char evaluate() {

    // FEED (Submission)
    // =====================================================

    // Compute hidden layer activation
    for (int j = 0; j < NUM_NODES_HIDDEN; j++) {
        double activation = hiddenLayerBias[j];
        for (int k = 0; k < NUM_NODES_INPUT; k++) {
            double v = feed[k]/255.0;
            activation += (v * hiddenWeights[k][j]);
        }
        activation = 0.1 + ((activation-activation_floor) * activation_scale);
        activation = activation / 10.0;
        hiddenLayer[j] = sigmoid(activation);
    }

    // Compute output layer activation
    for (int j = 0; j < NUM_NODES_OUTPUT; j++) {
        double activation = outputLayerBias[j];
        for (int k = 0; k < NUM_NODES_HIDDEN; k++) {
            activation += hiddenLayer[k] * outputWeights[k][j];
        }
        outputLayer[j] = sigmoid(activation);
    }

    // highest is the anwer
    int answer = -1;
    double highest = -1;
    for (int j = 0; j < NUM_NODES_OUTPUT; j++) {
        if (outputLayer[j]>highest) {
            highest = outputLayer[j];
            answer = j;
        }
    }

    //int answer = round(outputLayer[0]*10);

    return answer + 1;

}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Enter 1 valid path (./solver \"path_example\")");
        return 1;
    }

    loadNetwork();

    int c = strtol(argv[1], NULL, 10);
    loadData(c);

    int grid[9][9];
    for (int l=0; l<9; l++) for(int c=0; c<9; c++) grid[l][c] = 0;
    int box = 0;
    for (int l=0; l<9; l++) {
        for(int c=0; c<9; c++) {
            if (evaldata[box][0]!=-1) {
                for (int i=0; i<100; i++) feed[i] = evaldata[box][i];
                grid[l][c] = evaluate(feed);
            }
            box++;
        }
    }

    // Print Result
    for (int l=0; l<9; l++) {
        for(int c=0; c<9; c++) {
            printf(" %i ", grid[l][c]);
        }
        printf("\n");
    }

    sudoku_solver(grid);
    array_to_text(grid, argv[1]);

    printf("Check the result save with \"name.result\" !\n");
    return 0;
}
