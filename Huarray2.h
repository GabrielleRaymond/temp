#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#define T UArray2_T
typedef struct T *T;

/*Creates a new UArray2_T with the specified width, height, and element size.*/
extern T  UArray2_new(int height, int width, int size);

/*Given a pointer to a UArray2_T, deallocates the UArray2_T.*/
extern void UArray2_free(T *uarray2);

/*Returns a pointer to the element in a UArray_T in the specified row and
  column */
extern (&Pnm_rgb) UArray2_at(T uarray2, int row, int column);

/* Returns the height of a UArray2_T */
extern int UArray2_height(T uarray2);

/* Returns the width of a UArray2_T */
extern int UArray2_width(T uarray2);

/* Returns the size of the elements of a UArray2_T */
extern int UArray2_size(T uarray2);

/* Calls function apply on each element of uarray2, row by row */
extern void UArray2_map_row_major(T uarray2, void apply(int row, int column,
                                  Pnm_rgb element, void*cl), void *cl);

/* Calls function apply on each element of uarray2, column by column */
extern void UArray2_map_col_major(T uarray2, void apply(int row, int column,
                                  Pnm_rgb element, void*cl), void *cl);

#undef T
#endif
