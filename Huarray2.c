#include <stdio.h>
#include <stdlib.h>
#include "uarray2.h"
#include "pnm.h" 

#define T UArray2_T

struct T {
    int height;
    int width;
    Pnm_rgb elements[];
};

/* Creates a UArray2_T that stores a c-array ***MIGHT WANT TO CHANGE NAME */
T UArray2_new(int h, int w, int size) {
    (void)size;
    T uarray2 = malloc(sizeof(T) + (h*w) * sizeof(Pnm_rgb));
    uarray2->height = h;
    uarray2->width = w;
    return uarray2;
}

/*Given a UArray2, a row, and a column, returns the index in a 1D array of the
  element in the given row and column */
static int index (T uarray2, int row, int column) {
    return (uarray2->width * row) + column;
} 

(&Pnm_rgb) UArray2_at(T uarray2, int row, int column) {
    return &uarray2->elements[index(uarray2, row, column)];
}

int UArray2_height(T uarray2) {
    return uarray2->height;
}

int UArray2_width(T uarray2) {
    return uarray2->width;
}

int UArray2_size(T uarray2) {
    return (uarray2->width)*(uarray2->height);
}

void UArray2_map_row_major(T uarray2, void apply(int row, int column,
            Pnm_rgb element, void *cl), void *cl) {
    int h = uarray2->height;
    int w = uarray2->width;
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            apply(r, c, uarray2->elements[index(uarray2, r, c)], cl); 
            //uarray2->elements[index(uarray2, r, c)], cl);
        }
    }
}

void UArray2_map_col_major(T uarray2, void apply(int row, int column, 
            void *element, void *cl), void *cl) {
    int h = uarray2->height;
    int w = uarray2->width;
    for (int c = 0; c < w; c++) {
        for (int r = 0; r < h; r++) {
            apply(r, c, uarray2->elements[index(uarray2, r, c)], cl);
        }
    }
}

void UArray2_free(T *uarray2) {
  free(*uarray2);
}

#undef T
