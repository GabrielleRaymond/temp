#include <a2plain.h>
#include "uarray2.h"

typedef A2Methods_UArray2 A2;

static A2 new(int width, int height, int size) {
  return UArray2_new(width, height, size);
}

static A2 new_with_blocksize(int width, int height, int size, int blocksize) {
  (void) blocksize;
  return UArray2_new(width, height, size);
}

static void a2free (A2 *array2p) {
  UArray2_free((UArray2_T *)array2p);
}

static int width(A2 array2) { return UArray2_width(array2); }
static int height(A2 array2) { return UArray2_height(array2); }
static int size(A2 array2) { return UArray2_size(array2); }

static int blocksize(A2 array2) { 
  (void) array2;
  return 1; 
}

static A2Methods_Object *at(A2 array2, int col, int row) { 
  return UArray2_at(array2, row, col);
}

struct small_closure {
  A2Methods_smallapplyfun *apply; 
  void *cl;
};

static void smallinnerapply(UArray2_T uarray2, int col, int row, void * cl)
{
  struct small_closure *cltemp = cl;
  cltemp->apply(UArray2_at(uarray2, row, col), cltemp->cl);
}

static void small_map_row_major(A2 a2, A2Methods_smallapplyfun apply, void *cl){
  struct small_closure mycl = { apply, cl};
  UArray2_map_row_major(a2, smallinnerapply, &mycl);
}

static void small_map_col_major(A2 a2, A2Methods_smallapplyfun apply, void *cl){
  struct small_closure mycl = { apply, cl};
  UArray2_map_column_major(a2, smallinnerapply, &mycl);
}

struct large_closure {
  A2Methods_applyfun *apply; 
  void *cl;
};

static void largeinnerapply(UArray2_T uarray2, int col, int row, void * cl) 
{
  struct large_closure *cltemp= cl;
  cltemp->apply(col, row, uarray2, UArray2_at(uarray2, row, col), 
            cltemp->cl);
}

static void map_row_major(A2 array2, A2Methods_applyfun apply, void * cl)
{
  struct large_closure mycl = { apply, cl};
  UArray2_map_row_major(array2, largeinnerapply, &mycl);
}
        
static void map_col_major(A2 array2, A2Methods_applyfun apply, void * cl)
{
  struct large_closure mycl = { apply, cl};
  UArray2_map_column_major(array2, largeinnerapply, &mycl);
}


static struct A2Methods_T uarray2_methods_plain_struct = {
  new,
  new_with_blocksize,
  a2free,
  width,
  height,
  size,
  blocksize,
  at,
  map_row_major,
  map_col_major,
  NULL, //map_block_major,
  map_row_major, // map_default
  small_map_row_major,
  small_map_col_major,
  NULL, //small_map_block_major,
  small_map_row_major, // small_map_default
};

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;

