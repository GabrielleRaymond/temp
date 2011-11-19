#include <UMSegments.h>
#include "segment.h"
#include <seq.h>

#define T UMSegs_T
typedef uint32_t segmentID;

struct T{
    UArray_T segments;
    Seq_T seqUnmapped;
    int size;
};

/* Initializes UMSeg_T with allocSize number of unmapped segments. Each segID
 * (index) in UArray points to NULL, and seqUnmapped holds all unmapped IDS
 */
T UMSegs_new(int allocSize){
    assert(allocSize >= 0);
    T UMSegs;
    NEW(UMSegs);
    Seg_T *ptr;
    segmentID *segID;
    UMSegs->segments = UArray_new(allocSize, sizeof(ptr));
    UMSegs->seqUnmapped = Seq_new(allocSize);
    UArray_T segs = UMSegs->segments;
    Seq_T seq = UMSegs->seqUnmapped;
    for(int i = 0; i < allocSize; i++) {
        ptr = UArray_at(segs, i);
        *ptr = NULL;
        NEW(segID);
        *segID = i;
        Seq_addhi(seq, segID);
    }
    UMSegs->size = allocSize;
    return UMSegs;
}

/* Frees all memory allocated to the segments */
void UMSegs_free(T *ptr){
    assert(ptr!=NULL);
    assert(*ptr!=NULL);
    Seg_T **SegPtr;
    segmentID * segID;
    int length;
    length = (*ptr)->size;
    for(int i = 0; i < length; i++) {
        SegPtr = UArray_at((*ptr)->segments, i);
        if(*SegPtr!=NULL)
            UMSegs_unmapSeg(*ptr, i); 
    }
    UArray_free(&((*ptr)->segments));
    Seq_T seq = (*ptr)->seqUnmapped;
    length = Seq_length(seq);
    while(length != 0){
        segID = Seq_remhi(seq);
        FREE(segID);
        length--;
    }
    Seq_free(&((*ptr)->seqUnmapped));
    FREE(*ptr);
}

/* Returns the word located at 'offset' in the segment at the segID index of 
 * the array 
 */
uint32_t UMSegs_getWord(T segments, segmentID segID, int offset){
    Seg_T **seg;
    seg = UArray_at(segments->segments, (int)segID);
    return Seg_get(**seg, offset); 
}

/* Puts "value" at offset in the segment at the segID index of the array */
void UMSegs_putWord(T segments, segmentID segID, int offset, uint32_t value){
    Seg_T **seg;
    seg = UArray_at(segments->segments, (int)segID);
    Seg_put(**seg, offset, value);
}

/* Creates a mapped sequence corresponding to a previously unmapped ID */
uint32_t UMSegs_mapSeg(T segments, int length){
    Seg_T **seg;
    segmentID * segIDptr;
    segmentID segID;
    /* If there are no unmapped segIDs, resize */
    if(Seq_length(segments->seqUnmapped) == 0) {
        TRY     
            UArray_resize(segments->segments, 2*segments->size);
        ELSE 
            fprintf(stderr, "Resource exhaustion.\n");
        exit(1);
        END_TRY;   
        for(int i = segments->size; i < 2*segments->size; i++) {
            NEW(segIDptr);
            *segIDptr = i;           
            Seq_addhi(segments->seqUnmapped, segIDptr);
            seg = UArray_at(segments->segments, *segIDptr);
            *seg = NULL;
        }
        segments->size = 2*segments->size;
    }

    segIDptr = Seq_remlo(segments->seqUnmapped);
    segID = *segIDptr;
    FREE(segIDptr);

    seg = UArray_at(segments->segments, (int)segID);
    NEW(*seg);
    **seg = Seg_new(length);
    assert(seg != NULL);
    assert(*seg != NULL);
    return segID;
}

/* Segment freed and segID is placed into list of Unmapped segments */
void UMSegs_unmapSeg(T segments, segmentID segID){
    Seg_T **seg;
    seg = UArray_at(segments->segments, (int)segID);
    Seg_free(*seg);
    FREE(*seg);
    segmentID * segIDstore;
    NEW(segIDstore);
    *segIDstore = segID;
    Seq_addlo(segments->seqUnmapped, segIDstore);
}

/* Returns a duplicate of the segment corresponding the the segID */
Seg_T UMSegs_getSeg(T segments, uint32_t segID){       
     Seg_T **seg;
     Seg_T segDup;
     seg = UArray_at(segments->segments, (int)segID);
     assert(seg!=NULL && *seg!=NULL);
     int length = Seg_length(**seg);
     segDup = Seg_new(length);
     for(int i = 0; i < length; i++){
        Seg_put(segDup, i, Seg_get(**seg, i));
      }
    return segDup;
}

#undef T
