#include <segment.h>
#include "mem.h"

#define T Seg_T

struct T{
    UArray_T uarray;
};

/* Values stored are inherently 0 based on the contract of UArray_new */
T Seg_new(int length){
    T seg;
    NEW(seg);
    seg->uarray = UArray_new(length, sizeof(word32));
    return seg;
}

/* Puts the word in the segment at the indicated offset */
void Seg_put(T segment, int offset, word32 word){
    word32 *value;
    value = UArray_at(segment->uarray, offset);
    *value = word;
}

/* Returns the word in the segment at the indicated offset */
uint32_t Seg_get(T segment, int offset){
    word32 *value;
    value = UArray_at(segment->uarray, offset);
    return *value;
}

/* Frees the memory associated with the segment */
void Seg_free(T *ptr){
    assert(ptr!=NULL && *ptr!=NULL);
    UArray_free(&((*ptr)->uarray));
    FREE(*ptr);
}

/* Returns the length of the segment */
int Seg_length(T segment){
    return UArray_length(segment->uarray);
}

#undef T
