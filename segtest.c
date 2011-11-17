#include<stdio.h>
#include<stdlib.h>
#include<segment.h>
#include<assert.h>
#include<except.h>

void check_newfree();
void check_length();
void check_putget();

int main()
{

    check_newfree();
    check_length();
    check_putget();
    return 0;
} 

void check_newfree(){
    Seg_T seg;
    TRY
        //seg = Seg_new(1); 
    ELSE
        fprintf(stderr, "Assert raised on new for negative length\n");
    END_TRY;

    TRY
        seg = Seg_new(5);
    ELSE
        fprintf(stderr, "Unable to create a segment of length 5\n");
    return;
    END_TRY;

    TRY
        Seg_free(&seg);
    ELSE
        fprintf(stderr, "Failed to free segment\n");
    return;
    END_TRY;

    TRY
        Seg_free(&seg);
    ELSE
        fprintf(stderr, "Assert raised for trying to free a NULL segment\n");
    END_TRY;

    TRY 
        Seg_free(NULL);
    ELSE
        fprintf(stderr, "Assert raised for trying to free a NULL pointer \n");
    END_TRY;

}

void check_length(){
    Seg_T seg;
    TRY
        seg = Seg_new(6);
    assert(Seg_length(seg) == 6);
    ELSE
        fprintf(stderr, "Length does not return the proper length \n");
    END_TRY;
    Seg_free(&seg);
}

void check_putget(){
    Seg_T seg = Seg_new(6);

    for(int i=0; i<6; i++){
        TRY
            assert(Seg_get(seg, i)==(uint32_t)(0));
        ELSE 
            fprintf(stderr, "Segment word values not initialized to zero, \
value is %u \n", Seg_get(seg,i));
        END_TRY;

        TRY
            Seg_put(seg, i, i+1);
        assert(Seg_get(seg, i) == (uint32_t)(i+1));
        ELSE
            fprintf(stderr, "Put at offset and get at offset %d doesn't return\
value, returned is %u \n", i, Seg_get(seg, i));
        END_TRY;

    }

    TRY
        Seg_put(seg, 7, 1);
    ELSE
        fprintf(stderr, "Assert raised trying to put out of bounds\n");
    END_TRY;

    TRY 
        Seg_get(seg, 7);
    ELSE
        fprintf(stderr, "Assert raised trying to get out of bounds \n");
    END_TRY;
    Seg_free(&seg);
}
