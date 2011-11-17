#include <stdio.h>
#include <stdlib.h>
#include <segment.h>
#include <assert.h>
#include <UMSegments.h>
#include <except.h>

void check_newAndFree();
void check_mapping();
void check_putGetWord();
void check_getSeg(); 

int main(){
    check_newAndFree();
    check_mapping();
    check_putGetWord();
    check_getSeg(); 
    return 0;
}

void check_newAndFree()
{
    UMSegs_T segments;
    TRY
       //segments = UMSegs_new(-6);
    ELSE
        fprintf(stderr, "Cannot create a UMSegs_T with less than 1 segID\n");
    END_TRY;

    TRY
        segments = UMSegs_new(6);
    ELSE
        fprintf(stderr, "UMSegs_new failed to create a new UMSegs_T\n");
    return;
    END_TRY;

    TRY
        UMSegs_free(&segments);
    ELSE
        fprintf(stderr, "UMSegs_free was unable to free\n");
    return;
    END_TRY;

    TRY
        UMSegs_free(&segments);
    ELSE
        fprintf(stderr, "Cannot free a NULL ptr\n");
    END_TRY;

    TRY
        UMSegs_free(NULL);
    ELSE
        fprintf(stderr, "Cannot free NULL\n");
    END_TRY;
}

void check_mapping()
{
    int length = 6;
    uint32_t catch;
    UMSegs_T segments;
    segments = UMSegs_new(1);
    TRY
        //catch = UMSegs_mapSeg(segments, -3);
    ELSE
        fprintf(stderr, "Cannot create a segment with negative length\n");
    END_TRY;

    TRY
        catch = UMSegs_mapSeg(segments, length);
    ELSE
       fprintf(stderr, "Mapping failed\n");
    return;
    END_TRY;

    TRY
        catch = UMSegs_mapSeg(segments, 513);
        fprintf(stderr, "catch: %u ", catch);
    ELSE
        fprintf(stderr, "Failed to resize the UMSeg_T segID array\n");
    END_TRY;

    TRY
        catch = UMSegs_mapSeg(segments, 1025); // where 1025 is out of bounds 
    ELSE
        fprintf(stderr, "length is outside the bounds\n");
    END_TRY;

    TRY
        UMSegs_unmapSeg(segments, catch);
    ELSE
        fprintf(stderr, "Unmapping failed for ID: %u\n", catch);
    END_TRY;

    //This test will be run with a memory cap in place. It checks against memory
    //leaks that may occur unnoticed during the program

uint32_t segID, segID2;;
    for(int i = 0; i < 500000; i++){
segID = UMSegs_mapSeg(segments, length);
segID2 = UMSegs_mapSeg(segments, length);    
        UMSegs_unmapSeg(segments, segID); 
        UMSegs_unmapSeg(segments, segID2);
    }

    UMSegs_free(&segments);
}

void check_putGetWord()
{
    UMSegs_T segments;
    segments = UMSegs_new(1024);
    uint32_t value1 = (uint32_t)42;
    uint32_t value2 = (uint32_t)52;
    uint32_t segID;

    for(int i = 0; i < 90; i++){
    segID = UMSegs_mapSeg(segments, 90);
    TRY
        assert(UMSegs_getWord(segments, segID, i%90) == (uint32_t)0);
    ELSE
        fprintf(stderr, "Segments not initialized to zero\n");
    END_TRY;

    TRY
        UMSegs_putWord(segments, segID, i%90, value1);
    assert(UMSegs_getWord(segments, segID, i%90) == value1);
    ELSE
        fprintf(stderr, "Put, get did not return same value\n");
    END_TRY;

    TRY
        UMSegs_putWord(segments, segID, (i)%90, value1);
    UMSegs_putWord(segments, segID, (i+1)%90, value2);
    assert(UMSegs_getWord(segments, segID, i%90) == value1);
    ELSE
        fprintf(stderr, "Sucessive puts overwrote the first \n");
    END_TRY;

    UMSegs_unmapSeg(segments, segID);
}
UMSegs_free(&segments);      
}

void check_getSeg()
{

    UMSegs_T segments;
    segments = UMSegs_new(1024);
    Seg_T seg;
    uint32_t segID = UMSegs_mapSeg(segments, 90);
    UMSegs_putWord(segments, segID, 2, (uint32_t)42);

    TRY
        seg = UMSegs_getSeg(segments, segID);
    ELSE
        fprintf(stderr, "getSeg failed to get segment from segID\n");
    END_TRY;

    TRY
        assert(seg!=NULL);
    ELSE
        fprintf(stderr, "getSeg returned a NUll Segment\n");
        return;
    END_TRY;

    TRY
        assert((uint32_t)42==Seg_get(seg, 2));
    ELSE
        fprintf(stderr, "getSeg did not copy segment correctly \n");
    END_TRY;

    TRY
        UMSegs_unmapSeg(segments, segID);
        UMSegs_getSeg(segments, segID);
    ELSE
        fprintf(stderr, "Unmap seg freed the segments correctly \n");
    END_TRY;
    Seg_free(&seg);
    UMSegs_free(&segments);

}
