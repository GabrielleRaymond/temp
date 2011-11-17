#include <stdio.h>
#include "mem.h"
#include "assert.h"
#include "uarray2.h"
#include "uarray2b.h"
#include "uarray.h"
#include "math.h"

#define T UArray2b_T 

static void applyInitialize(UArray2_T UArray2, int col, int row, void *cl);
static void applyFree(UArray2_T UArray2, int col, int row, void *cl); 

/* INV: blocksize = sqrt(#of elements in each UArray_T) 
   INV: for an element at index [x,y], its block index is [x/blocksize,
        y/blocksize]
   INV: the index of an element within a block is blocksize*(x % blocksize) + 
        (y % blocksize)
   INV: To find the (blocked array) coordinates of an element from its index in
        the UArray: 
        Row Coordinate = (index / blocksize) + (row of block * blocksize) 
        Column Coordinate = (index % blocksize) + (blocksize * column of block)

   INV: width, height, size, and blocksize are all positive

   There is a 1:1 mapping of each individual element in our representation 
   to its location in the blocked UArray_T    
*/


struct T {
        UArray2_T UArray2;
        int width;
        int height;
        int size;
        int blocksize;
};

static void applyInitialize(UArray2_T UArray2, int col, int row, void *cl)
{
        T * temp2b = cl;
        UArray_T *temp;
        temp = UArray2_at(UArray2, row, col);
        *temp = UArray_new((*temp2b)->blocksize * (*temp2b)->blocksize,
            (*temp2b)->size);
}

static void applyFree(UArray2_T UArray2, int col, int row, void *cl)
{
        (void) cl;
        UArray_T *temp;
        temp = UArray2_at(UArray2, row, col);
        UArray_free(temp);
}

/* This function creates a new blocked array, represented as a UArray2_T with
 * elements that are UArray_Ts. Each UArray_T represents a block and stores the
 * individual elements of the UArray2b.  
 */
extern T UArray2b_new(int width, int height, int size, int blocksize) 
{
        assert(width > 0 && height > 0 && size > 0);    
        T UArray2b;
        NEW(UArray2b);
        
        UArray2b->width = width;
        UArray2b->height = height;
        UArray2b->size = size;
        UArray2b->blocksize = blocksize;
        int array2width=width/blocksize;
        int array2height=height/blocksize;
        UArray2b->UArray2 = UArray2_new(array2width+1, array2height+1,
            sizeof(UArray_T));
        UArray2_map_row_major(UArray2b->UArray2, applyInitialize, &UArray2b);
        return UArray2b;
}

/* Calculates the largest blocksize that will fit within 64KB and calls
 * UArray2b_new
 */
extern T UArray2b_new_64K_block(int width, int height, int size) 
{
        assert(size > 0);
        int blocksize;
        if(size < 65536) 
                blocksize = (int)sqrt(65536/size); 
        else    
                blocksize = 1;
        return UArray2b_new(width, height, size, blocksize);
}

/* Frees all data stored in the UArray2b_T*/
extern void UArray2b_free(T *array2b)
{
        assert(array2b != NULL && *array2b != NULL);
        UArray2_map_row_major((*array2b)->UArray2, applyFree, NULL);
        UArray2_free(&((*array2b)->UArray2));
        FREE(*array2b);
}

extern int UArray2b_width(T array2b) { return array2b->width; }

extern int UArray2b_height(T array2b) { return array2b->height; }

extern int UArray2b_size(T array2b) { return array2b->size; }

extern int UArray2b_blocksize(T array2b) { return array2b->blocksize; }

extern void *UArray2b_at(T array2b, int x, int y)
{
        assert(array2b != NULL);
        assert(x >= 0 && x < UArray2b_width(array2b));
        assert(y >=0 && y < UArray2b_height(array2b));
        int blocksize = UArray2b_blocksize(array2b);    
        int blockX = x / blocksize;
        int blockY = y / blocksize;
        int goOver = (blocksize * (x % blocksize)) + (y % blocksize);
                
        UArray_T * array = UArray2_at(array2b->UArray2, blockY, blockX);
        
        return UArray_at(*array, goOver); 
}

/* Maps in block major order */
extern void UArray2b_map(T uarray2b, void apply(int x, int y, 
                        T array2b, void *elem, void *cl), void *cl)
{
    assert(uarray2b != NULL);
        assert(uarray2b->UArray2 !=NULL);
        assert(apply);
        UArray_T * temp = NULL;
        int rowcoord;
        int colcoord;
        int blocksize = uarray2b->blocksize;
        for(int y=0; y < UArray2_height(uarray2b->UArray2); y++)
        {
                for(int x=0; x < UArray2_width(uarray2b->UArray2); x++)
                {
                        temp = UArray2_at(uarray2b->UArray2, y, x);
                        for(int i = 0; i < blocksize*blocksize; i++)
                        {
                                rowcoord = (i / blocksize) + y * blocksize;
                                colcoord = i % blocksize + blocksize * x;
                                if(rowcoord < uarray2b->height && 
                                   colcoord < uarray2b->width)
                                        apply(colcoord, rowcoord, uarray2b, 
                                              UArray2b_at(uarray2b, 
                                              colcoord, rowcoord), cl);
                        }
                }
        }
}

#undef T
