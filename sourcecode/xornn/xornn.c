#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Defines
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

// Neural Network Dimensions
// ======================================================
#define NUM_NODES_INPUT 2
#define NUM_NODES_HIDDEN 2
#define NUM_NODES_OUTPUT 1
#define NUM_TRAINING_SETS 4

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

// Training Data
// ======================================================
// 0 xor 0 = 0. 1 xor 0 - 1, 0 xor 1 = 1, 1 xor 1 = 0
// numTrainingSets numInputs
double training_inputs[NUM_TRAINING_SETS][NUM_NODES_INPUT] = 
    { {0.0f,0.0f},{1.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f} }; 
 // numTrainingSets  numOutputs
double training_outputs[NUM_TRAINING_SETS][NUM_NODES_OUTPUT] = 
    { {0.0f},{1.0f},{1.0f},{0.0f} };

// Activation function and its derivative for correction
// ======================================================
double sigmoid(double x) { return 1 / (1 + exp(-x)); }
// Feedback
double dSigmoid(double x) { return x * (1 - x); }

// Init all weights and biases between 0.0 and 1.0
double init_weight() { return ((double)rand())/((double)RAND_MAX); }

// Shuffle training set
// Example: Receive 0,1,2,3 => Return 2,1,0,3 (random order)
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

int main() {
    // Iterate through the entire training for a number of epochs
    // Epochs => One epoch is "completed" when one full training set 
    // has been submitted: in our case 4 submissions
    int epochs = 1000000;
    // Learning rate: [0,infini] => Recommended [0,0.5]
    double lr = 0.5;
    // When to Report: For example, when the number of Epochs submitted 
    // after the last report has reached this number
    int cyclesreport = 15000;
    // Epoch counter=> One epoch submitted, counter+1. Reset to 0 when 
    // cyclesreport is reached
    long cycles = 0;
    // Just to have an idea of the total workload to stabilize the NN
    int nbsubmissions = 0;
    // Just to have an idea of the impact of the training data
    // Generally 1/3 to reach the first good answer, 2/3 to refine NN
    int nbgoodanswers = 0;
    // How many submissions within the current cycle (n Epochs)
    int cyclesubmissions = 0;
    // How many evaluated as "good answers" within the current cycle (n Epochs)
    // To have an idea on how we are "currently" doing:
    // Cycle Percentage of success = cyclegoodanswers / cyclesubmissions
    int cyclegoodanswers = 0;
    // How to evaluate what is a good answer
    // The difference between the NN answer and the expecteed answer
    // If the difference < good_delta => This is a good answer
    double good_delta = 0.003;
    // To count how many "100" in a row we had
    int nb100percent = 0;
    // the nb of "100" in a row we need to reach to consider
    // the NN stabilized
    int nb100percent_inarow = 10;
    // If the network is stabilizer, let's stop
    boolean stop = FALSE;

    // Iniitialization of weights and biases
    for (int j = 0; j < NUM_NODES_HIDDEN; j++) {
        hiddenLayerBias[j] = init_weight();
        for (int k = 0; k < NUM_NODES_INPUT; k++) {
            hiddenWeights[k][j] = init_weight(); 
        }
        hiddenLayer[j] = 0;
    }
    for (int j = 0; j < NUM_NODES_OUTPUT; j++) {
        outputLayerBias[j] = init_weight();
        for (int k = 0; k < NUM_NODES_HIDDEN; k++) {
            outputWeights[k][j] = init_weight();
        }
        outputLayer[j]=0;
    }

    // Let's train the NN
    for (int n=0; n < epochs; n++) {
        // for good practices, shuffle the order of the training set
        int trainingSetOrder[] = {0, 1, 2, 3};
        shuffle(trainingSetOrder, NUM_TRAINING_SETS);
        // Cycle through each of the training set elements
        for (int x = 0; x < NUM_TRAINING_SETS; x++) {
            int i = trainingSetOrder[x];

            // FEED (Submission)
            // =====================================================

            // Compute hidden layer activation
            for (int j = 0; j < NUM_NODES_HIDDEN; j++) {
                double activation = hiddenLayerBias[j];
                for (int k = 0; k < NUM_NODES_INPUT; k++) {
                    activation += training_inputs[i][k] * hiddenWeights[k][j];
                }
                hiddenLayer[j] = sigmoid(activation);
            }
            nbsubmissions++;
            cyclesubmissions++;
            cycles++;

            // Compute output layer activation
            for (int j = 0; j < NUM_NODES_OUTPUT; j++) {
                double activation = outputLayerBias[j];
                for (int k = 0; k < NUM_NODES_HIDDEN; k++) {
                    activation += hiddenLayer[k] * outputWeights[k][j];
                }
                outputLayer[j] = sigmoid(activation);
            }

            // EVALUATION (NN Error (compare to expected answers))
            // =====================================================
            // and also Compute change in output weights
            double dError = 0;
            double deltaOutput[1]; // numOutputs
            int answer = TRUE;
            for (int j = 0; j < NUM_NODES_OUTPUT; j++) {
                dError = (training_outputs[i][j] - outputLayer[j]);
                if (ABS(training_outputs[i][j] - outputLayer[j])>good_delta) 
                    answer = FALSE;
                deltaOutput[j] = dError * dSigmoid(outputLayer[j]);
            }
            if (answer) {
                nbgoodanswers++;
                cyclegoodanswers++;
            }

            // FEEDBACK (Correction)
            // =====================================================
            // Compute change in hidden weights
            double deltaHidden[2]; //numHiddenNodes
            for (int j = 0; j < NUM_NODES_HIDDEN; j++) {
                double dError = 0.0f;
                for (int k = 0; k < NUM_NODES_OUTPUT; k++) {
                    dError += deltaOutput[k] * outputWeights[j][k];
                }
                deltaHidden[j] = dError * dSigmoid(hiddenLayer[j]);
            }

            // Apply change in output weights
            for (int j = 0; j < NUM_NODES_OUTPUT; j++) {
                outputLayerBias[j] += deltaOutput[j] * lr;
                for (int k = 0; k < NUM_NODES_HIDDEN; k++) {
                    outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j] * lr;
                }
            }

            // Apply change in hidden weights
            for (int j = 0; j < NUM_NODES_HIDDEN; j++) {
                hiddenLayerBias[j] += deltaHidden[j] * lr;
                for (int k = 0; k < NUM_NODES_INPUT; k++) {
                    hiddenWeights[k][j] += 
                        training_inputs[i][k] * deltaHidden[j] * lr;
                }
            }

            // REPORT (to not be too much into a black box) 
            // =====================================================
            if ((cycles>cyclesreport)||(nbsubmissions<50)) {
                int percent = (int)round(
                    100.0* ((1.0 * nbgoodanswers) / (1.0*nbsubmissions)));
                int cyclepercent = (int)round(
                    100.0* ((1.0 * cyclegoodanswers) / (1.0*cyclesubmissions)));
                if (cyclepercent>=100) {
                    nb100percent++;
                    if (nb100percent>=nb100percent_inarow) {
                        // We've got to our goal "in a row"
                        // Network is stabilized, no need to to continue
                        stop = TRUE;
                    }
                } else {
                    // We reset, since the "in a row" was not completed
                    nb100percent = 0;  
                }
                printf("Epoch #%i\t|\tlr=%f\t|\tSubmissions: %i\t|\t"
                    "Good Answers: %i\t|\tCycle: (%i /c)\t|\t"
                    "Total: (%i /c)\t|\t=> Error %f\n", 
                    n, lr, nbsubmissions, nbgoodanswers, 
                    cyclepercent, percent, dError);
                lr = 
                    ((1.0-((1.0 * cyclegoodanswers) / 
                        (1.0 * cyclesubmissions))));
                lr = lr / 2.0;
                if (lr<0.1) lr = 0.1;
                cyclesubmissions = 0;
                cyclegoodanswers = 0;
                cycles = 0;
            }

        }
        if (stop) {
            printf("We reached stabilization: Let's stop here!\n");
            break;
        }
       
    }

    // Interactive test:
    // ============================================
    stop = FALSE;
    while (!stop) {
        double test_inputs[NUM_NODES_INPUT];
        int n1;
        int n2;
        printf("\n");
        printf("Please, enter a pair of values to test: "
            "(ex: x,y with x and y being strickly 0 or 1)\n");
        scanf("%i,%i", &n1, &n2);
        test_inputs[0]=n1;
        test_inputs[1]=n2;
        printf("You want to test XOR function for x=%i and y=%i\n", n1, n2 );

        // NN Application
        // ================================================
        // Compute hidden layer activation
        for (int j = 0; j < NUM_NODES_HIDDEN; j++) {
            double activation = hiddenLayerBias[j];
            for (int k = 0; k < NUM_NODES_INPUT; k++) {
                activation += test_inputs[k] * hiddenWeights[k][j];
            }
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

        // Evaluation and Display
        // ===================================================
        double deltafalse = ABS(outputLayer[0]);
        double deltatrue = ABS(1.0 - outputLayer[0]);
        printf("\n");
        if (deltafalse<=good_delta) {
            printf("XOR Answer for %i,%i =  FALSE\n", n1, n2);
        } else if (deltatrue<=good_delta) {
            printf("XOR Answer for %i,%i =  TRUE\n", n1, n2);
        } else {
            printf("XOR Answer for %i,%i =  UNDETERMINED\n", n1, n2);
        }
        printf("\n");
        const char chr[10];
        printf("Continue? y/n: ");
        scanf("%s", chr);
        if (!((strcmp("y",chr)==0)||(strcmp("Y",chr)==0))) {
            stop = TRUE;
        }
    }

    return 0;
}