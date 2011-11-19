#include <segment.h>
#include "mem.h"
#define T Seg_T

struct T{
    int length;
    word32 array[];
};

/* Initializes a segment to store length number of 0s */
T Seg_new(int length){
    T seg;
    seg = malloc(sizeof(T) + length * sizeof(word32));
    seg->length = length;
    for(int i = 0; i < length; i++) {
        seg->array[i] = 0;
    }
    return seg;
}

/* Puts the word in the segment at the indicated offset */
void Seg_put(T segment, int offset, word32 word){
    segment->array[offset] = word;
}

/* Returns the word in the segment at the indicated offset */
uint32_t Seg_get(T segment, int offset){
    return segment->array[offset];
}

/* Frees the memory associated with the segment */
void Seg_free(T *ptr){
    FREE(*ptr);
}

/* Returns the length of the segment */
int Seg_length(T segment){
    return segment->length;
}

#undef T
