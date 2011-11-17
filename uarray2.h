#include <stdlib.h>
#include <stdio.h>
#include <uarray.h>
#include <assert.h>
#include <string.h>

#ifndef _ARRAY2_INCLUDE
#define _ARRAY2_INCLUDE

#define T UArray2_T    
typedef struct T* T;


/* Contract: passed non-zero values for width and height and a value at
   least 1 for size, we return an initialized and allocated 2D
   array with width bounds (0, width-1), height bounds
   (0, height-1), and size bytes allocated (each with an initial
   value of 0) at each array position.
*/

extern T UArray2_new(int width, int height, int size);

// Contract: passed a pointer to a UArray2, we deallocate the array and all
//        its contents.
extern void UArray2_free(T* uarray2);

//     Contract: passed a UArray2, we return its height.
int UArray2_height(T uarray2);

// Contract: passed a UArray2, we return its width.
extern int UArray2_width(T uarray2);

// Contract: passed a UArray2, we return its width * height.
extern int UArray2_area(T uarray2);

// Contract: passed a UArray2, we return the number of bytes allocated
// for the data at each array position.
extern int UArray2_size(T uarray2);    

// Contract: passed a UArray2 and non-negative values for row and column, 
//        we return a pointer to the data at those indices.
extern void* UArray2_at(T uarray2, int row, int column);


void UArray2_map_column_major(T uarray2, 
    void apply(T uarray2, int col, int row, void* cl), 
    void* cl);
    
void UArray2_map_row_major(T uarray2, 
    void apply(T uarray2, int col, int row, void* cl), 
    void* cl);
    
#undef T
#endif
