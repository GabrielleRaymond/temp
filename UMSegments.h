/* Represents a large address space divided into segments */
#include <segment.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include <mem.h>

#ifndef _UMSEGS_INCLUDE
#define _UMSEGS_INCLUDE
#define T UMSegs_T
typedef struct T* T;

/* It is a checked runtime error to pass in an unmapped segID to getWord,
 * putWord, and getSeg. offset must also be within the bounds of [0, length).
 */

T UMSegs_new(int allocSize);
//Contract: returns a UMSegs_T containing allocSize number of unmapped segment
//identifiers. Checked runtime error to pass in a allocSize <= 0.

void UMSegs_free(T *ptr);
//Contract: frees all memory allocated to the UMSegs_T. Checked runtime error to
//pass in a null pointer. 

uint32_t UMSegs_getWord(T segments, uint32_t segID, int offset);
//Contract: gets the word contained in the segment refered to by the segID
//at that particular offset. 

void UMSegs_putWord(T segments, uint32_t segID, int offset, uint32_t value);
//Contract: place the word "value" in segment indicated by segID at the offset

uint32_t UMSegs_mapSeg(T segments, int length);
//Contract: maps the segment of length 'length;' returns the segID of the newly
//mapped segment. Checked runtime error to pass in a negative length. 

void UMSegs_unmapSeg(T segments, uint32_t segID);
//Contract: unmaps the segment refered to by segID, segment identifier is 
//available for reassignment. 

Seg_T UMSegs_getSeg(T segments, uint32_t segID);
//Contract: returns a duplicate of the entire segment stored at that segID

#undef T
#endif
