#include <stdlib.h>
#include <stdio.h>
#include <uarray.h>
#include <string.h>
#include "uarray2.h"

#define T UArray2_T

struct T
{
    Pnm_ppm array[];
    int length;
};

// INV: UArray_length(ua->uarray) == ua->width * height of ua2
//      therefore height of ua2 = length(ua2->uarray) / ua2->width
//
// INV: abstract bit at location (col, row) == bit at location (row * width + col)
// INV: width is positive
// INV: UArray_length(ua2->uarray) is positive
// INV: ua2->uarray != NULL

// We assert that the invariant ensures that for any (col, row) pair
// in bounds there is exactly one corresponding position in
// ua2->uarray


struct cr { // (col, row) coordinate
  int col;
  int row;
};

static inline int index_of_cr(T ua2, int col, int row) {
  return row * ua2->width + col;
}

static inline struct cr cr_of_index(T ua2, int i) {
  int w = ua2->width;
  struct cr coordinates;
  coordinates.row = i / w;
  coordinates.col = i - coordinates.row * w;
  return coordinates;
}

T UArray2_new(int width, int height, int size)
{
    T uarray2;
    uarray2 = malloc((long)sizeof * uarray2);
    uarray2->uarray = UArray_new(width*height, size);
    uarray2->width = width;
    return uarray2;
}

void UArray2_free(T* uarray2)
{
    T ua2 = *uarray2;
    UArray_free(&(ua2->uarray)); 
    free(*uarray2);
}


int UArray2_height(T uarray2)
{
    return UArray2_area(uarray2) / UArray2_width(uarray2);
}

int UArray2_width(T uarray2)
{
    return uarray2->width;
}

int UArray2_area(T uarray2)
{
    return UArray_length(uarray2->uarray);
}

int UArray2_size(T uarray2)
{
    return UArray_size(uarray2->uarray);
}
    
void* UArray2_at(T uarray2, int row, int column)
{
    return UArray_at(uarray2->uarray, index_of_cr(uarray2, column, row));    
}

void UArray2_map_row_major(T uarray2, 
    void apply(T uarray2, int col, int row, void* cl), 
    void* cl)
{
    int area = UArray2_area(uarray2);
    for (int i = 0; i < area; i++)
    {
	struct cr temp = cr_of_index(uarray2, i);
        int row = temp.row;
        int column = temp.col;
        apply(uarray2, column, row, cl);
    }    
}
    
void UArray2_map_column_major(T uarray2, 
    void apply(T uarray2, int col, int row, void* cl), 
    void* cl)
{
    int w = UArray2_width(uarray2);
    int h = UArray2_height(uarray2);
    for (int column = 0; column < w; column++)
        for (int row = 0; row < h; row++)
        {
            apply(uarray2, column, row, cl);
        }    
}

#undef T
