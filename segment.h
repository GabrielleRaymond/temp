/* A collection of unboxed containers that hold words */
#include <stdlib.h>
#include <stdio.h>
#include <uarray.h>
#include <assert.h>
#include <inttypes.h>
#ifndef _SEG_INCLUDE
#define _SEG_INCLUDE
#define T Seg_T
typedef struct T *T;
typedef uint32_t word32;

/* Checked runtime error if the bounds length > 0 and offset in [0, length) are
 * not met for Seg_put and Seg_get. It is also a checked runtime error to pass
 * in a NULL segment or segment pointer to any of the functions. 
 */


/* Returns a segment where every word is initialize to 0, checked run time error
 * to pass in a negative length */
T Seg_new(int length);

/* Puts word at the offset in the given segment */
void Seg_put(T segment, int offset, word32 word);

/* Gets a word at the offset in the given segment */
word32 Seg_get(T segment, int offset);

/* Frees every element within the given segment and frees the segment itself */
void Seg_free(T * ptr);

/* Returns the length of the given segment */
int Seg_length(T segment);

#undef T
#endif
