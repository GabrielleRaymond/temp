#include "a2methods.h"

typedef struct Array2_T *Array2_T;
typedef void Array2_apply(int row, int col, void *elem, void *cl);
extern void Array2_map_row_major(Array2_T a2, Array2_apply apply, void *cl);


struct a2fun_closure {
  A2Methods_applyfun *apply; // apply function as known to A2Methods
  void *cl;               // closure that goes with that apply function
  A2Methods_UArray2 array2;   // array being mapped over
};

static void apply_a2methods_using_array2_prototype
                (int row, int col, void *elem, void *cl) 
{
  struct a2fun_closure *f = cl; // this is the function/closure originally passed
  f->apply(col, row, f->array2, elem, f->cl);
}

static void map_row_major(A2Methods_UArray2 array2, A2Methods_applyfun apply, void *cl) {
  struct a2fun_closure mycl = { apply, cl, array2 };
  Array2_map_row_major(array2, apply_a2methods_using_array2_prototype, &mycl);
}
int f(void (*m)(void *, A2Methods_applyfun, void*)) {
  return m == map_row_major;
}



