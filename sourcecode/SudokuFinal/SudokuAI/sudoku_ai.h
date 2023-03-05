// SudokuIA.h : Include file for standard system include files,
// or project specific include files.

#include <gtk/gtk.h>

const int SAI_GRAY_MODE_MIN = 0;
const int SAY_GRAY_MODE_AVG = 1;
const int SAI_GRAY_MODE_MAX = 2;
const int SAI_IMAGE_SIZE = 460;

const int SAI_HISTO_MODE_ALL = 0;
const int SAI_HISTO_MODE_COLOR = 1;
const int SAI_HISTO_MODE_GRAY = 2;
const int SAI_BLACK_TRESHOLD_MAX = 200;

const float SAI_PERCENTILE_BLACK = 0.05;
const float SAI_PERCENTILE_WHITE = 0.05;

// 12% of black pixels and 88% of white pixels
// in a black and white image
const float SAI_RATIO_BW_PIXELS = 0.7;

typedef struct {
    int x;
    int y;
} SaiIntDot;

extern GtkWidget* sai_app_window;



// TODO: Reference additional headers your program requires here.
