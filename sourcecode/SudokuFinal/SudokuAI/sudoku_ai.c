#include <stdio.h>
#include <math.h>
#include <cairo.h>
#include <glib.h>
#include <stdlib.h>
#include "sudoku_ai.h"


// To Compile:
/*
* Replace in the file build.ninja 
    of the out/build/Mingw64-Debug folder:
* build CMakeFiles/SudokuIA.dir/SudokuIA.c.obj: 
    C_COMPILER__SudokuIA_Debug ../../../SudokuIA.c 
    || cmake_object_order_depends_target_SudokuIA
  #DEFINES = -D-mms-bitfields -D-pthread
  DEFINES = -g -mms-bitfields -pthread -IC:/SudokuIA 
*/


GtkWidget* sai_app_window;
long sai_histogram_red[256];
long sai_histogram_green[256];
long sai_histogram_blue[256];
long sai_histogram_gray[256];
char* sai_filename = 
    "E:\\Perso\\share\\CLion\\CMake\\SudokuData\\SudokuGrid6.png";
const gchar* sai_rotate_text = NULL;
float sai_rotate_value = 0.0;
const gchar* sai_treshold_text = NULL;
float sai_treshold_value = 0.0;
guint sai_border_red = 255;
guint sai_border_green = 255;
guint sai_border_blue = 255;
int sai_crop_x1 = 0;
int sai_crop_y1 = 0;
int sai_crop_x2 = 0;
int sai_crop_y2 = 0;
int sai_crop_x3 = -1;
int sai_crop_y3 = -1;
int sai_crop_x4 = -1;
int sai_crop_y4 = -1;
int sai_width = 0;
int sai_height = 0;
int sai_selection = -1;
int sai_box_x1[81];
int sai_box_y1[81];
int sai_box_x2[81];
int sai_box_y2[81];

int grid_vert_y1=0;
int grid_vert_y2=0;
int grid_vert_x1=0;
int grid_vert_x2=0;

int grid_hori_y1=0;
int grid_hori_y2=0;
int grid_hori_x1=0;
int grid_hori_x2=0;
int sai_resized_box_w = 15;
int sai_resized_box_h = 15;
int sai_final_box_w = 10;
int sai_final_box_h = 10;
int sai_current_digit_x;
int sai_current_digit_y;
int sai_current_digit_w;
int sai_current_digit_h;

gboolean sai_apply_rotate = FALSE;
gboolean sai_box_value[81];

GtkWidget* sai_main_box = NULL;
GtkWidget* buttonsBox = NULL;

GtkWidget* imagesBoxLine1 = NULL;
GtkWidget* imagesBoxLine2 = NULL;

GtkWidget* imagesBoxBoxes1 = NULL;
GtkWidget* imagesBoxBoxes2 = NULL;
GtkWidget* imagesBoxBoxes3 = NULL;
GtkWidget* imagesBoxBoxes4 = NULL;
GtkWidget* imagesBoxBoxes5 = NULL;
GtkWidget* imagesBoxBoxes6 = NULL;
GtkWidget* imagesBoxBoxes7 = NULL;
GtkWidget* imagesBoxBoxes8 = NULL;
GtkWidget* imagesBoxBoxes9 = NULL;

GtkWidget* buttonQuit = NULL;
GtkWidget* buttonLoad = NULL;
GtkWidget* sai_btn_proceed_extract = NULL;
GtkWidget* sai_btn_digit_values = NULL;
GtkWidget* sai_btn_nn_values = NULL;
GtkWidget* button_rotate = NULL;
GtkWidget* entry_rotate = NULL;
GtkWidget* entry_treshold = NULL;

GtkWidget* image_color = NULL;
GdkPixbuf* image_original_pixbuf = NULL;
GdkPixbuf* image_color_pixbuf = NULL;
GtkWidget* image_percentile = NULL;
GdkPixbuf* image_percentile_pixbuf = NULL;
GtkWidget* image_white = NULL;
GdkPixbuf* image_white_pixbuf = NULL;
GtkWidget* image_gray = NULL;
GdkPixbuf* image_gray_pixbuf = NULL;
GtkWidget* image_bw = NULL;
GdkPixbuf* image_bw_pixbuf = NULL;
GtkWidget* image_border = NULL;
GdkPixbuf* image_border_pixbuf = NULL;
GtkWidget* image_crop = NULL;
GdkPixbuf* temporary_pixbuf = NULL;
GdkPixbuf* image_crop_pixbuf = NULL;
GtkDrawingArea* area_frame = NULL;
GtkWidget* sai_area_from_event_box = NULL;
GdkPixbuf* area_frame_pixbuf = NULL;

GdkPixbuf* image_proceed_pixbuf = NULL;

static void sai_do_persist_in_file(FILE* fptr, GdkPixbuf* pix_buf) {
    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf); 
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf); 
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf); 
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3; 
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf); 
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes); 

    // Read/Write Pointers to the image
    guchar* read_offset = pixels;

    // Scanning of the image
    int x;
    int y;
    char buffer[50];
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // read pixel info
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (has_alpha) a = *(read_offset++);
            int ret = snprintf(buffer, sizeof buffer, "%i", 255-r);
            fprintf(fptr, "%s\t", buffer);
        }
        fprintf(fptr, "\n");
        // jump to next pixels row start
        read_offset += row_remain;

    }
}

static void sai_do_findvertical(GdkPixbuf* pix_buf) {

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar gr; // gray value
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf); 
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf); 
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf); 
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3; 
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf); 
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes); 

    // Read/Write Pointers to the image
    guchar* read_offset = pixels;
    guchar* write_offset = pixels;
    guchar* magenta_write_offset = pixels;
    guchar* magenta_read_offset = pixels;

    // Scanning of the image
    int x;
    int y;
    float avg;
    int yl = h/2;
    int xl = 0;

    int stack_size = 50000;
    int stack_x[stack_size];     
    int stack_y[stack_size];     
    int stack_top = -1;

    // Selected Top Left of line
    int sx1 = 0;
    int sy1 = 0;
    // Selected Right of line
    int sx2 = 0;
    int sy2 = 0;

    int fill_xmin=-1;
    int fill_ymin=-1;
    int fill_xmax=w;
    int fill_ymax=h;

    read_offset = pixels + (yl * row_stride);
    for (xl = 0; xl < w; xl++) {
        // read pixel info
        write_offset = read_offset;
        r = *(read_offset++);
        g = *(read_offset++);
        b = *(read_offset++);
        if (has_alpha) a = *(read_offset++);
        if (r==0) {
            *(write_offset++) = 255;
            *(write_offset++) = 0;
            *(write_offset++) = 255;
            int mx = 0;
            stack_x[0] = xl;
            stack_y[0] = yl;
            stack_top = 0;
            // Top Left of line
            int x1 = xl;
            int y1 = yl;
            // Bottom Right of line
            int x2 = xl;
            int y2 = yl;
            while ((stack_top>=0)&&(stack_top<stack_size-1)) {
                // Pop
                x = stack_x[stack_top];
                y = stack_y[stack_top];
                stack_top--;
                if (x>x2) x2=x;
                if (x<x1) x1=x;
                if (y>y2) y2=y;
                if (y<y1) y1=y;
                // Look around to see if there are some white pixels
                for (int ys=y-1; ys <= y+1; ys++ ) {
                    for (int xs=x-1; xs <= x+1; xs++ ) {
                        if ((ys>fill_ymin)&&(ys<fill_ymax)&&(xs>fill_xmin)&&(xs<fill_xmax)) {
                            magenta_write_offset = pixels + (ys * row_stride) +
                                    (xs * pixel_size_in_bytes);
                            magenta_read_offset = magenta_write_offset;
                            r = *(magenta_read_offset++);
                            g = *(magenta_read_offset++);
                            b = *(magenta_read_offset++);
                            if (r==0) {
                                // It is a black pixel
                                // Push
                                if (stack_top<stack_size-1) {
                                    stack_top++;
                                    stack_x[stack_top] = xs;
                                    stack_y[stack_top] = ys;
                                    if (stack_top>mx) {
                                        mx = stack_top;
                                    }
                                }
                                *(magenta_write_offset++) = 255;
                                *(magenta_write_offset++) = 0;
                                *(magenta_write_offset++) = 255;
                            }
                        }
                    }
                }
            }

            // Here we've got the top left and bottom right of the line
            int sh = sy2-sy1;
            int sw = sx2-sx1;
            int ch = y2-y1;
            int cw = x2-x1;
            if (ch>sh) {
                sy1 = y1;
                sy2 = y2;
                sx2 = x2;
                sx1 = x1;
            } else if (ch==sh) {
                if (cw>sw) {
                    sy1 = y1;
                    sy2 = y2;
                    sx2 = x2;
                    sx1 = x1;
                }
            }
        }

    }
    grid_vert_y1=sy1;
    grid_vert_y2=sy2;
    grid_vert_x1=sx1;
    grid_vert_x2=sx2;
}

static void sai_do_findhorizontal(GdkPixbuf* pix_buf) {

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar gr; // gray value
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf); 
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf); 
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf); 
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3; 
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf); 
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes); 

    // Read/Write Pointers to the image
    guchar* read_offset = pixels;
    guchar* write_offset = pixels;
    guchar* magenta_write_offset = pixels;
    guchar* magenta_read_offset = pixels;

    // Scanning of the image
    int x;
    int y;
    float avg;
    int yl = 0;
    int xl = w/2;

    int stack_size = 50000;
    int stack_x[stack_size];     
    int stack_y[stack_size];     
    int stack_top = -1;

    // Selected Top Left of line
    int sx1 = 0;
    int sy1 = 0;
    // Selected Right of line
    int sx2 = 0;
    int sy2 = 0;

    int fill_xmin=-1;
    int fill_ymin=-1;
    int fill_xmax=w;
    int fill_ymax=h;

    for (yl = 0; yl < h; yl++) {
        // read pixel info
        read_offset = pixels + (yl * row_stride) + (xl * pixel_size_in_bytes);
        write_offset = read_offset;
        r = *(read_offset++);
        g = *(read_offset++);
        b = *(read_offset++);
        if (has_alpha) a = *(read_offset++);
        if (r==0) {
            *(write_offset++) = 255;
            *(write_offset++) = 0;
            *(write_offset++) = 255;
            int mx = 0;
            stack_x[0] = xl;
            stack_y[0] = yl;
            stack_top = 0;
            // Top Left of line
            int x1 = xl;
            int y1 = yl;
            // Bottom Right of line
            int x2 = xl;
            int y2 = yl;
            while ((stack_top>=0)&&(stack_top<stack_size-1)) {
                // Pop
                x = stack_x[stack_top];
                y = stack_y[stack_top];
                stack_top--;
                if (x>x2) x2=x;
                if (x<x1) x1=x;
                if (y>y2) y2=y;
                if (y<y1) y1=y;
                // Look around to see if there are some white pixels
                for (int ys=y-1; ys <= y+1; ys++ ) {
                    for (int xs=x-1; xs <= x+1; xs++ ) {
                        if ((ys>fill_ymin)&&(ys<fill_ymax)&&(xs>fill_xmin)&&(xs<fill_xmax)) {
                            magenta_write_offset = pixels + (ys * row_stride) +
                                    (xs * pixel_size_in_bytes);
                            magenta_read_offset = magenta_write_offset;
                            r = *(magenta_read_offset++);
                            g = *(magenta_read_offset++);
                            b = *(magenta_read_offset++);
                            if (r==0) {
                                // It is a black pixel
                                // Push
                                if (stack_top<stack_size-1) {
                                    stack_top++;
                                    stack_x[stack_top] = xs;
                                    stack_y[stack_top] = ys;
                                    if (stack_top>mx) {
                                        mx = stack_top;
                                    }
                                }
                                *(magenta_write_offset++) = 255;
                                *(magenta_write_offset++) = 0;
                                *(magenta_write_offset++) = 255;
                            }
                        }
                    }
                }
            }
            // Here we've got the top left and bottom right of the line
            int sh = sy2-sy1;
            int sw = sx2-sx1;
            int ch = y2-y1;
            int cw = x2-x1;
            if (cw>sw) {
                sy1 = y1;
                sy2 = y2;
                sx1 = x1;
                sx2 = x2;
            } else if (cw==sw) {
                if (ch>sh) {
                    sy1 = y1;
                    sy2 = y2;
                    sx1 = x1;
                    sx2 = x2;
                }
            }
        }

    }
    grid_hori_y1=sy1;
    grid_hori_y2=sy2;
    grid_hori_x1=sx1;
    grid_hori_x2=sx2;
}

static void sai_do_convolution(GdkPixbuf* source_pixbuf, 
    GdkPixbuf* destination_pixbuf, 
    float *mat_template, int mat_size, int filter ) {

    float mat[49];
    float* copy_mat = mat_template;
    int i = 0;
    // printf("Mat:\n");
    for (i=0; i<49; i++) {
        float f = (float)(*copy_mat);
        mat[i] = f;
        // printf("%f, ", mat[i]);
        copy_mat++;
    }
    // printf("\n");

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar gr; // gray value
    guchar* source_pixels;
    guchar* dest_pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(source_pixbuf);
    //width in pixels
    int w = gdk_pixbuf_get_width(source_pixbuf); 
    // Pointer on the pixels of the image
    source_pixels = gdk_pixbuf_get_pixels(source_pixbuf); 
    dest_pixels = gdk_pixbuf_get_pixels(destination_pixbuf); 
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(source_pixbuf); 
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3; 
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(source_pixbuf); 
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes); 

    // Read/Write Pointers to the image
    guchar* read_offset = source_pixels;
    guchar* write_offset = dest_pixels;

    // Scanning of the image
    int x;
    int y;
    float avg;
    float sr = 0;
    float sg = 0;
    float sb = 0;
    mat_size = 7;
    int n=0;
    int rel_mat_size = (int)floor((1.0*mat_size)/2.0); // for a matrix of 7
    for (y = 0; y < h; y++) {
        int ystart = y-rel_mat_size;
        if (ystart<0) ystart= 0;
        if (ystart>h-1) ystart= h-1;
        int yend =  y+rel_mat_size;
        if (yend<0) yend= 0;
        if (yend>h-1) yend= h-1;
        for (x = 0; x < w; x++) {
            int xstart = x-rel_mat_size;
            if (xstart<0) xstart= 0;
            if (xstart>w-1) xstart= w-1;
            int xend =  x+rel_mat_size;
            if (xend<0) xend= 0;
            if (xend>w-1) xend= w-1;
            sr = 0;
            sg = 0;
            sb = 0;
            float sf = 0;
            // Look around to see if there are some white pixels
            for (int ys=ystart; ys <= yend; ys++ ) {
                for (int xs=xstart; xs <= xend; xs++ ) {
                    read_offset = source_pixels + (ys * row_stride) +
                            (xs * pixel_size_in_bytes);
                    r = *(read_offset++);
                    g = *(read_offset++);   
                    b = *(read_offset++);
                    int ym = y-ys+rel_mat_size;
                    int xm = x-xs+rel_mat_size;
                    int pm = (ym*mat_size)+xm;
                    // n++;
                    // if (n<100) printf("N=%i, PM=%i, %f\n", n,pm, mat[pm] );
                    sr += r * mat[pm];
                    sg += g * mat[pm];
                    sb += b * mat[pm];
                    sf += mat[pm];
                }
            }
            int nr = (int)round(sr/sf);
            int ng = (int)round(sg/sf);
            int nb = (int)round(sb/sf);
            nr = (nr<0 ? nr = 0 : (nr>255 ? 255 : nr));
            ng = (ng<0 ? ng = 0 : (ng>255 ? 255 : ng));
            nb = (ng<0 ? nb = 0 : (nb>255 ? 255 : nb));
            r = (guchar)nr;
            g = (guchar)ng;
            b = (guchar)nb;
            // write result operation to the pixel
            write_offset = dest_pixels + (y * row_stride) +
                    (x * pixel_size_in_bytes);
            if (filter==0) {
                *(write_offset++) = r;
                *(write_offset++) = g;
                *(write_offset++) = b;
            } else if (filter>0) {
                if (r<filter) {
                    *(write_offset++) = r;
                    *(write_offset++) = g;
                    *(write_offset++) = b;
                } else {
                    *(write_offset++) = 255;
                    *(write_offset++) = 255;
                    *(write_offset++) = 255;
                }
            } else {
                if (r>-filter) {
                    *(write_offset++) = r;
                    *(write_offset++) = g;
                    *(write_offset++) = b;
                } else {
                    *(write_offset++) = 0;
                    *(write_offset++) = 0;
                    *(write_offset++) = 0;
                }
            }
            // we remove any transparency
            if (has_alpha) *(write_offset++) = 255; 

        }

    }
}


static void sai_do_bordercolor(GdkPixbuf* pix_buf) {

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar bw; // Black and White value
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;
    const int size = 400;
    long color_value[size];
    long color_occurance[size];
        

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf);
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf);
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf);
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3;
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf);
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes);

    // Read/Write Pointers to the image
    guchar* read_offset = pixels;
    guchar* write_offset = pixels;
    int x;
    int y;
    int ix;
    int iy;
    int in = 0;

    int i; for (i = 0; i < size; i++) {
        color_value[i] = 0;
        color_occurance[i] = 0;
    }

    int n = size/4;
    int ystep = h / n;
    int xstep = w / n;
    int yn = h / ystep;
    int xn = w / xstep;
    // Scanning of the image
    ix = 0;

    for (iy = 0; iy < yn-1; iy++) {

        y = ystep * iy + (ix);
        x = xstep + (xstep * ix);
        ix++;
        if (ix > 2) ix = 0;

        // Left side
        // =========
        read_offset = pixels;
        read_offset += y * row_stride;
        read_offset += x * pixel_size_in_bytes;
        r = *(read_offset++);
        g = *(read_offset++);
        b = *(read_offset++);
        float red = r;
        float green = g;
        float blue = b;
        /*
        red = 3.0 * round(red / 3.0);
        green = 3.0 * round(red / 3.0);
        blue = 3.0 * round(red / 3.0);
        */
        guint value = red * 256 * 256 + green * 256 + blue;
        // Do we have already this value?
        gboolean found = FALSE;
        for (i = 0; i < in; i++) {
            if (color_value[i] == value) {
                color_occurance[i]++;
                found = TRUE;
                break;
            }
        }
        if (found == FALSE) {
            color_value[in] = value;
            color_occurance[in] = 1;
            in++;
        }

        // Right side
        // ==========
        read_offset = pixels;
        read_offset += y * row_stride;
        read_offset += ((w * pixel_size_in_bytes) - (x * pixel_size_in_bytes));
        r = *(read_offset++);
        g = *(read_offset++);
        b = *(read_offset++);
        red = r;
        green = g;
        blue = b;
        red = 3.0 * round(red / 3.0);
        green = 3.0 * round(green / 3.0);
        blue = 3.0 * round(blue / 3.0);
        value = red * 256 * 256 + green * 256 + blue;
        // Do we have already this value?
        found = FALSE;
        for (i = 0; i < in; i++) {
            if (color_value[i] == value) {
                color_occurance[i]++;
                found = TRUE;
                break;
            }
        }
        if (found == FALSE) {
            color_value[in] = value;
            color_occurance[in] = 1;
            in++;
        }

    }

    iy = 0;
    for (ix = 0; ix < xn - 1; ix++) {

        x = xstep * ix + (iy);
        y = ystep + (ystep * iy);
        iy++;
        if (iy > 2) iy = 0;

        // Top side
        // =========
        read_offset = pixels;
        read_offset += y * row_stride;
        read_offset += x * pixel_size_in_bytes;
        r = *(read_offset++);
        g = *(read_offset++);
        b = *(read_offset++);
        float red = r;
        float green = g;
        float blue = b;
        red = 3.0 * round(red / 3.0);
        green = 3.0 * round(red / 3.0);
        blue = 3.0 * round(red / 3.0);
        guint value = red * 256 * 256 + green * 256 + blue;
        // Do we have already this value?
        gboolean found = FALSE;
        for (i = 0; i < in; i++) {
            if (color_value[i] == value) {
                color_occurance[i]++;
                found = TRUE;
                break;
            }
        }
        if (found == FALSE) {
            color_value[in] = value;
            color_occurance[in] = 1;
            in++;
        }

        // Bottom side
        // ==========
        read_offset = pixels;
        read_offset += (h-y-1) * row_stride;
        read_offset += x * pixel_size_in_bytes;
        r = *(read_offset++);
        g = *(read_offset++);
        b = *(read_offset++);
        red = r;
        green = g;
        blue = b;
        red = 3.0 * round(red / 3.0);
        green = 3.0 * round(green / 3.0);
        blue = 3.0 * round(blue / 3.0);
        value = red * 256 * 256 + green * 256 + blue;
        // Do we have already this value?
        found = FALSE;
        for (i = 0; i < in; i++) {
            if (color_value[i] == value) {
                color_occurance[i]++;
                found = TRUE;
                break;
            }
        }
        if (found == FALSE) {
            color_value[in] = value;
            color_occurance[in] = 1;
            in++;
        }

    }

    // Here we've got a sample of the border colors. 
    // Let's find the most represented
    guint max_occur = 0;
    guint max_index = 0;
    for (i = 0; i < in; i++) {
        if (color_occurance[i] > max_occur) {
            max_occur = color_occurance[i];
            max_index = i;
        }
    }

    guint value = color_value[max_index];
    sai_border_red = value / (256 * 256);
    value -= (sai_border_red * (256 * 256));
    sai_border_green = value / 256;
    value -= sai_border_green * 256;
    sai_border_blue = value;

}

static void sai_do_colors_to_grays(GdkPixbuf* pix_buf, 
    int gray_mode) {

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar gr; // gray value
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf); 
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf); 
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf); 
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3; 
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf); 
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes); 

    // Read/Write Pointers to the image
    guchar* read_offset = pixels;
    guchar* write_offset = pixels;

    // Scanning of the image
    int x;
    int y;
    float avg;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // read pixel info
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (has_alpha) a = *(read_offset++);

            // do the operation
            switch (gray_mode) {
            case 0: // SAI_GRAY_MODE_MIN
                gr = r;
                if ((b < r) && (b < g)) {
                    gr = b;
                }
                else if ((g < r) && (g < b)) {
                    gr = g;
                }
                break;
            case 1: // SAY_GRAY_MODE_AVG
                avg = (1.0 * r + g + b) / 3.0;
                avg = ((avg < 0) ? 0 : avg);
                avg = ((avg > 255) ? 255 : avg);
                gr = (guchar)avg;
                break;
            case 2: // SAI_GRAY_MODE_MAX
                gr = r;
                if ((b > r) && (b > g)) {
                    gr = b;
                }
                else if ((g > r) && (g > b)) {
                    gr = g;
                }
                break;
            }

            // write result operation to the pixel
            *(write_offset++) = gr;
            *(write_offset++) = gr;
            *(write_offset++) = gr;
            // we remove any transparency
            if (has_alpha) *(write_offset++) = 255; 

        }

        // jump to next pixels row start
        read_offset += row_remain;
        write_offset += row_remain;

    }
}

static void sai_do_clear_digit(GdkPixbuf* pix_buf) {

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar gr; // gray value
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;
    int white_limit = 245;

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf); 
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf); 
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf); 
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3; 
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf); 
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes); 

    // Read/Write Pointers to the image
    guchar* write_offset = pixels;
    guchar* read_offset = pixels;

    // Scanning of the image
    int x;
    int y;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // We let only the "Red" to say it is cleared
            // read pixel info
            r = *(read_offset);
            if (has_alpha) {
                read_offset+=4;
            } else {
                read_offset+=3;
            }
            if (r<white_limit) {
                // We let only the "Red" to say it is cleared
                write_offset++;
                *(write_offset++) = 0;
                *(write_offset++) = 0;
            } else {
                // We jump and let untouched
                // in short we let the whites
                write_offset += 3;
            }
            if (has_alpha) *(write_offset++) = 254; // We mark as non-digit
        }
        // jump to next pixels row start
        write_offset += row_remain;
    }

}


static void sai_do_rescale_to_256(GdkPixbuf* pix_buf) {

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar gr; // gray value
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf); 
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf); 
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf); 
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3; 
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf); 
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes); 

    // Read/Write Pointers to the image
    guchar* read_offset = pixels;
    guchar* write_offset = pixels;

    // Scanning of the image
    int x;
    int y;
    int min = 255;
    int max = 0;
    int gmax = 0;
    int white_limit = 245;
    int black_limit = 64;
    int digit_limit = 195; // meaning, if < there is a digit inside
    float black_count = 0;
    float avg = 0;
    float count = 0;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // read pixel info
            r = *(read_offset);
            if (has_alpha) {
                read_offset+=4;
            } else {
                read_offset+=3;
            }

            if (r<min) min = r;
            if (r>max)  max = r;
            if (r<white_limit) {
                if (r>gmax) gmax = r;
                avg += r;
                count++;
            }
            if (r<black_limit) {
                black_count++;
            }

        }
        // jump to next pixels row start
        read_offset += row_remain;
    }
    float factor = 1.0;
    if (max>min) {
        factor = (1.0*255) / (1.0*(max-min));
    } else {
        min = 0;
        max = 255;
    }
    float blk = 1.0;
    if (count==0) {
        avg = 255.0;
    }  else {
        avg = avg / count;
        blk = black_count / count;
    }
    g_print("Blk=\t%f\t\tAvg=\t%f\t\tFactor=\t%f\n", blk, avg, factor);
    if ( ( (avg>120) && (avg<digit_limit) ) || ( (blk<0.2) && (blk>0.005) ) ) {
        g_print("Proceed to Rescale 0-255\n");
        // reset
        read_offset = pixels;
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                // read pixel info
                r = *(read_offset);
                if (has_alpha) {
                    read_offset+=4;
                } else {
                    read_offset+=3;
                }
                gr = (int)round(factor * (r-min));
                if (gr<0) gr = 0;
                if (gr>255) gr = 255;
                // write result operation to the pixel
                *(write_offset++) = gr;
                *(write_offset++) = gr;
                *(write_offset++) = gr;
                // we remove any transparency
                if (has_alpha) *(write_offset++) = 255; 

            }

            // jump to next pixels row start
            read_offset += row_remain;
            write_offset += row_remain;
        }
    } else if ((avg>=digit_limit)||(blk<=0.005)) {
        // No digit, let's clear
        g_print("Proceed to Clear\n");
        // reset
        read_offset = pixels;
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                // read pixel info
                r = *(read_offset);
                if (has_alpha) {
                    read_offset+=4;
                } else {
                    read_offset+=3;
                }
                if (r<white_limit) {
                    // We let only the "Red" to say it is cleared
                    write_offset++;
                    *(write_offset++) = 0;
                    *(write_offset++) = 0;
                } else {
                    // We jump and let untouched
                    // in short we let the whites
                    write_offset += 3;
                }
                if (has_alpha) *(write_offset++) = 254; // We mark as non-digit
            }
            // jump to next pixels row start
            read_offset += row_remain;
            write_offset += row_remain;
        }
    } else {
        g_print("Remain as is\n");
    }

}

static void sai_do_balance_if_necessary(GdkPixbuf* pix_buf) {

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar gr; // gray value
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf); 
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf); 
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf); 
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3; 
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf); 
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes); 

    // Read/Write Pointers to the image
    guchar* read_offset = pixels;
    guchar* write_offset = pixels;

    // Scanning of the image
    int x;
    int y;
    int min = 255;
    int max = 0;
    int white_limit = 245;
    int gray_limit = 200;
    int black_limit = 64;
    int light_limit = black_limit*2;
    float black_count = 0;
    float avg = 0;
    float count = 0;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // read pixel info
            r = *(read_offset);
            if (has_alpha) {
                read_offset+=4;
            } else {
                read_offset+=3;
            }

            if (r<min) min = r;
            if (r<white_limit) {
                if (r>max)  max = r;
                avg += r;
                count++;
            }
            if (r<black_limit) {
                black_count++;
            }

        }
        // jump to next pixels row start
        read_offset += row_remain;
    }
    float factor = 1.0;
    float blk = 1.0;
    if ((count==0)||(max==min)) {
        max = white_limit;
        min = 0;
        avg = 255.0;
    }  else {
        factor = (1.0*gray_limit) / (1.0*(max-min));
        avg = avg / count;
        blk = black_count / count;
    }
    g_print("Blk=\t%f\t\tAvg=\t%f\t\tFactor=\t%f\n", blk, avg, factor);
    // 3 slices
    // blk>0 but <0.2 => Darken
    // Blk>=0.2 but <0.3 => Untouch
    // Blk>=0.3 => Lighten
    if ( ( (avg>120) && (avg<gray_limit) ) || ( (blk<0.2) && (blk>0.005) ) ) {
        // That means we do need a darken with a scaling
        g_print("Proceed to Darken\n");
        // reset
        read_offset = pixels;
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                // read pixel info
                r = *(read_offset);
                if (has_alpha) {
                    read_offset+=4;
                } else {
                    read_offset+=3;
                }
                if (r<white_limit) {
                    gr = (int)round(factor * (r-min));
                    if (gr<0) gr = 0;
                    if (gr>gray_limit) gr = gray_limit;
                    // write result operation to the pixel
                } else {
                    gr = 255;
                }
                *(write_offset++) = gr;
                *(write_offset++) = gr;
                *(write_offset++) = gr;
                // we remove any transparency
                if (has_alpha) *(write_offset++) = 255; 

            }

            // jump to next pixels row start
            read_offset += row_remain;
            write_offset += row_remain;

        }
    } else if ( (avg<light_limit) || (blk>=0.3) ) {
        // That means we do need a lighten with a scaling
        g_print("Proceed to Lighten\n");
        // reset
        float expand = (1.0 * gray_limit) / (1.0 * light_limit);
        float stretch = (1.0*(255-gray_limit)) / (1.0 * (255-light_limit));
        read_offset = pixels;
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                // read pixel info
                r = *(read_offset);
                if (has_alpha) {
                    read_offset+=4;
                } else {
                    read_offset+=3;
                }
                if (r<light_limit) {
                    gr = (int)round(expand * r);
                    if (gr<0) gr = 0;
                    if (gr>gray_limit) gr = gray_limit;
                    // write result operation to the pixel
                } else {
                    gr = gray_limit + (int)round(stretch * (r-light_limit));
                }
                *(write_offset++) = gr;
                *(write_offset++) = gr;
                *(write_offset++) = gr;
                // we remove any transparency
                if (has_alpha) *(write_offset++) = 255; 

            }

            // jump to next pixels row start
            read_offset += row_remain;
            write_offset += row_remain;

        }
    } else {
        g_print("Do not Proceed to Darken\n");
    }

}

static float sai_do_stddev(GdkPixbuf* source_pixbuf,     
    float *mat_template, int mat_size, float div ) {

    float mat[mat_size*mat_size];
    float* copy_mat = mat_template;
    int i = 0;
    // printf("Mat:\n");
    for (i=0; i<mat_size*mat_size; i++) {
        float f = (float)(*copy_mat);
        mat[i] = f;
        // printf("%f, ", mat[i]);
        copy_mat++;
    }
    // printf("\n");

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar gr; // gray value
    guchar* source_pixels;
    guchar* dest_pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(source_pixbuf);
    //width in pixels
    int w = gdk_pixbuf_get_width(source_pixbuf); 
    // Pointer on the pixels of the image
    source_pixels = gdk_pixbuf_get_pixels(source_pixbuf); 
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(source_pixbuf); 
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3; 
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(source_pixbuf); 
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes); 

    // Read/Write Pointers to the image
    guchar* read_offset = source_pixels;
    guchar* write_offset = dest_pixels;

    // Scanning of the image
    int x;
    int y;
    float avg;
    float sr = 0;
    float sg = 0;
    float sb = 0;
    float stddev = 0;
    int n=0;
    float count = 0;
    int rel_mat_size = (int)floor((1.0*mat_size)/2.0); // for a matrix of 7
    for (y = 0; y < h; y++) {
        int ystart = y-rel_mat_size;
        if (ystart<0) ystart= 0;
        if (ystart>h-1) ystart= h-1;
        int yend =  y+rel_mat_size;
        if (yend<0) yend= 0;
        if (yend>h-1) yend= h-1;
        for (x = 0; x < w; x++) {
            int xstart = x-rel_mat_size;
            if (xstart<0) xstart= 0;
            if (xstart>w-1) xstart= w-1;
            int xend =  x+rel_mat_size;
            if (xend<0) xend= 0;
            if (xend>w-1) xend= w-1;
            sr = 0;
            sg = 0;
            sb = 0;
            float sf = 0;
            // Look around to see if there are some white pixels
            for (int ys=ystart; ys <= yend; ys++ ) {
                for (int xs=xstart; xs <= xend; xs++ ) {
                    read_offset = source_pixels + (ys * row_stride) +
                            (xs * pixel_size_in_bytes);
                    r = *(read_offset++);
                    g = *(read_offset++);   
                    b = *(read_offset++);
                    int ym = y-ys+rel_mat_size;
                    int xm = x-xs+rel_mat_size;
                    int pm = (ym*mat_size)+xm;
                    // n++;
                    // if (n<100) printf("N=%i, PM=%i, %f\n", n,pm, mat[pm] );
                    float fact = mat[pm];
                    sr += r * fact;
                    sg += g * fact;
                    sb += b * fact;
                    n++;
                    sf += fact;
                }
            }
            float nr;
            float ng;
            float nb;
            nr = sr/div/255.0;
            ng = sg/div/255.0;
            nb = sb/div/255.0;
            float ts = 0;
            ts += (nr<0 ? nr = 0 : (nr>1.0 ? 1.0 : nr));
            ts += (ng<0 ? ng = 0 : (ng>1.0 ? 1.0 : ng));
            ts += (nb<0 ? nb = 0 : (nb>1.0 ? 1.0 : nb));
            stddev += ts / 3.0;
            count++;
        }
    }
    return stddev / count;
}


static void sai_do_colors_to_bw(GdkPixbuf* source_pixbuf) {

    GdkPixbuf* pix_buf = NULL;
    pix_buf = gdk_pixbuf_copy(source_pixbuf);
    gdk_pixbuf_copy_options(source_pixbuf, pix_buf);

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar bw; // Black and White value
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;
    
    guchar black_treshold = 127; // Anyway dynamically calculated later

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf);
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf);
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf);
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3;
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf);
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes);

    // Read/Write Pointers to the image
    guchar* read_offset = pixels;
    guchar* write_offset = pixels;
    int x;
    int y;

    // Scanning of the image
    guint histogram_cumul[256];
    float sum = 0;
    int i; for (i = 0; i < 256; i++) histogram_cumul[i] = 0;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // read pixel info
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (has_alpha) a = *(read_offset++);

            // do the operation
            // do the operation
            bw = r;
            if ((b > r) && (b > g)) {
                bw = b;
            }
            else if ((g > r) && (g > b)) {
                bw = g;
            }
            sum += bw;
            // we sum from there (bw) to 256 => this gives an idea
            // of the percentage of white pixels from there
            for (i = bw; i < 256; i++) histogram_cumul[i]++;

        }

        // jump to next pixels row start
        read_offset += row_remain;

    }

    // We then look for a certain percentage limit between 
    // future black and white pixels
    // let's consider that the black pixels should not represent
    // more than SAI_MAX_BLACK_PIXELS % below gray SAI_BLACK_TRESHOLD_MAX
    int avg = (int)round((sum) / (1.0*h*w));

    int median_value = (w*h)/2;
    int median_up =0;
    for (i = 0; i < 256; i++) {
        if (histogram_cumul[i] >= median_value) {
            median_up = i;
            break;
        }
    }
    int median_down =255;
    for (i = 255; i >=0; i--) {
        if (histogram_cumul[i] <= median_value) {
            median_down = i;
            break;
        }
    }
    
    float median = (1.0*median_up + median_down) / 2.0;
    black_treshold = (int)round((median*median * SAI_BLACK_TRESHOLD_MAX)  / 
        (255.0*255.0));
    black_treshold = (int)round((avg * black_treshold)  / (200.0));

    // Calculate the std dev:
    float vert_template[] = {
        -1,  -1,  -1,
        -1,   8,  -1,
        -1,  -1,  -1
    };
    float* mat_mem = malloc(sizeof(float) * 9);
    float* mat = mat_mem;
    for (int i =0; i<9; i++) {
        *mat = (float)vert_template[i];
        mat++;
    }
    mat = mat_mem;
    float stddev = sai_do_stddev(source_pixbuf, mat, 3, 1.0);
    free(mat_mem);

    // The one read in the text box
    // Below 0.4 or above 1.0, it should be calculated
    if ((sai_treshold_value<0.4)||(sai_treshold_value>1.0))  { 
        // Let' s calculate the sai_treshold_value
        // Empiric
        // We consider:
        //      3 Inflexion point on the Std Dev
        //          0.065, 0.13, 0.195
        //      1 Inflexion point on the black treshold
        //          96
        float V1 = 255.0-black_treshold;
        float V2 = V1 / 255.0;
        float V3 = stddev/0.13 * stddev/0.13 * stddev/0.13;
        float V4 = stddev - 0.065;
        float V5 = ABS(0.13 - stddev);
        float V6 = 0.195 - stddev;
        float V7 = ABS(V4 / 0.065);
        float V8 = ABS(V5 / 0.13);
        float V9 = ABS(V6 / 0.195);
        float V10 = V7*V8*V9;
        float V11 = V10 * sqrt(ABS(black_treshold-96));
        float V12 = V11 / 0.42;
        float V13 = V12 / V3;
        float V15 = 0.13 - stddev;
        float V16 = V15 - 0.04175;
        float V17 = 1.0;
        if (V16>=0) V17 = V16*5200.0; // (10000 * 4 * 0.13 )
        sai_treshold_value = V2+(V13/V17);
        g_print("Calculated SAI Treshold value: %f\n", sai_treshold_value);
        int sbreak = 1;
        if (sai_treshold_value<0.4) sai_treshold_value = 0.4;
        if (sai_treshold_value>1.0) sai_treshold_value = 1.0;

    }

    g_print("Pass #1 avg=%i => median up=%i\tdown=%i => black treshold=%i\t"
            "stddev=%f\ttreshold value=%f\n", 
        avg, median_up, median_down, black_treshold, 
        stddev, sai_treshold_value);
    float nb_of_black_pixels =
        (histogram_cumul[black_treshold] * sai_treshold_value);
    for (i = 0; i < 255; i++) {
        if (histogram_cumul[i] >= nb_of_black_pixels) {
            black_treshold = i;
            break;
        }
    }

    // Reset the read offset
    read_offset = pixels;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // read pixel info
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (has_alpha) a = *(read_offset++);

            // do the operation
            bw = r;
            if ((b > r) && (b > g)) {
                bw = b;
            }
            else if ((g > r) && (g > b)) {
                bw = g;
            }
            if (bw < black_treshold) {
                bw = 0;
            }
            else {
                bw = 255;
            }

            // write result operation to the pixel
            *(write_offset++) = bw;
            *(write_offset++) = bw;
            *(write_offset++) = bw;
            // we remove any transparency
            if (has_alpha) *(write_offset++) = 255;

        }

        // jump to next pixels row start
        read_offset += row_remain;
        write_offset += row_remain;

    }

    // Prepare Second pass
    // Identify large black areas
    GSList *areas_to_clear = NULL;
    int ntoKeep = 0;
    int stack_size = 50000;
    int stack_x[stack_size];     
    int stack_y[stack_size];     
    int stack_top = 0;
    int stack_max = 0;
    int px; int py;
    int small_areas_count = 0;
    for (py=0; py<h; py++) {
        for (px=0; px<w; px++) {
            read_offset = pixels + (py * row_stride) +
                    (px * pixel_size_in_bytes);
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (r+g+b==0+0+0) {
                // We have a Start
                stack_top = 0;
                stack_x[stack_top] = px;     
                stack_y[stack_top] = py;
                int fill_xmin=px;
                int fill_ymin=py;
                int fill_xmax=px;
                int fill_ymax=py;
                int nfill = 1;
                // We prepare a list of dots
                GSList *dotlist = NULL;
                // We create the first dot of the list
                SaiIntDot* firstdot = g_new(SaiIntDot,1);
                firstdot->x = px;
                firstdot->y = py;
                dotlist = g_slist_append(dotlist, firstdot);
                while ((stack_top>=0)&&(stack_top<stack_size-1)) {
                    // Pop
                    x = stack_x[stack_top];
                    y = stack_y[stack_top];
                    stack_top--;
                    write_offset = pixels + (y * row_stride) +
                            (x * pixel_size_in_bytes);
                    read_offset = write_offset;
                    r = *(read_offset++);
                    g = *(read_offset++);
                    b = *(read_offset++);
                    if (r+g+b==0+0+0) {
                        write_offset = pixels + (y * row_stride) +
                                (x * pixel_size_in_bytes);
                        *(write_offset++) = 255;
                        *(write_offset++) = 220;
                        *(write_offset++) = 255;
                        // we've got a new dot
                        if (x<fill_xmin) fill_xmin = x;
                        if (y<fill_ymin) fill_ymin = y;
                        if (x>fill_xmax) fill_xmax = x;
                        if (y>fill_ymax) fill_ymax = y;
                        SaiIntDot* newdot = g_new(SaiIntDot,1);
                        newdot->x = x;
                        newdot->y = y;
                        dotlist = g_slist_append(dotlist, newdot);
                        nfill++;
                    }
                    // Look around to see if there are some white pixels
                    for (int ys=y-1; ys <= y+1; ys++ ) {
                        for (int xs=x-1; xs <= x+1; xs++ ) {
                            if ((ys>=0)&&(ys<h)&&(xs>=0)&&(xs<w)) {
                                read_offset = pixels + (ys * row_stride) +
                                        (xs * pixel_size_in_bytes);
                                r = *(read_offset++);
                                g = *(read_offset++);
                                b = *(read_offset++);
                                if (r+g+b==0+0+0) {
                                    // It is a black pixel
                                    // Push
                                    if (stack_top<stack_size-1) {
                                        stack_top++;
                                        if (stack_top>stack_max) 
                                            stack_max = stack_top;
                                        stack_x[stack_top] = xs;
                                        stack_y[stack_top] = ys;
                                    }
                                }
                            }
                        }
                    }
                }
                gboolean tokeep = FALSE;
                // How many points in this list?
                if (nfill>2000) {
                    int area = (fill_xmax-fill_xmin) * (fill_ymax-fill_ymin);
                    float density = (1.0*nfill) / (1.0 *area);
                    if (density>0.4) {
                        tokeep = TRUE;
                    }
                } if (nfill<10) { 
                    // It is only few isolated dots,
                    // let's clean them too
                    small_areas_count++;
                    tokeep = TRUE;
                }
                if (!tokeep) {
                    // We free the dot list
                    g_slist_foreach(dotlist, (GFunc)g_free, NULL);
                    g_slist_free(dotlist);
                } else {
                    areas_to_clear = g_slist_append(areas_to_clear, dotlist);
                    ntoKeep++;
                }
            }
        }
    }
    
    g_print("Samll Areas Clearing:%i\n", small_areas_count);

    // Now we can clear the "black" areas... on the source pixbuf if any
    if (ntoKeep>0) {
        // Pointer on the pixels of the image
        pixels = gdk_pixbuf_get_pixels(source_pixbuf);
        GSList *area_iterator = NULL;
        GSList *dot_iterator = NULL;
        for (area_iterator = areas_to_clear; area_iterator; 
                area_iterator = area_iterator->next) {
            GSList* arealist = (GSList*)area_iterator->data;
            for (dot_iterator = arealist; dot_iterator; 
                    dot_iterator = dot_iterator->next) {
                SaiIntDot* pnt = (SaiIntDot*)dot_iterator->data;
                write_offset = pixels + (pnt->y * row_stride) +
                        (pnt->x * pixel_size_in_bytes);
                // *(write_offset++) = sai_border_red;
                // *(write_offset++) = sai_border_green;
                // *(write_offset++) = sai_border_blue;
                *(write_offset++) = 255;
                *(write_offset++) = 255;
                *(write_offset++) = 255;
            }
            // We free the area list
            g_slist_foreach(arealist, (GFunc)g_free, NULL);
            g_slist_free(arealist);
        }    
        // We free the areas to clear list
        g_slist_free(areas_to_clear);
    }


    g_print("Stack max:%i\n", stack_max);
    g_object_unref(pix_buf);

    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(source_pixbuf);
    read_offset = pixels;
    write_offset = pixels;

    // Scanning of the image
    sum = 0;
    for (i = 0; i < 256; i++) histogram_cumul[i] = 0;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // read pixel info
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (has_alpha) a = *(read_offset++);

            // do the operation
            // do the operation
            bw = r;
            if ((b > r) && (b > g)) {
                bw = b;
            }
            else if ((g > r) && (g > b)) {
                bw = g;
            }
            sum += bw;
            // we sum from there (bw) to 256 => this gives an idea
            // of the percentage of white pixels from there
            for (i = bw; i < 256; i++) histogram_cumul[i]++;

        }

        // jump to next pixels row start
        read_offset += row_remain;

    }

    avg = (int)round((sum) / (1.0*h*w));

    // We then look for a certain percentage limit between 
    // future black and white pixels
    // let's consider that the black pixels should not represent
    // more than SAI_MAX_BLACK_PIXELS % of the pixels below 
    // SAI_BLACK_TRESHOLD_MAX in gray
    median_value = (w*h)/2;
    median_up =0;
    for (i = 0; i < 256; i++) {
        if (histogram_cumul[i] >= median_value) {
            median_up = i;
            break;
        }
    }
    median_down =255;
    for (i = 255; i >=0; i--) {
        if (histogram_cumul[i] <= median_value) {
            median_down = i;
            break;
        }
    }
    median = (1.0*median_up + median_down) / 2.0;
    black_treshold = (int)round((median*median * SAI_BLACK_TRESHOLD_MAX)  / 
        (255.0*255.0));
    black_treshold = (int)round((avg * black_treshold)  / (200.0));
    g_print("Pass #2 avg=%i => median up=%i down=%i => black treshold=%i\n", 
        avg, median_up, median_down, black_treshold);
    nb_of_black_pixels =
        (histogram_cumul[black_treshold] * sai_treshold_value);
    for (i = 0; i < 256; i++) {
        if (histogram_cumul[i] >= nb_of_black_pixels) {
            black_treshold = i;
            break;
        }
    }

    // Reset the read offset
    read_offset = pixels;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // read pixel info
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (has_alpha) a = *(read_offset++);

            // do the operation
            bw = r;
            if ((b > r) && (b > g)) {
                bw = b;
            }
            else if ((g > r) && (g > b)) {
                bw = g;
            }
            if (bw < black_treshold) {
                bw = 0;
            }
            else {
                bw = 255;
            }

            // write result operation to the pixel
            *(write_offset++) = bw;
            *(write_offset++) = bw;
            *(write_offset++) = bw;
            // we remove any transparency
            if (has_alpha) *(write_offset++) = 255;

        }

        // jump to next pixels row start
        read_offset += row_remain;
        write_offset += row_remain;

    }



}

static void sai_do_white_balance(GdkPixbuf* pix_buf) {

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar bw; // Black and White value
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;


    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf);
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf);
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf);
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3;
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf);
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes);

    // Read/Write Pointers to the image
    guchar* read_offset = pixels;
    guchar* write_offset = pixels;

    // Scanning of the image
    int x;
    int y;
    float avg;
    guint white_treshold = 255;
    int anbofwhites = 0;
    float avg_r = 0.0;
    float avg_g = 0.0;
    float avg_b = 0.0;
    guint histogram_gray[256];
    int i; for (i = 0; i < 256; i++) histogram_gray[i] = 0;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {

            // read pixel info
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (has_alpha) a = *(read_offset++);

            // do the operation
            avg = (1.0 * r + g + b) / 3.0;
            avg = ((avg < 0) ? 0 : avg);
            avg = ((avg > 255) ? 255 : avg);
            histogram_gray[(int)avg]++;
        }

        // jump to next pixels row start
        read_offset += row_remain;

    }

    // Find the white treshold
    guint sum = 0;
    for (i = 255; i >= 0; i--) {
        sum += histogram_gray[i];
        if (sum > 1000) {
            white_treshold = (guchar)i;
            break;
        }
    }

    // If there is no balance to do, let's end here
    if (white_treshold == 255) return;

    // Reset the read offset
    read_offset = pixels;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {

            // read pixel info
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (has_alpha) a = *(read_offset++);

            // do the operation
            avg = (1.0 * r + g + b) / 3.0;
            avg = ((avg < 0) ? 0 : avg);
            avg = ((avg > 255) ? 255 : avg);
            if (avg > white_treshold) {
                avg_r += r;
                avg_g += g;
                avg_b += b;
                anbofwhites++;
            }

        }

        // jump to next pixels row start
        read_offset += row_remain;

    }

    // If there is no balance to do, let's end here
    if (anbofwhites == 0) return;

    // Do the math
    avg_r /= (1.0 * anbofwhites);
    avg_g /= (1.0 * anbofwhites);
    avg_b /= (1.0 * anbofwhites);
    avg = (avg_r + avg_g + avg_b) / 3.0;
    float dr = (255.0 - avg_r) / avg_r;
    float dg = (255.0 - avg_g) / avg_g;
    float db = (255.0 - avg_b) / avg_b;
    
    // Apply the white balance shift
    float v;
    // Reset the read offset
    read_offset = pixels;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // read pixel info
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (has_alpha) a = *(read_offset++);

            // do the operation
            v = r + r*dr;
            r = (guchar)((v > 255) ? 255 : ((v < 0) ? 0 : v));
            v = g + g * dg;
            g = (guchar)((v > 255) ? 255 : ((v < 0) ? 0 : v));
            v = b + b * db;
            b = (guchar)((v > 255) ? 255 : ((v < 0) ? 0 : v));

            // write result operation to the pixel
            *(write_offset++) = r;
            *(write_offset++) = g;
            *(write_offset++) = b;
            // we remove any transparency
            if (has_alpha) *(write_offset++) = 255;

        }

        // jump to next pixels row start
        read_offset += row_remain;
        write_offset += row_remain;

    }


}

static void sai_do_histograms(GdkPixbuf* pix_buf, int histogram_mode) {

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar bw; // Black and White value
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;


    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf);
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf);
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf);
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3;
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf);
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes);

    // Read/Write Pointers to the image
    guchar* read_offset = pixels;
    guchar* write_offset = pixels;
    int x;
    int y;

    int i; for (i = 0; i < 256; i++) {
        if ((histogram_mode == SAI_HISTO_MODE_ALL) ||
             (histogram_mode == SAI_HISTO_MODE_COLOR)) {
            sai_histogram_red[i] = 0;
            sai_histogram_green[i] = 0;
            sai_histogram_blue[i] = 0;
        }
        if ((histogram_mode == SAI_HISTO_MODE_ALL) || 
            (histogram_mode == SAI_HISTO_MODE_GRAY)) {
            sai_histogram_gray[i] = 0;
        }
    }

    // Scanning of the image
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // read pixel info
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (has_alpha) a = *(read_offset++);

            // do the operation
            if ((histogram_mode == SAI_HISTO_MODE_ALL) || 
                (histogram_mode == SAI_HISTO_MODE_COLOR)) {
                sai_histogram_red[r]++;
                sai_histogram_green[g]++;
                sai_histogram_blue[b]++;
            }
            if ((histogram_mode == SAI_HISTO_MODE_ALL) || 
                (histogram_mode == SAI_HISTO_MODE_GRAY)) {
                guint gr = (guint)round(1.0 * (r + g + b) / 3.0);
                sai_histogram_gray[gr]++;
            }
        }

        // jump to next pixels row start
        read_offset += row_remain;

    }

}

static void sai_do_percentile_contrast(GdkPixbuf* pix_buf,
    float blackPointPercent, float whitePointPercent) {

    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar bw; // Black and White value
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;


    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf);
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf);
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf);
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3;
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf);
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes);

    // Read/Write Pointers to the image
    guchar* read_offset = pixels;
    guchar* write_offset = pixels;
    int x;
    int y;

    // Get the total number of pixels in the image
    int numPixels = w * h;

    // Set the minimum intensity value of an image (0 = black)
    int minI = 0;

    // Get the total number of black pixels
    float blackPixels = numPixels * blackPointPercent;

    // Set a variable for summing up the pixels that will 
    // turn black
    int accum = 0;

    // Sum up the darkest shades until you reach the total of 
    // black pixels
    while (minI < 255)
    {
        accum += sai_histogram_gray[minI];
        if (accum > blackPixels) break;
        minI++;
    }

    // Set the maximum intensity of an image (255 = white)
    int maxI = 255;

    // Set the total number of white pixels
    float whitePixels = numPixels * whitePointPercent;

    //Reset the summing variable back to 0
    accum = 0;

    //Sum up the pixels that are the lightest which will turn white
    while (maxI > 0)
    {
        accum += sai_histogram_gray[maxI];
        if (accum > whitePixels) break;
        maxI--;
    }

    // Part of a normalization equation that doesn't vary with 
    // each pixel
    float spread = 255.0 / (maxI - minI);

    float v;
    // Write new pixel data to the image
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // read pixel info
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (has_alpha) a = *(read_offset++);

            // do the operation
            v = round((r - minI) * spread);
            r = (guchar)((v > 255) ? 255 : ((v < 0) ? 0 : v));
            v = round((g - minI) * spread);
            g = (guchar)((v > 255) ? 255 : ((v < 0) ? 0 : v));
            v = round((b - minI) * spread);
            b = (guchar)((v > 255) ? 255 : ((v < 0) ? 0 : v));

            // write result operation to the pixel
            *(write_offset++) = r;
            *(write_offset++) = g;
            *(write_offset++) = b;
            // we remove any transparency
            if (has_alpha) *(write_offset++) = 255;

        }

        // jump to next pixels row start
        read_offset += row_remain;
        write_offset += row_remain;

    }

}

static void sai_do_fill(GdkPixbuf* pix_buf, int boxnum,
        int x_start, int y_start, 
        int xmin, int ymin, int xmax, int ymax) {
    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar bw; // Black and White value
    guchar* pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;
    int stack_size = 50000;
    int stack_x[stack_size];     
    int stack_y[stack_size];     
    int stack_top = -1;
    int mx = 0;
    stack_x[0] = x_start;
    stack_y[0] = y_start;
    stack_top = 0;
    
    int nfill = 0;

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(pix_buf);
    //width in pixels
    int w = gdk_pixbuf_get_width(pix_buf);
    // Pointer on the pixels of the image
    pixels = gdk_pixbuf_get_pixels(pix_buf);
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(pix_buf);
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3;
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(pix_buf);
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes);

    // Read/Write Pointers to the image
    guchar* read_offset = pixels;
    guchar* write_offset = pixels;
    int x;
    int y;
    int fill_xmin=x_start;
    int fill_ymin=y_start;
    int fill_xmax=x_start;
    int fill_ymax=y_start;
    while ((stack_top>=0)&&(stack_top<stack_size-1)) {
        // Pop
        x = stack_x[stack_top];
        y = stack_y[stack_top];
        stack_top--;
        write_offset = pixels + (y * row_stride) +
                (x * pixel_size_in_bytes);
        read_offset = write_offset;
        r = *(read_offset++);
        g = *(read_offset++);
        b = *(read_offset++);
        if (r+g+b==255+255+255) {
            if (x<fill_xmin) fill_xmin = x;
            if (y<fill_ymin) fill_ymin = y;
            if (x>fill_xmax) fill_xmax = x;
            if (y>fill_ymax) fill_ymax = y;
            write_offset = pixels + (y * row_stride) +
                    (x * pixel_size_in_bytes);
            if (nfill<10) {
                *(write_offset++) = 180;
                *(write_offset++) = 0;
                *(write_offset++) = 0;
                nfill++;
            } else {
                *(write_offset++) = 255;
                *(write_offset++) = 220;
                *(write_offset++) = 255;
                nfill++;
            }
        }
        // Look around to see if there are some white pixels
        for (int ys=y-1; ys <= y+1; ys++ ) {
            for (int xs=x-1; xs <= x+1; xs++ ) {
                if ((ys>ymin)&&(ys<ymax)&&(xs>xmin)&&(xs<xmax)) {
                    read_offset = pixels + (ys * row_stride) +
                            (xs * pixel_size_in_bytes);
                    r = *(read_offset++);
                    g = *(read_offset++);
                    b = *(read_offset++);
                    if (r+g+b==255+255+255) {
                        // It is a white pixel
                        // Push
                        if (stack_top<stack_size-1) {
                            stack_top++;
                            stack_x[stack_top] = xs;
                            stack_y[stack_top] = ys;
                            if (stack_top>mx) {
                                mx = stack_top;
                            }
                        }
                    }
                }
            }
        }
    }

    // Let's check if these mins and maxs are really "delimiting white area"
    // since during the filling "some holes" may have been present
    // in the grid lines, and let pass the filling
    // Top
    gboolean is_ok = FALSE;
    int down_max = 3;
    int down_count = 0;
    while (!is_ok) {
        int fill_yes = 0;
        int fill_no = 0;
        for (x=fill_xmin; x<=fill_xmax; x++) {
            read_offset = pixels + (fill_ymin * row_stride) +
                (x * pixel_size_in_bytes);
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if ((r==180)||(b==220)) {
                // This is a color used for the fill
                fill_yes++;
            } else {
                // this is not a color used for the fill
                fill_no++;
            }
        }
        float r_fill = (1.0 * fill_yes) / (1.0*(fill_yes+fill_no));
        if (r_fill<0.4) {
            // let's move a bit "down" this limit
            if (fill_ymin<h-1){
                fill_ymin++;
                down_count++;
                if (down_count==down_max) is_ok = TRUE;
            } else {
                is_ok = TRUE;
            }
        } else {
            is_ok = TRUE;
        }
    }
    // Bottom
    is_ok = FALSE;
    int up_max = 3;
    int up_count = 0;
    while (!is_ok) {
        int fill_yes = 0;
        int fill_no = 0;
        for (x=fill_xmin; x<=fill_xmax; x++) {
            read_offset = pixels + (fill_ymax * row_stride) +
                (x * pixel_size_in_bytes);
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if ((r==180)||(b==220)) {
                // This is a color used for the fill
                fill_yes++;
            } else {
                // this is not a color used for the fill
                fill_no++;
            }
        }
        float r_fill = (1.0 * fill_yes) / (1.0*(fill_yes+fill_no));
        if (r_fill<0.4) {
            // let's move a bit "up" this limit
            if (fill_ymax>0) {
                fill_ymax--;
                up_count++;
                if (up_count==up_max) is_ok = TRUE;
            } else {
                is_ok = TRUE;
            }
        } else {
            is_ok = TRUE;
        }
    }
    // Left
    is_ok = FALSE;
    int left_max = 3;
    int left_count = 0;
    while (!is_ok) {
        int fill_yes = 0;
        int fill_no = 0;
        for (y=fill_ymin; y<=fill_ymax; y++) {
            read_offset = pixels + (y * row_stride) +
                (fill_xmin * pixel_size_in_bytes);
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if ((r==180)||(b==220)) {
                // This is a color used for the fill
                fill_yes++;
            } else {
                // this is not a color used for the fill
                fill_no++;
            }
        }
        float r_fill = (1.0 * fill_yes) / (1.0*(fill_yes+fill_no));
        if (r_fill<0.4) {
            // let's move a bit "left" this limit
            if (fill_xmin<w-1) {
                fill_xmin++;
                left_count++;
                if (left_count==left_max) is_ok = TRUE;
            } else {
                is_ok = TRUE;
            }
        } else {
            is_ok = TRUE;
        }
    }
    // Right
    is_ok = FALSE;
    int right_max = 3;
    int right_count = 0;
    while (!is_ok) {
        int fill_yes = 0;
        int fill_no = 0;
        for (y=fill_ymin; y<=fill_ymax; y++) {
            read_offset = pixels + (y * row_stride) +
                (fill_xmax * pixel_size_in_bytes);
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if ((r==180)||(b==220)) {
                // This is a color used for the fill
                fill_yes++;
            } else {
                // this is not a color used for the fill
                fill_no++;
            }
        }
        float r_fill = (1.0 * fill_yes) / (1.0*(fill_yes+fill_no));
        if (r_fill<0.4) {
            // let's move a bit "left" this limit
            if (fill_xmax>0) {
                fill_xmax--;
                right_count++;
                if (right_count==right_max) is_ok = TRUE;
            } else {
                is_ok = TRUE;
            }
        } else {
            is_ok = TRUE;
        }
    }

    sai_box_x1[boxnum] = fill_xmin;
    sai_box_y1[boxnum] = fill_ymin;
    sai_box_x2[boxnum] = fill_xmax;
    sai_box_y2[boxnum] = fill_ymax;
    int nb_pixels_if_white = (fill_xmax-fill_xmin) * (fill_ymax-fill_ymin);
    if (( nb_pixels_if_white>0)&&(nfill>0)) {
        float ratio = (1.0* nfill) / (1.0* nb_pixels_if_white);
        if (ratio>0.995) {
            sai_box_value[boxnum] = FALSE;
        } else {
            sai_box_value[boxnum] = TRUE;
        }
    } else {
        sai_box_value[boxnum] = FALSE;
    }
    // Draw the box for info
    if (sai_box_value[boxnum]==TRUE) {
        r = 0;
        g = 180;
        b = 0;
    } else {
        r = 180;
        g = 0;
        b = 0;
    }
    // Top & Bottom lines
    for (x=fill_xmin; x<=fill_xmax; x++) {
        write_offset = pixels + (fill_ymin * row_stride) +
            (x * pixel_size_in_bytes);
        *(write_offset++) = r;
        *(write_offset++) = g;
        *(write_offset++) = b;
        if (fill_ymin<h-1) {
            write_offset = pixels + ((fill_ymin+1) * row_stride) +
                (x * pixel_size_in_bytes);
            *(write_offset++) = r;
            *(write_offset++) = g;
            *(write_offset++) = b;
        }
        write_offset = pixels + (fill_ymax * row_stride) +
            (x * pixel_size_in_bytes);
        *(write_offset++) = r;
        *(write_offset++) = g;
        *(write_offset++) = b;
        if (fill_ymax>0) {
            write_offset = pixels + ((fill_ymax-1) * row_stride) +
                (x * pixel_size_in_bytes);
            *(write_offset++) = r;
            *(write_offset++) = g;
            *(write_offset++) = b;
        }
    }
    // Left & Right lines
    for (y=fill_ymin; y<=fill_ymax; y++) {
        write_offset = pixels + (y * row_stride) +
            (fill_xmin * pixel_size_in_bytes);
        *(write_offset++) = r;
        *(write_offset++) = g;
        *(write_offset++) = b;
        if (fill_ymin<w-1) {
            write_offset = pixels + (y * row_stride) +
                ((fill_xmin+1) * pixel_size_in_bytes);
            *(write_offset++) = r;
            *(write_offset++) = g;
            *(write_offset++) = b;
        }
        write_offset = pixels + (y * row_stride) +
            (fill_xmax * pixel_size_in_bytes);
        *(write_offset++) = r;
        *(write_offset++) = g;
        *(write_offset++) = b;
        if (fill_xmax>0) {
            write_offset = pixels + (y * row_stride) +
                ((fill_xmax-1) * pixel_size_in_bytes);
            *(write_offset++) = r;
            *(write_offset++) = g;
            *(write_offset++) = b;
        }
    }

}

static void sai_do_findborders(GdkPixbuf* source_pixbuf, 
        GdkPixbuf* dest_pixbuf) {
    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar bw; // Black and White value
    guchar* source_pixels;
    guchar* dest_pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(source_pixbuf);
    //width in pixels
    int w = gdk_pixbuf_get_width(source_pixbuf);
    // Pointer on the pixels of the image
    source_pixels = gdk_pixbuf_get_pixels(source_pixbuf);
    dest_pixels = gdk_pixbuf_get_pixels(dest_pixbuf);
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(source_pixbuf);
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3;
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(source_pixbuf);
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes);

    // Read/Write Pointers to the image
    guchar* read_offset = source_pixels;
    guchar* write_offset = dest_pixels;
    int x;
    int y;
    int left = 0;
    int right = 0;
    int top = 0;
    int bottom = 0;

    int hor_size = w / 4;
    long hor_positions[hor_size+1];
    int ver_size = h / 4;
    long ver_positions[ver_size+1];

    int i; 
    for (i = 0; i <= hor_size; i++) hor_positions[i] = 0;

    // Scanning of the image
    // Left side
    // =========
    read_offset = source_pixels;
    write_offset = dest_pixels;
    for (y = 0; y < h; y++) {
        gboolean stop = FALSE;
        for (x = 0; x < w; x++) {
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (has_alpha) a = *(read_offset++);
            if (r + g + b == 255 * 3) {
                *(write_offset++) = 255;
                *(write_offset++) = 0;
                *(write_offset++) = 0;
                if (has_alpha) *(write_offset++) = 255;
            }
            else {
                // We stop here
                int index = x / 4;
                hor_positions[index]++;
                stop = TRUE;
                break;
            }
        }
        if (stop == TRUE) {
            read_offset = source_pixels + (y * row_stride);
            write_offset = dest_pixels + (y * row_stride);
        } else {
            read_offset += row_remain;
            write_offset += row_remain;
        }
    }

    left = 0;
    long occur = 0;
    for (i = 0; i <= hor_size; i++) {
        if (hor_positions[i] > occur) {
            left = i;
            occur = hor_positions[i];
        }
    }
    left = (left * 4) + 2;
    if (left < 0) left = 0;

    // Right side
    // ==========
    for (i = 0; i <= hor_size; i++) hor_positions[i] = 0;
    read_offset = source_pixels + w * pixel_size_in_bytes-1;
    write_offset = dest_pixels + w * pixel_size_in_bytes-1;
    for (y = 0; y < h; y++) {
        gboolean stop = FALSE;
        for (x = w - 1; x >= 0; x--) {
            if (has_alpha) a = *(read_offset--);
            b = *(read_offset--);
            g = *(read_offset--);
            r = *(read_offset--);
            if (r + g + b == 255 * 3) {
                if (has_alpha) *(write_offset--) = 255;
                *(write_offset--) = 0;
                *(write_offset--) = 255;
                *(write_offset--) = 0;
            }
            else {
                // We stop here
                int index = x / 4;
                hor_positions[index]++;
                stop = TRUE;
                break;
            }
        }
        read_offset = source_pixels + (y * row_stride) +
            (w * pixel_size_in_bytes)-1;
        write_offset = dest_pixels + (y * row_stride) +
            (w * pixel_size_in_bytes)-1;
    }


    right = 0;
    occur = 0;
    for (i = 0; i <= hor_size; i++) {
        if (hor_positions[i] > occur) {
            right = i;
            occur = hor_positions[i];
        }
    }
    right = (right * 4) + 2;
    if (right > w-1) right = w-1;

    // Top side
    // ========
    for (i = 0; i <= ver_size; i++) ver_positions[i] = 0;
    for (x = left; x <= right; x++) {
        gboolean stop = FALSE;
        for (y = 0; y < h; y++) {
            read_offset = source_pixels + (y * row_stride) +
                (x * pixel_size_in_bytes);
            write_offset = dest_pixels + (y * row_stride) +
                (x * pixel_size_in_bytes);
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (r + g + b == 255 * 3) {
                *(write_offset++) = 0;
                *(write_offset++) = 0;
                *(write_offset++) = 255;
            }
            else {
                // We stop here
                int index = y / 4;
                ver_positions[index]++;
                stop = TRUE;
                break;
            }
        }
    }


    top = 0;
    occur = 0;
    for (i = 0; i <= ver_size; i++) {
        if (ver_positions[i] > occur) {
            top = i;
            occur = ver_positions[i];
        }
    }
    top = (top * 4) + 2;
    if (top < 0) top = 0;

    // Bottom side
    // ===========
    for (i = 0; i <= ver_size; i++) ver_positions[i] = 0;
    for (x = left; x <= right; x++) {
        gboolean stop = FALSE;
        if ((x > 121)&&(x<350)) {
            int z = 3;
        }
        for (y = h-1; y >= 0; y--) {
            read_offset = source_pixels + (y * row_stride) +
                (x * pixel_size_in_bytes);
            write_offset = dest_pixels + (y * row_stride) +
                (x * pixel_size_in_bytes);
            r = *(read_offset++);
            g = *(read_offset++);
            b = *(read_offset++);
            if (r + g + b == 255 * 3) {
                *(write_offset++) = 255;
                *(write_offset++) = 0;
                *(write_offset++) = 255;
            }
            else {
                // We stop here
                int index = y / 4;
                ver_positions[index]++;
                stop = TRUE;
                break;
            }
        }
    }


    bottom = 0;
    occur = 0;
    /*
    for (i = 0; i <= ver_size; i++) {
        g_print("%i:%i; ", i, ver_positions[i]);
    }
    */
    for (i = 0; i <= ver_size; i++) {
        if (ver_positions[i] > occur) {
            bottom = i;
            occur = ver_positions[i];
        }
    }
    bottom = (bottom * 4) + 2;
    if (bottom > h - 1) bottom = h - 1;

    sai_crop_x1 = 0;
    sai_crop_y1 = 0;
    int crop_w = right-left;
    int crop_h = bottom-top;
    if ((crop_w <= 0) || (crop_w > w)) crop_w = w;
    if ((crop_h <= 0) || (crop_h > h))  crop_h = w;
    float rxy = ABS(((1.0 * crop_w) - (1.0 * crop_h)) / (1.0 * crop_w));
    if (rxy > 0.05) {
        if (crop_h > crop_w) {
            if ((top == 0) && (bottom < h)) top = h - bottom;
            if ((top > 0) && (bottom == h)) bottom = h - top;
        } else {
            if ((left == 0) && (right < w)) left = w - right;
            if ((left > 0) && (right == w)) right = w - top;
        }
    }
    sai_crop_x1 = left;
    sai_crop_y1 = top;
    sai_crop_x2 = left;
    sai_crop_y2 = bottom;
    sai_crop_x3 = right;
    sai_crop_y3 = bottom;
    sai_crop_x4 = right;
    sai_crop_y4 = top;
    // Make it ready to be shown
    sai_selection = 3;

}

// We expect to receive only a 15x15 pixels image, in gray shades
// sai_resized_box_w, sai_resized_box_h
// that should contain in its center a representation of a digit
static float sai_do_digit_only_image(GdkPixbuf* source_pixbuf) {
    guchar r; // Red component
    guchar g; // Green component
    guchar b; // Blue component
    guchar a; // Alpha component
    guchar bw; // Black and White value
    guchar* source_pixels;
    gboolean has_alpha;
    guint pixel_size_in_bytes;
    guint row_stride;
    guint row_remain;

    // Image info
    // ==========
    //Height in pixels
    int h = gdk_pixbuf_get_height(source_pixbuf);
    //width in pixels
    int w = gdk_pixbuf_get_width(source_pixbuf);
    // Pointer on the pixels of the image
    source_pixels = gdk_pixbuf_get_pixels(source_pixbuf);
    // is it a 24 bit image (r,g,b) or a 32 bit image (r,g,b,a) ?
    has_alpha = gdk_pixbuf_get_has_alpha(source_pixbuf);
    // size of a pixel in bytes: r=1, g=1, b=1 (1+1+1=3), 
    //optionally a=1 (3+1=4). so it is either 4 or 3 
    //for 24/32 bits images
    pixel_size_in_bytes = has_alpha ? 4 : 3;
    // a row in the pixels map may not be a row in pixels 
    // (generally a multiple of 4). So if an image is 14 pixels, 
    // is stride may be 16 pixels
    row_stride = gdk_pixbuf_get_rowstride(source_pixbuf);
    // so, for a 14 pixels width, it means that 2 pixels 
    // (or 2 "values" of r,g,b(a)) are not part of the image,
    // so they "remain" and need to be skipped
    row_remain = row_stride - (w * pixel_size_in_bytes);

    // We will do a matrix of "weights" of the size of the image
    // Frames will represent the sum of their gray values (pixels on the frame)
    // Frames are "containing" the center (which may not be at frame center)
    // Frames may then be excentered
    // Frames may have their height and width different
    // Smallest Frame => W = 5 pixels, H = 7 pixels
    // Biggest => W = sai_resized_box_w pixels, H = sai_resized_box_h pixels
    // First Frame x=6, y=6, w/2=2, h/2=3
    // First Frame:"
    //      Left = 6-2 = 4;
    //      Top = 6-3 = 3;
    //      Right = 6+2 = 8;
    //      Bottom = 6+3 = 9;
    //      W = 4,5,6,7,8 = 5 pixels
    //      H = 3,4,5,6,7,8,9 = 7 pixels
    //     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 
    //    ╔═════════════════════════════╗
    //  0 ║                             ║
    //  1 ║                             ║
    //  2 ║                             ║
    //  3 ║             ░░██░░          ║
    //  4 ║           ████░░████        ║
    //  5 ║         ▓▓██      ██▓▓      ║
    //  6 ║           ██      ██        ║
    //  7 ║             ██████          ║
    //  8 ║           ░░██████░░        ║
    //  9 ║           ██      ██        ║
    // 10 ║         ▓▓██      ██▓▓      ║
    // 11 ║           ████░░████        ║
    // 12 ║             ░░██░░          ║
    // 13 ║                             ║
    // 14 ║                             ║
    //    ╚═════════════════════════════╝
    // In the case above, we need to find the "right final frame"
    // that encapsulate the "8", nad only the 8, but all the 8
    // Left = 5, Top=3, Right=11, Bottom=12
    // To do:
    //     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 
    //    ╔═════════════════════════════╗
    //  0 ║                             ║
    //  1 ║                             ║
    //  2 ║        ┌──────────────┐     ║
    //  3 ║        │    ░░██░░    │     ║
    //  4 ║        │  ████░░████  │     ║
    //  5 ║        │▓▓██      ██▓▓│     ║
    //  6 ║        │  ██      ██  │     ║
    //  7 ║        │    ██████    │     ║
    //  8 ║        │  ░░██████░░  │     ║
    //  9 ║        │  ██      ██  │     ║
    // 10 ║        │▓▓██      ██▓▓│     ║
    // 11 ║        │  ████░░████  │     ║
    // 12 ║        │    ░░██░░    │     ║
    // 13 ║        └──────────────┘     ║
    // 14 ║                             ║
    //    ╚═════════════════════════════╝    
    //
    // Horizontal variations: W/X
    // ==========================
    //  W varies from 5 (min) to 15 (max) = 11 times
    //  X, in function of W
    //  W=15 => X=0
    //  W=14 => X=0, X=1
    //  W=13 => X=0, X=1, X=2
    //  W=12 => X=0, X=1, X=2, X=3
    //  ....
    //  W=5 => X=0, X=1, X=2, X=3... X=10
    //  Horizontal Variations = (11-1)*(AVG(1+2+3+4+5+6+7+8+9+10))= 10*5.5 = 55
    // Veritcal variations: H/Y
    // ========================
    //  H varies from 7 (min) to 15 (max) = 9 times
    //  Y, in function of H
    //  H=15 => Y=0
    //  H=14 => Y=0, Y=1
    //  H=13 => Y=0, Y=1, Y=2
    //  H=12 => Y=0, Y=1, Y=2, Y=3
    //  ....
    //  H=7 => Y=0, Y=1, Y=2, Y=3... Y=8
    //  Vertical Variations = (9-1)*(AVG(1+2+3+4+5+6+7+8)) = 8*4.5 = 36
    int half_frame_w = 2;
    int half_frame_h = 3;
    int xframes = sai_resized_box_w-(half_frame_w*2+1);
    float avg = 0;
    int i=0;
    for (i = 0; i<xframes; i++) avg += (i+1);
    avg /= xframes;
    xframes = (int)round(xframes*avg);
    int yframes = sai_resized_box_h-(half_frame_h*2+1);
    avg = 0;
    for (i = 0; i<yframes; i++) avg += (i+1);
    avg /= yframes;
    yframes = (int)round(yframes*avg);
    float frame_weight[yframes*xframes];
    float frame_size[yframes*xframes];
    float frame_w[yframes*xframes];
    float frame_h[yframes*xframes];
    float frame_x[yframes*xframes];
    float frame_y[yframes*xframes];
    float frame_sorting[yframes*xframes];

    for (i = 0; i <= yframes*xframes; i++) {
        // We set it at its min possible value (all white)
        frame_weight[i] = 0;
        // Set current sorting
        frame_sorting[i] = i;
    }

    // Read/Write Pointers to the image
    guchar* read_offset = source_pixels;
    int x;
    int y;
    int dx;
    int dy;
    int xs;
    int ys;
    int xe;
    int ye;
    int xz;
    int yz;
    int left = 0;
    int right = 0;
    int top = 0;
    int bottom = 0;


    // Scanning of the image
    int frame_num = -1;
    float highest_weight = 0;
    int highest_frame = 0;
    read_offset = source_pixels;
    for (dy = half_frame_h; dy < sai_resized_box_h; dy++) { // y shift
        for (yz=0; yz<sai_resized_box_h; yz++) { // y zero
            ys = yz - dy; // y start
            ye = yz + dy; // y end
            if ((ys>=0) && (ye < sai_resized_box_h)) {
                // Here we are good, we have a valid Y Start and Y End
                for (dx = half_frame_h; dx < sai_resized_box_w; dx++) { // x shift
                    for (xz=0; xz<sai_resized_box_w; xz++) { // x zero
                        xs = xz - dx; // x start
                        xe = xz + dx; // x end
                        if ((xs>=0) && (xe < sai_resized_box_w)) {
                            // Here we are good, we have a valid X Start and X End
                            // This means it is a frame!
                            frame_w[frame_num] = xe-xs+1;
                            frame_h[frame_num] = ye-ys+1;
                            frame_x[frame_num] = xs;
                            frame_y[frame_num] = ys;
                            // Ok, then let's do the sum of the pixels
                            // the sum will be it's weight
                            // More it is white only, more the sum will be big
                            float sum = 0;
                            for (y=ys; y<=ye; y++) {
                                read_offset = source_pixels + (y * row_stride) +
                                    (xs * pixel_size_in_bytes);
                                for (x=xs; x<=xe; x++) {
                                    sum += 255 - *(read_offset);
                                    if (has_alpha) {
                                        read_offset+=4;
                                    } else {
                                        read_offset+=3;
                                    }
                                }
                            }
                            // if a 3x3 is all black, it will be the higest
                            // whilst, a 8x8 with 10 white pixels
                            // is going to be lower while we want it to be
                            // selected...
                            // Weight = Cubed Sum / max possible value
                            sum /= 255.0;
                            frame_weight[frame_num] = (1.0*sum*sum*sum*sum)/
                                (1.0*frame_w[frame_num]*frame_h[frame_num]);
                            if (frame_num==-1) {
                                highest_weight = frame_weight[frame_num];
                                highest_frame = frame_num;
                            } else if (frame_weight[frame_num]>=highest_weight) {
                                // In case of same weight, let's take 
                                // the biggest frame
                                if (frame_weight[frame_num]==highest_weight) {
                                    if (frame_w[frame_num]*frame_h[frame_num]>
                                            frame_w[highest_frame]*
                                            frame_h[highest_frame]) {
                                        highest_weight = frame_weight[frame_num];
                                        highest_frame = frame_num;
                                    }
                                } else {
                                    highest_weight = frame_weight[frame_num];
                                    highest_frame = frame_num;
                                }
                            }
                            frame_num++;
                        }
                    }
                }
            }
        }
    }
    // At this stage, we have all the frames and their weigths
    // and the selected frame
    sai_current_digit_x = frame_x[highest_frame];
    sai_current_digit_y = frame_y[highest_frame];
    sai_current_digit_w = frame_w[highest_frame];
    sai_current_digit_h = frame_h[highest_frame];
    
    int iBreak = 0;

    return frame_weight[highest_frame];

}


static GdkPixbuf* sai_do_rotate(GdkPixbuf* pix_buf, float angle, 
        guint r, guint g, guint b) {
    sai_apply_rotate = TRUE;
    cairo_matrix_t matrix;
    int h = gdk_pixbuf_get_height(pix_buf);
    int w = gdk_pixbuf_get_width(pix_buf);

    float red = (1.0 * r) / 255.0;
    float green = (1.0 * g) / 255.0;
    float blue = (1.0 * b) / 255.0;

    GdkWindow* gtk_window = gtk_widget_get_parent_window(sai_app_window);
    cairo_surface_t* surface = 
        gdk_cairo_surface_create_from_pixbuf(pix_buf, 0, NULL);
    cairo_t* cr = cairo_create(surface);

    cairo_save(cr);
    cairo_set_source_rgb(cr, red, green, blue);
    cairo_rectangle(cr, 0, 0, w, h);
    cairo_fill(cr);
    // Rotation around a given center x,y =>
    // Translate(x, y) * Rotate(theta) * Translate(-x, -y)
    // We deliberately choose base 1 instead of base 0, for easy reading
    float t1[4][4]; float rot[4][4]; float t2[4][4]; 
    float rt[4][4]; float rm[4][4];
    // Move center of image to 0,0
    t1[1][1] = 1.0;
    t1[1][2] = 0;
    t1[1][3] = 0;
    t1[2][1] = 0;
    t1[2][2] = 1.0;
    t1[2][3] = 0;
    t1[3][1] = -w/2;
    t1[3][2] = -h/2;
    t1[3][3] = 1.0;
    // Rotate
    rot[1][1] = cosf(angle);
    rot[1][2] = sinf(angle);
    rot[1][3] = 0;
    rot[2][1] = -sinf(angle);
    rot[2][2] = cosf(angle);
    rot[2][3] = 0;
    rot[3][1] = 0;
    rot[3][2] = 0;
    rot[3][3] = 1.0;
    // Place again to top left to 0,0
    t2[1][1] = 1.0;
    t2[1][2] = 0;
    t2[1][3] = 0;
    t2[2][1] = 0;
    t2[2][2] = 1.0;
    t2[2][3] = 0;
    t2[3][1] = +w/2;
    t2[3][2] = +h/2;
    t2[3][3] = 1.0;
    int i; int j; int k;
    // Rotate(theta, x,y) = Translate(x, y) * Rotate(theta)
    for(i=1;i<=3;i++) {    
        for(j=1;j<=3;j++) {    
            rt[i][j]=0;    
            for(k=1;k<=3;k++){    
                rt[i][j]+=t1[i][k]*rot[k][j];    
            }    
        }    
    }    
    // TanslateMatrix = Rotate(theta, x,y) * Translate(-x, -y)
    for(i=1;i<=3;i++) {    
        for(j=1;j<=3;j++) {    
            rm[i][j]=0;    
            for(k=1;k<=3;k++){    
                rm[i][j]+=rt[i][k]*t2[k][j];    
            }    
        }    
    }    
    cairo_matrix_init(&matrix, rm[1][1], rm[1][2], rm[2][1], 
        rm[2][2], rm[3][1], rm[3][2]);
    cairo_transform(cr, &matrix);
    gdk_cairo_set_source_pixbuf(cr, pix_buf, 0, 0);
    cairo_paint(cr);
    cairo_restore(cr);

    GdkPixbuf* the_return = gdk_pixbuf_get_from_surface(surface, 0, 0, w, h);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    return the_return;

}

gboolean draw_event_callback(GtkWidget* widget,
    GdkEventExpose* event,
    gpointer data)
{

    // "convert" the G*t*kWidget to G*d*kWindow (no, it's not a GtkWindow!)
    GdkWindow* window = gtk_widget_get_window(widget);

    cairo_region_t* cairoRegion = cairo_region_create();

    GdkDrawingContext* drawingContext;
    drawingContext = gdk_window_begin_draw_frame(window, cairoRegion);

    {
        // say: "I want to start drawing"
        cairo_t* cr = gdk_drawing_context_get_cairo_context(drawingContext);

        { // do your drawing
            cairo_save(cr);
            gdk_cairo_set_source_pixbuf(cr, area_frame_pixbuf, 0, 0);
            cairo_paint(cr);
            if (sai_selection > -1) {
                cairo_set_source_rgb(cr, 255, 0, 0);
                cairo_set_line_width(cr, 4.0);
                cairo_move_to(cr, sai_crop_x1, sai_crop_y1);
                int dx = ABS(sai_crop_x1 - sai_crop_x2);
                int dy = ABS(sai_crop_y1 - sai_crop_y2);
                float rx = 0;
                float ry = 0;
                if (dx > 0) rx = (1.0 * sai_width) / dx;
                if (dy > 0) ry = (1.0 * sai_height) / dy;
                if ((sai_selection == 0) && (rx < 3.0) && (ry < 3.0)) {
                    cairo_line_to(cr, sai_crop_x2, sai_crop_y1);
                    cairo_line_to(cr, sai_crop_x2, sai_crop_y2);
                    cairo_line_to(cr, sai_crop_x1, sai_crop_y2);
                    cairo_line_to(cr, sai_crop_x1, sai_crop_y1);
                }
                else {
                    if (sai_selection >= 0) {
                        cairo_line_to(cr, sai_crop_x2, sai_crop_y2);
                    }
                    if (sai_selection >= 1) {
                        cairo_line_to(cr, sai_crop_x3, sai_crop_y3);
                    }
                    if (sai_selection >= 2) {
                        cairo_line_to(cr, sai_crop_x4, sai_crop_y4);
                    }
                    if (sai_selection > 2) {
                        cairo_line_to(cr, sai_crop_x4, sai_crop_y4);
                        cairo_line_to(cr, sai_crop_x1, sai_crop_y1);
                    }
                }
                cairo_stroke(cr);
            }
            cairo_move_to(cr, 5, 20);
            cairo_set_font_size(cr, 16);
            if (sai_selection == 3) {
                cairo_set_source_rgb(cr, 0, 255, 0);
                cairo_show_text(cr, 
                    "Ready for new selection: First Click Maintained");
            }
            else {
                char buffer[64];
                cairo_set_source_rgb(cr, 0, 0, 255);
                int ret = snprintf(buffer, sizeof buffer, 
                    "Selection next click. Current: %i", sai_selection);
                cairo_show_text(cr, buffer);
            }
            cairo_restore(cr);
        }

        // say: "I'm finished drawing
        gdk_window_end_draw_frame(window, drawingContext);
    }

    // cleanup
    cairo_region_destroy(cairoRegion);

    return FALSE;
}

static float euclidian_distance(float x1, float y1, float x2, float y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

// Forward Call
static void sai_do_crop_image();
static void sai_do_crop_preview();

static gboolean button_release_callback(GtkWidget* event_box,
    GdkEventButton* event,
    gpointer        data)
{
    g_print("Event box released at coordinates %f,%f\n",
        event->x, event->y);
    if (sai_selection == 0) {
        int dx = ABS(sai_crop_x1 - sai_crop_x2);
        int dy = ABS(sai_crop_y1 - sai_crop_y2);
        float rx = 0;
        float ry = 0;
        if (dx > 0) rx = (1.0 * sai_width) / dx;
        if (dy > 0) ry = (1.0 * sai_height) / dy;
        if (!((rx < 3.0) && (ry < 3.0))) {
            // It is not a box, so it will be lines...
            sai_selection++;
        }
        else {
            sai_selection = 3;
            sai_crop_x3 = sai_crop_x2;
            sai_crop_y3 = sai_crop_y2;
            sai_crop_x4 = sai_crop_x2;
            sai_crop_y4 = sai_crop_y1;
            sai_crop_x2 = sai_crop_x1;
        }
    }
    else {
        sai_selection++;
    }
    g_print("Release Selection %i\n", sai_selection);

    if (sai_selection > 2) {
        // Ok, let's put some "order" into these 4 coordinates
        // We want:
        //  In Top Left => x1, y1
        //  In Bottom Left => x2, y2
        //  In Bottom Right => x3, y3
        //  In Top Right => x4, y4
        // Top Left?
        float x = 0;
        float y = 0;
        float d1 = euclidian_distance(x, y, sai_crop_x1, sai_crop_y1);
        float d2 = euclidian_distance(x, y, sai_crop_x2, sai_crop_y2);
        float d3 = euclidian_distance(x, y, sai_crop_x3, sai_crop_y3);
        float d4 = euclidian_distance(x, y, sai_crop_x4, sai_crop_y4);
        int x_tl = sai_crop_x1;
        int y_tl = sai_crop_y1;
        if ((d2 <= d1) && (d2 <= d3) && (d2 <= d4)) {
            x_tl = sai_crop_x2;
            y_tl = sai_crop_y2;
        }
        else if ((d3 <= d1) && (d3 <= d2) && (d3 <= d4)) {
            x_tl = sai_crop_x3;
            y_tl = sai_crop_y3;
        }
        else if ((d4 <= d1) && (d4 <= d2) && (d4 <= d3)) {
            x_tl = sai_crop_x4;
            y_tl = sai_crop_y4;
        }
        // Top Right?
        x = sai_width;
        y = 0;
        d1 = euclidian_distance(x, y, sai_crop_x1, sai_crop_y1);
        d2 = euclidian_distance(x, y, sai_crop_x2, sai_crop_y2);
        d3 = euclidian_distance(x, y, sai_crop_x3, sai_crop_y3);
        d4 = euclidian_distance(x, y, sai_crop_x4, sai_crop_y4);
        int x_tr = sai_crop_x1;
        int y_tr = sai_crop_y1;
        if ((d2 <= d1) && (d2 <= d3) && (d2 <= d4)) {
            x_tr = sai_crop_x2;
            y_tr = sai_crop_y2;
        }
        else if ((d3 <= d1) && (d3 <= d2) && (d3 <= d4)) {
            x_tr = sai_crop_x3;
            y_tr = sai_crop_y3;
        }
        else if ((d4 <= d1) && (d4 <= d2) && (d4 <= d3)) {
            x_tr = sai_crop_x4;
            y_tr = sai_crop_y4;
        }
        // Bottom Right?
        x = sai_width;
        y = sai_height;
        d1 = euclidian_distance(x, y, sai_crop_x1, sai_crop_y1);
        d2 = euclidian_distance(x, y, sai_crop_x2, sai_crop_y2);
        d3 = euclidian_distance(x, y, sai_crop_x3, sai_crop_y3);
        d4 = euclidian_distance(x, y, sai_crop_x4, sai_crop_y4);
        int x_br = sai_crop_x1;
        int y_br = sai_crop_y1;
        if ((d2 <= d1) && (d2 <= d3) && (d2 <= d4)) {
            x_br = sai_crop_x2;
            y_br = sai_crop_y2;
        }
        else if ((d3 <= d1) && (d3 <= d2) && (d3 <= d4)) {
            x_br = sai_crop_x3;
            y_br = sai_crop_y3;
        }
        else if ((d4 <= d1) && (d4 <= d2) && (d4 <= d3)) {
            x_br = sai_crop_x4;
            y_br = sai_crop_y4;
        }
        // Bottom Left?
        x = 0;
        y = sai_height;
        d1 = euclidian_distance(x, y, sai_crop_x1, sai_crop_y1);
        d2 = euclidian_distance(x, y, sai_crop_x2, sai_crop_y2);
        d3 = euclidian_distance(x, y, sai_crop_x3, sai_crop_y3);
        d4 = euclidian_distance(x, y, sai_crop_x4, sai_crop_y4);
        int x_bl = sai_crop_x1;
        int y_bl = sai_crop_y1;
        if ((d2 <= d1) && (d2 <= d3) && (d2 <= d4)) {
            x_bl = sai_crop_x2;
            y_bl = sai_crop_y2;
        }
        else if ((d3 <= d1) && (d3 <= d2) && (d3 <= d4)) {
            x_bl = sai_crop_x3;
            y_bl = sai_crop_y3;
        }
        else if ((d4 <= d1) && (d4 <= d2) && (d4 <= d3)) {
            x_bl = sai_crop_x4;
            y_bl = sai_crop_y4;
        }

        int xmin = 0;
        int ymin = 0;
        int xmax = 0;
        int ymax = 0;
        int w1 = ABS(x_tl - x_bl);
        int w2 = ABS(x_tr - x_br);
        int h1 = ABS(y_tl - y_tr);
        int h2 = ABS(y_bl - y_br);
        if ((w1<5)&&(w2 < 5)&& (h1 < 5) && (h2 < 5)) {
            xmin = (1.0 * (x_tl + x_bl)) / 2.0;
            xmax = (1.0 * (x_tr + x_br)) / 2.0;
            ymin = (1.0 * (y_tl + y_tr)) / 2.0;
            ymax = (1.0 * (y_bl + y_br)) / 2.0;
            sai_crop_x1 = xmin;
            sai_crop_y1 = ymin;
            sai_crop_x2 = xmin;
            sai_crop_y2 = ymax;
            sai_crop_x3 = xmax;
            sai_crop_y3 = ymax;
            sai_crop_x4 = xmax;
            sai_crop_y4 = ymin;
        }
        else {
            sai_crop_x1 = x_tl;
            sai_crop_y1 = y_tl;
            sai_crop_x2 = x_bl;
            sai_crop_y2 = y_bl;
            sai_crop_x3 = x_br;
            sai_crop_y3 = y_br;
            sai_crop_x4 = x_tr;
            sai_crop_y4 = y_tr;
        }

        gtk_widget_queue_draw_area((GtkWidget*)area_frame, 0, 0,
            sai_width, sai_height);
        sai_do_crop_image();
    }

    return TRUE;
}

static gboolean button_press_callback(GtkWidget* event_box,
    GdkEventButton* event,
    gpointer        data)
{
    g_print("Event box pressed at coordinates %f,%f\n",
        event->x, event->y);
    if ((sai_selection == -1)||(sai_selection == 0)||(sai_selection > 2)) {
        if (image_crop != NULL) {
            gtk_widget_destroy(image_crop);
            image_crop = NULL;
        }
        sai_selection = 0;
        // Begining of selection
        sai_crop_x1 = event->x;
        sai_crop_y1 = event->y;
        sai_crop_x2 = sai_crop_x1;
        sai_crop_y2 = sai_crop_y1;
        sai_crop_x3 = -1;
        sai_crop_y3 = -1;
        sai_crop_x4 = -1;
        sai_crop_y4 = -1;
    }

    g_print("Press Selection %i\n", sai_selection);
    g_print("Frame: %i,%i : %i,%i : %i,%i : %i,%i\n", 
        sai_crop_x1, sai_crop_y1, sai_crop_x2, sai_crop_y2, 
        sai_crop_x3, sai_crop_y3, sai_crop_x4, sai_crop_y4 );

    // Returning TRUE means we handled the event, so the signal
    // emission should be stopped (don’t call any further callbacks
    // that may be connected). 
    // Return FALSE to continue invoking callbacks.
    return TRUE;
}

static gboolean motion_callback(GtkWidget* event_box,
    GdkEventButton* event,
    gpointer        data)
{
    /*
    g_print("Motion at coordinates %f,%f\n",
        event->x, event->y);
    */
    if (sai_selection == 0) {
        sai_crop_x2 = event->x;
        sai_crop_y2 = event->y;
        sai_crop_x3 = -1;
        sai_crop_y3 = -1;
        sai_crop_x4 = -1;
        sai_crop_y4 = -1;
    }
    else if (sai_selection == 1) {
        // Next Selection...
        sai_crop_x3 = event->x;
        sai_crop_y3 = event->y;

    }
    else if (sai_selection == 2) {
        // Next Selection...
        sai_crop_x4 = event->x;
        sai_crop_y4 = event->y;
    }
    /*
    g_print("Frame: %i,%i : %i,%i : %i,%i : %i,%i\n", 
        sai_crop_x1, sai_crop_y1, 
        sai_crop_x2, sai_crop_y2, 
        sai_crop_x3, sai_crop_y3, 
        sai_crop_x4, sai_crop_y4);
    */
    gtk_widget_queue_draw_area((GtkWidget*)area_frame,0,0, 
        sai_width, sai_height);

    return TRUE;
}

static void sai_do_crop_image() {
    image_crop = NULL;
    if (area_frame != NULL) {
        int w1 = sai_crop_x4 - sai_crop_x1;
        int w2 = sai_crop_x3 - sai_crop_x2;
        int h1 = sai_crop_y2 - sai_crop_y1;
        int h2 = sai_crop_y3 - sai_crop_y4;
        g_print("Simple Rect Crop: %i. %i / %i, %i\n", 
            w1, w2, h1, h2);
        if ((w1 == w2) && (h1 == h2)) {
            // create the crop image
            temporary_pixbuf =
                gdk_pixbuf_copy(image_bw_pixbuf);
            image_crop_pixbuf = gdk_pixbuf_new_subpixbuf(
                temporary_pixbuf, sai_crop_x1,
                sai_crop_y1, w1, h1);
            sai_do_crop_preview();
            if (image_crop_pixbuf != NULL) {
                // Copy "Options" (like orientation)
                gdk_pixbuf_copy_options(
                    image_bw_pixbuf, image_crop_pixbuf);
                image_crop =
                    gtk_image_new_from_pixbuf(image_crop_pixbuf);
                // Add the image crop to the images layout
                if (image_crop != NULL) {
                    gtk_container_add(
                    GTK_CONTAINER(imagesBoxLine2), image_crop);
                    gtk_widget_show((GtkWidget*)image_crop);
                }
            }
            else {
                g_print("Graphical error message: Could not crop "
                    "BW Image into crop Image\n");
            }
        }
    }
}

static void sai_do_clear_ui() {
    // Free if necessary
    if (imagesBoxLine1 != NULL) {
        gtk_widget_destroy(imagesBoxLine1);
        imagesBoxLine1 = NULL;
    }
    if (imagesBoxLine2 != NULL) {
        gtk_widget_destroy(imagesBoxLine2);
        imagesBoxLine2 = NULL;
    }
    if (imagesBoxBoxes1 != NULL) {
        gtk_widget_destroy(imagesBoxBoxes1);
        imagesBoxBoxes1 = NULL;
    }
    if (imagesBoxBoxes2 != NULL) {
        gtk_widget_destroy(imagesBoxBoxes2);
        imagesBoxBoxes2 = NULL;
    }
    if (imagesBoxBoxes3 != NULL) {
        gtk_widget_destroy(imagesBoxBoxes3);
        imagesBoxBoxes3 = NULL;
    }
    if (imagesBoxBoxes4 != NULL) {
        gtk_widget_destroy(imagesBoxBoxes4);
        imagesBoxBoxes4 = NULL;
    }
    if (imagesBoxBoxes5 != NULL) {
        gtk_widget_destroy(imagesBoxBoxes5);
        imagesBoxBoxes5 = NULL;
    }
    if (imagesBoxBoxes6 != NULL) {
        gtk_widget_destroy(imagesBoxBoxes6);
        imagesBoxBoxes6 = NULL;
    }
    if (imagesBoxBoxes7 != NULL) {
        gtk_widget_destroy(imagesBoxBoxes7);
        imagesBoxBoxes7 = NULL;
    }
    if (imagesBoxBoxes8 != NULL) {
        gtk_widget_destroy(imagesBoxBoxes8);
        imagesBoxBoxes8 = NULL;
    }
    if (imagesBoxBoxes9 != NULL) {
        gtk_widget_destroy(imagesBoxBoxes9);
        imagesBoxBoxes9 = NULL;
    }
    g_object_unref(temporary_pixbuf);
    image_color = NULL;
    image_color_pixbuf = NULL;
    image_percentile = NULL;
    image_percentile_pixbuf = NULL;
    image_white = NULL;
    image_white_pixbuf = NULL;
    image_gray = NULL;
    image_gray_pixbuf = NULL;
    image_bw = NULL;
    image_bw_pixbuf = NULL;
    image_border = NULL;
    image_border_pixbuf = NULL;
    area_frame = NULL;
    area_frame_pixbuf = NULL;
    image_crop = NULL;
    image_crop_pixbuf = NULL;
    temporary_pixbuf = NULL;

}

static void sai_do_create_nn_values() {
    // Here we have 81 pixbuf of corresponding to the
    // 81 crops. All of them are:
    // sai_resized_box_w by sai_resized_box_h (15x15)
    // We need now to frame exactly the "digit"

    // the current line (1 out of 9)
    GtkWidget* cur_line;
    // the current crop of that line (1 out of 9)
    GtkWidget* cur_crop;

    int x = 0;
    int y = 0;
    int crop_num = -1;
    GList* linelist = gtk_container_get_children(GTK_CONTAINER(sai_main_box));
    GList* lines;
    GdkPixbuf* tempbixbuf[81];
    float weights[81];
    float widths[81];
    float heights[81];
    float weight_max = 0;
    float weight_ndx = -1;
    int line_ndx = 0;
    for (lines = linelist; lines; lines = lines->next) {
        cur_line = (GtkWidget*)lines->data;
        const gchar* s = gtk_widget_get_tooltip_text(cur_line);
        if ((s!=NULL)&&(g_ascii_strncasecmp(s,"L",1)==0)) {
            line_ndx++;
            int box_ndx = 0;
            g_print(s);
            g_print("\n");
            GList* croplist = 
                gtk_container_get_children(GTK_CONTAINER(cur_line));
            GList* crops;
            for (crops = croplist; crops; crops = crops->next) {
                cur_crop = (GtkWidget*)crops->data;
                const gchar* s = gtk_widget_get_tooltip_text(cur_crop);
                if ((s!=NULL)&&(g_ascii_strncasecmp(s,"P",1)==0)) {
                    // Current Crop is an image, it contains pixbuf
                    box_ndx++;
                    crop_num++;
                    g_print("\t");
                    g_print(s);
                    g_print("\n");
                    GdkPixbuf* pixbuf = 
                        gtk_image_get_pixbuf((GtkImage*)cur_crop);
                    // Frame the digit
                    if (crop_num==48) {
                        int zbreak = 1;
                    }
                    weights[crop_num] = sai_do_digit_only_image(pixbuf);
                    if (weights[crop_num]>weight_max) {
                        weight_ndx = crop_num;
                        weight_max=weights[crop_num];
                    }
                    int xcrop = sai_current_digit_x;
                    int ycrop = sai_current_digit_y;
                    int wcrop = sai_current_digit_w;
                    int hcrop = sai_current_digit_h;
                    widths[crop_num] = wcrop;
                    heights[crop_num] = hcrop;
                    
                    // Enlarge of 2 pixel on h, and 6 on w
                    xcrop--;
                    xcrop--;
                    xcrop--;
                    ycrop--;
                    wcrop += 6;
                    hcrop += 2;
                    if (xcrop<0) {
                        xcrop=0;
                        wcrop--;
                    }
                    if (ycrop<0) {
                        ycrop=0;
                        hcrop--;
                    }
                    if (xcrop+wcrop>sai_resized_box_w) wcrop = sai_resized_box_w-xcrop;
                    if (ycrop+hcrop>sai_resized_box_h) hcrop = sai_resized_box_h-ycrop;
                    // Crop and Resize it
                    GdkPixbuf* crop_pixbuf = gdk_pixbuf_new_subpixbuf(
                        pixbuf, xcrop, ycrop, wcrop, hcrop);
                    if (crop_pixbuf != NULL) {
                        // Work on constrast => Darkest should be black
                        sai_do_balance_if_necessary(crop_pixbuf);
                        // sai_final_box_w by sai_final_box_h (10x10)
                        tempbixbuf[crop_num] = gdk_pixbuf_scale_simple(
                            crop_pixbuf, sai_final_box_w, sai_final_box_w,
                            GDK_INTERP_HYPER);
                        // Rescale to 255 
                        sai_do_rescale_to_256(tempbixbuf[crop_num]);
                    } else {
                    g_print("Couldn't crop!\n");
                    }
                } else {
                    g_print("Not a crop widget!\n");
                }
            }
        }
    }

    if (weight_max>0) {

        // Let's clear the crops that seem to not contain a digit
        gboolean is_digit[81];
        int i=0;
        for (i=0; i<81; i++) {
            is_digit[i] = FALSE;
            float v = weights[i] / weight_max;
            if (v>0) {
                v = -log10f(v);
                if (v<2.0) {
                    is_digit[i] = TRUE;
                }
            }
            if (!is_digit[i]) {
                // Clear
                sai_do_clear_digit(tempbixbuf[i]);
            } else {
                // Write to a file
                size_t n = strlen(sai_filename);
                char *underscore = "_";
                sai_filename[n-4] = underscore[0];
                char newfilename[2000];
                int ret = snprintf(newfilename, sizeof newfilename, "%s_%i.%s", sai_filename, i, "txt");
                FILE *fPtr;
                fPtr = fopen(newfilename, "w");
                sai_do_persist_in_file(fPtr, tempbixbuf[i]);
                fclose(fPtr);
            }
        }

        g_print(" Widget found: %i\n", crop_num+1); // 0 start

        // Clear the UI
        sai_do_clear_ui();
        int margin = 4;
        imagesBoxBoxes1 =
            gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
        gtk_widget_set_tooltip_text(imagesBoxBoxes1, "L1");
        gtk_container_add(GTK_CONTAINER(sai_main_box), 
            imagesBoxBoxes1);
        imagesBoxBoxes2 =
            gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
        gtk_widget_set_tooltip_text(imagesBoxBoxes2, "L2");
        gtk_container_add(GTK_CONTAINER(sai_main_box),
            imagesBoxBoxes2);
        imagesBoxBoxes3 =
            gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
        gtk_widget_set_tooltip_text(imagesBoxBoxes3, "L3");
        gtk_container_add(GTK_CONTAINER(sai_main_box),
            imagesBoxBoxes3);
        imagesBoxBoxes4 =
            gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
        gtk_widget_set_tooltip_text(imagesBoxBoxes4, "L4");
        gtk_container_add(GTK_CONTAINER(sai_main_box),
            imagesBoxBoxes4);
        imagesBoxBoxes5 =
            gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
        gtk_widget_set_tooltip_text(imagesBoxBoxes5, "L5");
        gtk_container_add(GTK_CONTAINER(sai_main_box),
            imagesBoxBoxes5);
        imagesBoxBoxes6 =
            gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
        gtk_widget_set_tooltip_text(imagesBoxBoxes6, "L6");
        gtk_container_add(GTK_CONTAINER(sai_main_box),
            imagesBoxBoxes6);
        imagesBoxBoxes7 =
            gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
        gtk_widget_set_tooltip_text(imagesBoxBoxes7, "L7");
        gtk_container_add(GTK_CONTAINER(sai_main_box),
            imagesBoxBoxes7);
        imagesBoxBoxes8 =
            gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
        gtk_widget_set_tooltip_text(imagesBoxBoxes8, "L8");
        gtk_container_add(GTK_CONTAINER(sai_main_box),
            imagesBoxBoxes8);
        imagesBoxBoxes9 =
            gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
        gtk_widget_set_tooltip_text(imagesBoxBoxes9, "L9");
        gtk_container_add(GTK_CONTAINER(sai_main_box),
            imagesBoxBoxes9);

        // From there we can take the 81 cases
        crop_num = -1;
        float thickness = 0;
        for (y = 0; y < 9; y++) {
            if (y == 0) {
                cur_line = imagesBoxBoxes1;
            }
            else if (y == 1) {
                cur_line = imagesBoxBoxes2;
            }
            else if (y == 2) {
                cur_line = imagesBoxBoxes3;
            }
            else if (y == 3) {
                cur_line = imagesBoxBoxes4;
            }
            else if (y == 4) {
                cur_line = imagesBoxBoxes5;
            }
            else if (y == 5) {
                cur_line = imagesBoxBoxes6;
            }
            else if (y == 6) {
                cur_line = imagesBoxBoxes7;
            }
            else if (y == 7) {
                cur_line = imagesBoxBoxes8;
            }
            else {
                cur_line = imagesBoxBoxes9;
            }
            for (x = 0; x < 9; x++) {
                crop_num++;
                // Copy "Options" (like orientation)
                GtkWidget* image_box =
                    gtk_image_new_from_pixbuf(tempbixbuf[crop_num]);
                // Release the reference for GTK Garbage collector
                g_object_unref(tempbixbuf[crop_num]);
                // Add the image crop to the images layout
                if (image_box != NULL) {
                    char buffer[10];
                    int ret = snprintf(buffer, sizeof buffer, 
                        "D%i", crop_num);
                    gtk_widget_set_tooltip_text(image_box, buffer);
                    gtk_container_add(
                        GTK_CONTAINER(cur_line), image_box);
                }
            }
        }

        gtk_widget_show_all(sai_app_window);
        
    }

}

static void sai_do_create_digit_values() {
    // Here we have 81 pixbuf of corresponding to the
    // 81 crops of different sizes
    // We need to frame the "digit"

    // the current line (1 out of 9)
    GtkWidget* cur_line;
    // the current crop of that line (1 out of 9)
    GtkWidget* cur_crop;

    int x = 0;
    int y = 0;
    int crop_num = -1;
    GList* linelist = gtk_container_get_children(GTK_CONTAINER(sai_main_box));
    GList* lines;
    GdkPixbuf* tempbixbuf[81];
    for (lines = linelist; lines; lines = lines->next) {
        cur_line = (GtkWidget*)lines->data;
        const gchar* s = gtk_widget_get_tooltip_text(cur_line);
        if ((s!=NULL)&&(g_ascii_strncasecmp(s,"L",1)==0)) {
            g_print(s);
            g_print("\n");
            GList* croplist = 
                gtk_container_get_children(GTK_CONTAINER(cur_line));
            GList* crops;
            for (crops = croplist; crops; crops = crops->next) {
                cur_crop = (GtkWidget*)crops->data;
                const gchar* s = gtk_widget_get_tooltip_text(cur_crop);
                if ((s!=NULL)&&(g_ascii_strncasecmp(s,"C",1)==0)) {
                    // Current Crop is an image, it contains pixbuf
                    crop_num++;
                    g_print("\t");
                    g_print(s);
                    g_print("\n");
                    GdkPixbuf* pixbuf = 
                        gtk_image_get_pixbuf((GtkImage*)cur_crop);
                    // Copy the pixbuf and Resize it to 
                    // sai_resized_box_w by sai_resized_box_h (15x15)
                    tempbixbuf[crop_num] = gdk_pixbuf_scale_simple(
                        pixbuf, sai_resized_box_w, sai_resized_box_h,
                        GDK_INTERP_HYPER);
                    /*
                    // Set 0,0 pixel Red
                    guchar* source_pixels;
                    source_pixels = gdk_pixbuf_get_pixels(tempbixbuf[crop_num]);
                    guchar* write_offset = source_pixels;
                    *(write_offset++) = 255;
                    *(write_offset++) = 0;
                    *(write_offset++) = 0;
                    */
                } else {
                    g_print("Not a crop widget!\n");
                }
            }
        }
    }

    g_print(" Widget found: %i\n", crop_num+1); // 0 start

    // Clear the UI
    sai_do_clear_ui();
    int margin = 4;
    imagesBoxBoxes1 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes1, "L1");
    gtk_container_add(GTK_CONTAINER(sai_main_box), 
        imagesBoxBoxes1);
    imagesBoxBoxes2 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes2, "L2");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes2);
    imagesBoxBoxes3 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes3, "L3");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes3);
    imagesBoxBoxes4 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes4, "L4");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes4);
    imagesBoxBoxes5 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes5, "L5");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes5);
    imagesBoxBoxes6 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes6, "L6");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes6);
    imagesBoxBoxes7 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes7, "L7");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes7);
    imagesBoxBoxes8 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes8, "L8");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes8);
    imagesBoxBoxes9 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes9, "L9");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes9);

    // From there we can take the 81 cases
    crop_num = -1;
    float thickness = 0;
    for (y = 0; y < 9; y++) {
        if (y == 0) {
            cur_line = imagesBoxBoxes1;
        }
        else if (y == 1) {
            cur_line = imagesBoxBoxes2;
        }
        else if (y == 2) {
            cur_line = imagesBoxBoxes3;
        }
        else if (y == 3) {
            cur_line = imagesBoxBoxes4;
        }
        else if (y == 4) {
            cur_line = imagesBoxBoxes5;
        }
        else if (y == 5) {
            cur_line = imagesBoxBoxes6;
        }
        else if (y == 6) {
            cur_line = imagesBoxBoxes7;
        }
        else if (y == 7) {
            cur_line = imagesBoxBoxes8;
        }
        else {
            cur_line = imagesBoxBoxes9;
        }
        for (x = 0; x < 9; x++) {
            crop_num++;
            // Copy "Options" (like orientation)
            GtkWidget* image_box =
                gtk_image_new_from_pixbuf(tempbixbuf[crop_num]);
            // Release the reference for GTK Garbage collector
            g_object_unref(tempbixbuf[crop_num]);
            // Add the image crop to the images layout
            if (image_box != NULL) {
                char buffer[10];
                int ret = snprintf(buffer, sizeof buffer, 
                    "P%i", crop_num);
                gtk_widget_set_tooltip_text(image_box, buffer);
                gtk_container_add(
                    GTK_CONTAINER(cur_line), image_box);
            }
        }
    }

    gtk_widget_show_all(sai_app_window);

}

static void sai_do_proceed_extract() {
    // Clear the UI
    sai_do_clear_ui();
    int margin = 4;
    imagesBoxBoxes1 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes1, "L1");
    gtk_container_add(GTK_CONTAINER(sai_main_box), 
        imagesBoxBoxes1);
    imagesBoxBoxes2 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes2, "L2");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes2);
    imagesBoxBoxes3 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes3, "L3");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes3);
    imagesBoxBoxes4 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes4, "L4");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes4);
    imagesBoxBoxes5 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes5, "L5");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes5);
    imagesBoxBoxes6 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes6, "L6");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes6);
    imagesBoxBoxes7 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes7, "L7");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes7);
    imagesBoxBoxes8 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes8, "L8");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes8);
    imagesBoxBoxes9 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    gtk_widget_set_tooltip_text(imagesBoxBoxes9, "L9");
    gtk_container_add(GTK_CONTAINER(sai_main_box),
        imagesBoxBoxes9);

    // Where to place the cropped thumbnails
    GtkWidget* cur_line;

    if (image_original_pixbuf != NULL) {
        // Apply the transformation seen and used during the first phase
        // ============================================================
        {
            // Rotate if necessary
            if ((sai_apply_rotate==TRUE)&&(sai_rotate_value != 0)) {
                GdkPixbuf* temp_rot_pixbuf =
                    gdk_pixbuf_copy(image_original_pixbuf);
                sai_do_bordercolor(temp_rot_pixbuf);
                image_proceed_pixbuf = sai_do_rotate(temp_rot_pixbuf,
                    sai_rotate_value,
                    sai_border_red,
                    sai_border_green,
                    sai_border_blue);
                g_object_unref(temp_rot_pixbuf);
            } else {
                // Prepare the Image to be used from the original one
                // without rotation
                image_proceed_pixbuf =
                    gdk_pixbuf_copy(image_original_pixbuf);
            }
            // Copy "Options" (like orientation)
            gdk_pixbuf_copy_options(image_original_pixbuf,
                image_proceed_pixbuf);
            // Do the white balance
            sai_do_white_balance(image_proceed_pixbuf);
            // Do Historgram
            sai_do_histograms(image_proceed_pixbuf,
                SAI_HISTO_MODE_GRAY);
            // Do the percentile constrast
            sai_do_percentile_contrast(image_proceed_pixbuf,
                SAI_PERCENTILE_BLACK, SAI_PERCENTILE_WHITE);
            // Transform in Gray
            sai_do_colors_to_grays(image_proceed_pixbuf,
                SAI_GRAY_MODE_MAX);
            // Go to BW
            sai_do_colors_to_bw(image_proceed_pixbuf);
        }

        // Then calculate the rx and ry
        // based on the ratio
        // of original image dimensions v.s.
        // preview dimensions used during the first phase
        int w = gdk_pixbuf_get_width(image_proceed_pixbuf);
        int h = gdk_pixbuf_get_height(image_proceed_pixbuf);
        float rx = (1.0 * w) / (1.0 * sai_width);
        float ry = (1.0 * h) / (1.0 * sai_height);
        // Move to the real top left corner of the grid 
        float dx = sai_crop_x1*rx;
        float dy = sai_crop_y1*rx;

        // From there we can take the 81 cases
        int x = 0;
        int y = 0;
        int crop_num = -1;
        float thickness = 0;
        for (y = 0; y < 9; y++) {
            if (y == 0) {
                cur_line = imagesBoxBoxes1;
            }
            else if (y == 1) {
                cur_line = imagesBoxBoxes2;
            }
            else if (y == 2) {
                cur_line = imagesBoxBoxes3;
            }
            else if (y == 3) {
                cur_line = imagesBoxBoxes4;
            }
            else if (y == 4) {
                cur_line = imagesBoxBoxes5;
            }
            else if (y == 5) {
                cur_line = imagesBoxBoxes6;
            }
            else if (y == 6) {
                cur_line = imagesBoxBoxes7;
            }
            else if (y == 7) {
                cur_line = imagesBoxBoxes8;
            }
            else {
                cur_line = imagesBoxBoxes9;
            }
            for (x = 0; x < 9; x++) {
                crop_num++;
                // create the crop image
                int xcrop = (int)ceilf(dx + 
                    (sai_box_x1[crop_num]+thickness)*rx);
                int ycrop = (int)ceilf(dy + 
                    (sai_box_y1[crop_num]+thickness)*ry);
                int wcrop = (int)ceilf(dx + 
                    (sai_box_x2[crop_num]-thickness)*rx) - xcrop;
                int hcrop = (int)ceilf(dy + 
                    (sai_box_y2[crop_num]-thickness)*ry) - ycrop;
                GdkPixbuf* crop_pixbuf = gdk_pixbuf_new_subpixbuf(
                    image_proceed_pixbuf, xcrop, ycrop, wcrop, hcrop);
                if (crop_pixbuf != NULL) {
                    // Copy "Options" (like orientation)
                    gdk_pixbuf_copy_options(
                        image_proceed_pixbuf, crop_pixbuf);
                    GtkWidget* image_box =
                        gtk_image_new_from_pixbuf(crop_pixbuf);
                    char buffer[10];
                    int ret = snprintf(buffer, sizeof buffer, 
                        "C%i", crop_num);
                    gtk_widget_set_tooltip_text(image_box, buffer);
                    // Add the image crop to the images layout
                    if (image_box != NULL) {
                        gtk_container_add(
                            GTK_CONTAINER(cur_line), image_box);
                    }
                }
            }
        }


    }

    gtk_widget_show_all(sai_app_window);

}

static void sai_do_crop_preview() {

    // From there we can take the 81 cases
    int w = gdk_pixbuf_get_width(image_crop_pixbuf);
    int h = gdk_pixbuf_get_height(image_crop_pixbuf);
    float rx = 1;
    float ry = 1;

    float sai_ex_border = 2*rx;
    float sai_x_in_border = 7*rx;
    float sai_y_in_border = 6*ry;
    float x_start = (0*rx) + sai_ex_border;
    float x_end = (w*rx) - sai_ex_border;
    float y_start = (0*ry) + sai_ex_border;
    float y_end = (h*ry) - sai_ex_border;
    float xSize = (x_end - x_start) / 9.0;
    float ySize = (y_end - y_start) / 9.0;
    int xdelta = (int)round(5*rx);
    int ydelta = (int)round(5*ry);
    int x = 0;
    int y = 0;
    int z;
    float y_go = y_start;
    int n = 0;
    int m = 0;
    int nbox = -1;
    for (z = 0; z < 9; z++) {
        if (y_go + ySize < h) {
            float x_go = x_start;
            for (x = 0; x < 9; x++) {
                nbox++;
                // create the crop preview
                if ((y_go + ySize < h) && (x_go + xSize < w)) {
                    int xcrop = (int)round(x_go+sai_x_in_border);
                    int ycrop = (int)round(y_go+sai_y_in_border);
                    int wcrop = (int)round(xSize - 2.0 * sai_x_in_border);
                    int hcrop = (int)round(ySize - 2.0 * sai_y_in_border);
                    int xmin = xcrop - xdelta;
                    int ymin = ycrop - ydelta;
                    int xmax = xcrop + wcrop + xdelta;
                    int ymax = ycrop + hcrop + ydelta;
                    if (xmin<0) xmin=0;
                    if (ymin<0) ymin=0;
                    if (xmax>w-1) xmax=w-1;
                    if (ymax>h-1) ymax=h-1;
                    sai_do_fill(image_crop_pixbuf,nbox ,xcrop+1, 
                        (ycrop+(hcrop/2)),xmin,ymin,xmax,ymax);
                    x_go += xSize;
                }
            }
        }
        y_go += ySize;
    }


}

static void sai_do_load() {
    
    if (sai_filename=="") return;
    sai_rotate_text = gtk_entry_get_text((GtkEntry*)entry_rotate);
    sai_rotate_value = atof(sai_rotate_text);
    sai_treshold_text = gtk_entry_get_text((GtkEntry*)entry_treshold);
    sai_treshold_value = atof(sai_treshold_text);

    sai_do_clear_ui();

    // Create the first line for images layout box
    imagesBoxLine1 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    // Create the second line for images layout box
    imagesBoxLine2 =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    // Add the horizontal boxes to 
    // the main vertical layout box
    gtk_container_add(GTK_CONTAINER(sai_main_box), imagesBoxLine1);
    gtk_container_add(GTK_CONTAINER(sai_main_box), imagesBoxLine2);

    // Load the image in color
    GError* gerror = NULL;
    image_original_pixbuf = gdk_pixbuf_new_from_file(sai_filename,
        &gerror);

    if (image_original_pixbuf == NULL) {
        if (gerror != NULL) {
            g_print("Graphical error message not %s\n",
                gerror->message);
        }
        else {
            g_print("Graphical error message: Unknown\n");
        }
    }
    else {
        // Rescale to SAI_IMAGE_SIZE width
        //Height in pixels
        int h = gdk_pixbuf_get_height(image_original_pixbuf);
        //width in pixels
        int w = gdk_pixbuf_get_width(image_original_pixbuf);
        
        float r = SAI_IMAGE_SIZE / (1.0 * w);
        w = SAI_IMAGE_SIZE;
        h = (int)(r * h);
        sai_width = w;
        sai_height = h;
        if (image_original_pixbuf != NULL) {
            if ((sai_apply_rotate)&&(sai_rotate_value != 0)) {
                GdkPixbuf* temp_rot_pixbuf = gdk_pixbuf_scale_simple(
                    image_original_pixbuf, w, h, GDK_INTERP_HYPER);
                sai_do_bordercolor(temp_rot_pixbuf);
                image_color_pixbuf = sai_do_rotate(temp_rot_pixbuf,
                    sai_rotate_value,
                    sai_border_red,
                    sai_border_green,
                    sai_border_blue);
                g_object_unref(temp_rot_pixbuf);
            } else {
                image_color_pixbuf = gdk_pixbuf_scale_simple(
                    image_original_pixbuf, w, h,
                    GDK_INTERP_HYPER);
            }
            image_color = gtk_image_new_from_pixbuf(
                image_color_pixbuf);
            gtk_container_add(
                GTK_CONTAINER(imagesBoxLine1), image_color);
        }
        else {
            g_print("Graphical error message: Unable to rescale "
                "original image to %s pixels width\n",
                SAI_IMAGE_SIZE);
        }
    }

    if (image_color != NULL) {
        // create the white balance image
        image_white_pixbuf =
            gdk_pixbuf_copy(image_color_pixbuf);
        if (image_white_pixbuf != NULL) {
            // Copy "Options" (like orientation)
            gdk_pixbuf_copy_options(image_color_pixbuf,
                image_white_pixbuf);
            // Do the white balance
            sai_do_white_balance(image_white_pixbuf);
            image_white =
                gtk_image_new_from_pixbuf(image_white_pixbuf);
            // Add the image2 to the images layout
            gtk_container_add(
                GTK_CONTAINER(imagesBoxLine1), image_white);
        }
        else {
            g_print("Graphical error message: Could not clone "
                "Percentile Image into White Image\n");
        }
    }

    if (image_white != NULL) {
        // create the percentile image
        image_percentile_pixbuf =
            gdk_pixbuf_copy(image_white_pixbuf);
        if (image_percentile_pixbuf != NULL) {
            // Copy "Options" (like orientation)
            gdk_pixbuf_copy_options(image_white_pixbuf,
                image_percentile_pixbuf);
            // Do the gray histogram
            sai_do_histograms(image_percentile_pixbuf,
                SAI_HISTO_MODE_GRAY);
            // Do the percentile constrast
            sai_do_percentile_contrast(image_percentile_pixbuf,
                SAI_PERCENTILE_BLACK, SAI_PERCENTILE_WHITE);
            image_percentile =
                gtk_image_new_from_pixbuf(image_percentile_pixbuf);
            // Add the image2 to the images layout
            gtk_container_add(
                GTK_CONTAINER(imagesBoxLine1), image_percentile);
        }
        else {
            g_print(
                "Graphical error message: Could not clone Color "
                "Image into Percentil Image\n");
        }
    }

    if (image_percentile != NULL) {

        // create the gray image
        image_gray_pixbuf =
            gdk_pixbuf_copy(image_percentile_pixbuf);
        if (image_gray_pixbuf != NULL) {
            // Copy "Options" (like orientation)
            gdk_pixbuf_copy_options(image_percentile_pixbuf,
                image_gray_pixbuf);
            // Transform in Gray
            sai_do_colors_to_grays(image_gray_pixbuf,
                SAI_GRAY_MODE_MAX);
            image_gray =
                gtk_image_new_from_pixbuf(image_gray_pixbuf);
            // Add the image2 to the images layout
            gtk_container_add(
                GTK_CONTAINER(imagesBoxLine1), image_gray);
        }
        else {
            g_print("Graphical error message: Could not clone "
                "White balanced Image into Gray Image\n");
        }
    }

    if (image_gray != NULL) {
        // create the black and white image
        image_bw = NULL;
        image_bw_pixbuf =
            gdk_pixbuf_copy(image_gray_pixbuf);
        if (image_bw_pixbuf != NULL) {
            // Copy "Options" (like orientation)
            gdk_pixbuf_copy_options(
                image_gray_pixbuf, image_bw_pixbuf);
            // Transform in Black and White
            sai_do_colors_to_bw(image_bw_pixbuf);

            // Convolution Verticale
            float vert_template[] = {
                0.00,  0.00,  0.00,  0.15,  0.00,  0.00,  0.00,
                0.00,  0.00,  0.00,  0.15,  0.00,  0.00,  0.00,
                0.00,  0.00,  0.00,  0.15,  0.00,  0.00,  0.00,
                0.00,  0.00,  0.00,  0.10,  0.00,  0.00,  0.00,
                0.00,  0.00,  0.00,  0.15,  0.00,  0.00,  0.00,
                0.00,  0.00,  0.00,  0.15,  0.00,  0.00,  0.00,
                0.00,  0.00,  0.00,  0.15,  0.00,  0.00,  0.00,
            };
            float* mat_mem = malloc(sizeof(float) * 49);
            float* mat = mat_mem;
            for (int i =0; i<49; i++) {
                *mat = (float)vert_template[i];
                mat++;
            }
            GdkPixbuf* tmp1_pixbuf =
                gdk_pixbuf_copy(image_bw_pixbuf);
            mat = mat_mem;
            sai_do_convolution(image_bw_pixbuf, tmp1_pixbuf, mat, 7, 0);
            free(mat_mem);
            GdkPixbuf* tmp2_pixbuf =
                gdk_pixbuf_copy(tmp1_pixbuf);
            sai_do_findvertical(tmp2_pixbuf);
            g_object_unref(tmp2_pixbuf);
            g_object_unref(tmp1_pixbuf);

            // Convolution Horizontale
            float hori_template[] = {
                0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,
                0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,
                0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,
                0.16,  0.16,  0.16,  0.04,  0.16,  0.16,  0.16,
                0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,
                0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,
                0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,
            };
            float* hori_mem = malloc(sizeof(float) * 49);
            mat = hori_mem;
            for (int i =0; i<49; i++) {
                *mat = (float)hori_template[i];
                mat++;
            }
            GdkPixbuf* tmp3_pixbuf =
                gdk_pixbuf_copy(image_bw_pixbuf);
            mat = hori_mem;
            sai_do_convolution(image_bw_pixbuf, tmp3_pixbuf, mat, 7, 25);
            free(hori_mem);
            GdkPixbuf* tmp4_pixbuf =
                gdk_pixbuf_copy(tmp3_pixbuf);
            sai_do_findhorizontal(tmp4_pixbuf);
            g_object_unref(tmp4_pixbuf);
            g_object_unref(tmp3_pixbuf);

            image_bw =
                gtk_image_new_from_pixbuf(image_bw_pixbuf);
            // Add the image black&white to the images layout
            gtk_container_add(
                GTK_CONTAINER(imagesBoxLine2), image_bw);
        }
        else {
            g_print("Graphical error message: Could not clone "
                "Gray Image into Black&White Image\n");
        }

    }

    /*
    if (image_bw != NULL) {
        // create the black and white image
        image_border = NULL;
        image_border_pixbuf =
            gdk_pixbuf_copy(image_bw_pixbuf);
        if (image_bw_pixbuf != NULL) {
            // Copy "Options" (like orientation)
            gdk_pixbuf_copy_options(
                image_bw_pixbuf, image_border_pixbuf);
            // Find borders
            sai_do_findborders(image_bw_pixbuf, 
                image_border_pixbuf);
            image_border =
                gtk_image_new_from_pixbuf(image_border_pixbuf);
            // Add the image black&white to the images layout
            gtk_container_add(
                GTK_CONTAINER(imagesBoxLine2), image_border);
        }
        else {
            g_print("Graphical error message: Could not clone "
                "BW Image into Red Border Detection Image\n");
        }

    }
    */

    if (image_bw_pixbuf != NULL) {
        g_print("Framing\n");

        // To shiff the croping area a bit because of a bias
        int dx1 = (int)round(4.0*(sai_width/240));
        int dx2 = (int)round(6.0*(sai_width/240));
        int dy1 = (int)round(4.0*(sai_height/240));
        int dy2 = (int)round(6.0*(sai_height/240));

        // Croping area
        sai_crop_x1 = grid_hori_x1-dx1;
        if (sai_crop_x1 < 0) sai_crop_x1=0;
        
        sai_crop_y1 = grid_vert_y1-dy1;
        if (sai_crop_y1 < 0) sai_crop_y1=0;

        sai_crop_x2 = grid_hori_x1-dx1;
        if (sai_crop_x2 < 0) sai_crop_x2=0;

        sai_crop_y2 = grid_vert_y2+dy2;
        if (sai_crop_y2 > sai_height-1) sai_crop_y2=sai_height-1;

        sai_crop_x3 = grid_hori_x2+dx2;
        if (sai_crop_x3 > sai_width-1) sai_crop_x3=sai_width-1;

        sai_crop_y3 = grid_vert_y2+dy2;
        if (sai_crop_y3 > sai_height-1) sai_crop_y3=sai_height-1;

        sai_crop_x4 = grid_hori_x2+dx2;
        if (sai_crop_x4 > sai_width-1) sai_crop_x4=sai_width-1;

        sai_crop_y4 = grid_vert_y1-dy1;
        if (sai_crop_y4 < 0) sai_crop_y4=0;

        // To Show the red frame
        sai_selection = 3;

        // create the frame image
        area_frame = NULL;
        area_frame_pixbuf =
            gdk_pixbuf_copy(image_bw_pixbuf);
        if (area_frame_pixbuf != NULL) {
            // Copy "Options" (like orientation)
            //image_frame_pixbuf = sai_do_frame(image_frame_pixbuf);
            gdk_pixbuf_copy_options(
                image_bw_pixbuf, area_frame_pixbuf);
            int h = gdk_pixbuf_get_height(area_frame_pixbuf);
            int w = gdk_pixbuf_get_width(area_frame_pixbuf);
            area_frame = (GtkDrawingArea*)gtk_drawing_area_new();
            gtk_widget_set_size_request((GtkWidget*)area_frame, w, h);
            if (area_frame != NULL) {
                g_signal_connect((GtkWidget*)area_frame, 
                    "draw", 
                    G_CALLBACK(draw_event_callback), 
                    NULL);
                sai_area_from_event_box;
                sai_area_from_event_box = gtk_event_box_new();
                gtk_container_add(
                    GTK_CONTAINER(sai_area_from_event_box), 
                    (GtkWidget*)area_frame);
                // Handle the event for the image color
                g_signal_connect(G_OBJECT(sai_area_from_event_box),
                    "button_press_event",
                    G_CALLBACK(button_press_callback),
                    NULL);
                g_signal_connect(G_OBJECT(sai_area_from_event_box),
                    "button_release_event",
                    G_CALLBACK(button_release_callback),
                    NULL);
                g_signal_connect(G_OBJECT(sai_area_from_event_box),
                    "motion-notify-event",
                    G_CALLBACK(motion_callback),
                    NULL);
                gtk_widget_add_events(sai_area_from_event_box, 
                    GDK_POINTER_MOTION_MASK);
                gtk_container_add(GTK_CONTAINER(imagesBoxLine2),
                    sai_area_from_event_box);
            }
        }
        else {
            g_print("Graphical error message: Could not frame "
                "BW Image into frame Image\n");
        }

        sai_do_crop_image();

    }

    gtk_widget_show_all(sai_app_window);

}

static void sai_do_choose_file(GtkWidget* parent_window) {
    GtkWidget* dialog;

    dialog = gtk_file_chooser_dialog_new("Open File",
        (GtkWindow*)parent_window,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        ("_Cancel"),
        GTK_RESPONSE_CANCEL,
        ("_Open"),
        GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        sai_filename = gtk_file_chooser_get_filename(
            GTK_FILE_CHOOSER(dialog));
    } else {
       sai_filename = ""; 
    }

    gtk_widget_destroy(dialog);
}

static gboolean proceed_press_callback(GtkWidget* event_box, 
    GdkEventButton* event, gpointer data) {

    g_print("Proceed to extraction Event clicked\n");

    sai_do_proceed_extract();

    return TRUE;
}

static gboolean digit_values_press_callback(GtkWidget* event_box, 
    GdkEventButton* event, gpointer data) {

    g_print("Digit Values Event clicked\n");

    sai_do_create_digit_values();

    return TRUE;
}

static gboolean nn_values_press_callback(GtkWidget* event_box, 
    GdkEventButton* event, gpointer data) {

    g_print("NN Values Event clicked\n");

    sai_do_create_nn_values();

    return TRUE;
}

static gboolean rotate_press_callback(GtkWidget* event_box,
    GdkEventButton* event, gpointer data) {
    sai_apply_rotate = TRUE;
    sai_rotate_text = gtk_entry_get_text((GtkEntry*)entry_rotate);
    sai_rotate_value = atof(sai_rotate_text);
    char buffer[64];
    int ret = snprintf(buffer, sizeof buffer, "%f", 
        sai_rotate_value);
    g_print("Rotate Event clicked: %s\n", buffer);
    sai_do_load();
    return TRUE;
}

static gboolean load_press_callback(GtkWidget* event_box,
    GdkEventButton* event, gpointer data) {

    g_print("Load Event clicked\n");
    sai_do_choose_file(sai_app_window);
    sai_apply_rotate = FALSE;
    sai_do_load();
    return TRUE;
}

static void on_activate(GtkApplication* app) {
    // Create a new window
    sai_app_window =
            gtk_application_window_new(app);
    // Create a main layout box
    sai_main_box = 
            gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    // Create a buttons layout box
    buttonsBox = 
            gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);    
    // Add the horizontal boxes to 
    // the main vertical layout box
    gtk_container_add(GTK_CONTAINER(sai_main_box), buttonsBox);

    // Create a Quit button
    buttonQuit = gtk_button_new_with_label("Quit");
    // When the button is clicked,
    // close the window passed as an argument
    g_signal_connect_swapped(buttonQuit, "clicked", 
            G_CALLBACK(gtk_window_close), sai_app_window);
    // Add the Quit Button to the buttons layout box
    gtk_container_add(GTK_CONTAINER(buttonsBox), buttonQuit);

    // Create a Load button
    buttonLoad = gtk_button_new_with_label("Load");
    // When the button is clicked,
    // let user selects a file
    g_signal_connect_swapped(buttonLoad, "button_press_event",
        G_CALLBACK(load_press_callback), sai_app_window);
    // Add the Load Button to the buttons layout box
    gtk_container_add(GTK_CONTAINER(buttonsBox), buttonLoad);

    // Add a sai_rotate_value input
    entry_rotate = gtk_entry_new();
    // insert the following text into the text entry, 
    // as its initial value.
    gtk_entry_set_text(GTK_ENTRY(entry_rotate), "-0.61");
    // Create a Rotate button
    button_rotate = gtk_button_new_with_label("Rotate");
    // When the button is clicked, 
    // proceed
    g_signal_connect_swapped(button_rotate, "button_press_event",
        G_CALLBACK(rotate_press_callback), sai_app_window);
    // Add the Proceed Button to the buttons layout box
    gtk_container_add(GTK_CONTAINER(buttonsBox), button_rotate);
    gtk_container_add(GTK_CONTAINER(buttonsBox), entry_rotate);

    // Add a sai_treshold_value input
    GtkWidget* label_treshold = gtk_label_new("Black Treshold (avg=0.7, auto=0):");
    gtk_container_add(GTK_CONTAINER(buttonsBox), label_treshold);
    entry_treshold = gtk_entry_new();
    // insert the following text into the text entry, 
    // as its initial value.
    gtk_entry_set_text(GTK_ENTRY(entry_treshold), "0.0");
    // Add the Proceed Button to the buttons layout box
    gtk_container_add(GTK_CONTAINER(buttonsBox), entry_treshold);

    // Create a Proceed Extraction button
    sai_btn_proceed_extract = gtk_button_new_with_label(
        "Proceed to Extraction");
    // When the button is clicked, 
    // proceed
    g_signal_connect_swapped(sai_btn_proceed_extract, "button_press_event",
        G_CALLBACK(proceed_press_callback), sai_app_window);
    // Add the Proceed Extraction Button to the buttons layout box
    gtk_container_add(GTK_CONTAINER(buttonsBox), sai_btn_proceed_extract);

    // Create a create NN entries values button
    sai_btn_digit_values = gtk_button_new_with_label("Create Digit Values");
    // When the button is clicked, 
    // proceed
    g_signal_connect_swapped(sai_btn_digit_values, "button_press_event",
        G_CALLBACK(digit_values_press_callback), sai_app_window);
    // Add the Proceed Extraction Button to the buttons layout box
    gtk_container_add(GTK_CONTAINER(buttonsBox), sai_btn_digit_values);

    // Create a create NN entries values button
    sai_btn_nn_values = gtk_button_new_with_label("Create NN Values");
    // When the button is clicked, 
    // proceed
    g_signal_connect_swapped(sai_btn_nn_values, "button_press_event",
        G_CALLBACK(nn_values_press_callback), sai_app_window);
    // Add the Proceed Extraction Button to the buttons layout box
    gtk_container_add(GTK_CONTAINER(buttonsBox), sai_btn_nn_values);

    // Add the Layout Box to the Window
    gtk_container_add(GTK_CONTAINER(sai_app_window), sai_main_box);
    // Show the window to the user
    gtk_widget_show_all(sai_app_window);
    gtk_window_present(GTK_WINDOW(sai_app_window));

}


int main(int argc, char* argv[]) {
    // Create a new application
    //printf("Hello, World!\n");
    GtkApplication* app = 
        gtk_application_new("com.example.GtkApplication",
        G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", 
        G_CALLBACK(on_activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}

// NOT USED
// =======================================

static void sai_list_example() {

    GSList *dotlist = NULL;
    GSList *iterator = NULL;
    SaiIntDot* dot1 = g_new(SaiIntDot,1);
    dot1->x = 10;
    dot1->y = 5;
    dotlist = g_slist_append(dotlist, dot1);
    SaiIntDot* dot2 = g_new(SaiIntDot,1);
    dot2->x = 20;
    dot2->y = 5;
    dotlist = g_slist_append(dotlist, dot2);
    SaiIntDot* dot3 = g_new(SaiIntDot,1);
    dot3->x = 30;
    dot3->y = 5;
    dotlist = g_slist_append(dotlist, dot3);
    int n=0;
    for (iterator = dotlist; iterator; iterator = iterator->next) {
        n++;
        SaiIntDot* pnt = (SaiIntDot*)iterator->data;
        g_print("dot #%i is %i,%i\n", n, pnt->x, pnt->y);
    }    
    g_slist_foreach(dotlist, (GFunc)g_free, NULL);
    g_slist_free(dotlist);

}

