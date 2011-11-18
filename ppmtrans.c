#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "mem.h"

#define A2 A2Methods_UArray2

void applyRotate(int x, int y, A2 array2, A2Methods_Object *ptr, void *cl); 

typedef struct closure * closure;
struct closure{
    A2Methods_T  methods;
    Pnm_ppm * destination;
    int rotation;
};

int main(int argc, char *argv[]) {
    int rotation = 0;
    FILE * fp = NULL;

    A2Methods_T methods = uarray2_methods_plain; // default to UArray2 methods
    assert(methods);
    A2Methods_mapfun *map = methods->map_default; // default to best map
    assert(map);
#define SET_METHODS(METHODS, MAP, WHAT) do { \
    methods = (METHODS); \
    assert(methods); \
    map = methods->MAP; \
    if (!map) { \
        fprintf(stderr, "%s does not support " WHAT "mapping\n", argv[0]); \
        exit(1); \
    } \
} while(0)

    int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-row-major")) {
            SET_METHODS(uarray2_methods_plain, map_row_major, "row-major");
        } else if (!strcmp(argv[i], "-col-major")) {
            SET_METHODS(uarray2_methods_plain, map_col_major, "column-major");
        } else if (!strcmp(argv[i], "-block-major")) {
            SET_METHODS(uarray2_methods_blocked, map_block_major,"block-major");
        } else if (!strcmp(argv[i], "-rotate")) {
            assert(i + 1 < argc);
            char *endptr;
            rotation = strtol(argv[++i], &endptr, 10);
            assert(*endptr == '\0'); // parsed all correctly
            assert(rotation == 0   || rotation == 90
                    || rotation == 180 || rotation == 270); 
        } else if(!strcmp(argv[i], "-flip")) {  
            assert(i + 1 < argc);
            if(!strcmp(argv[++i], "horizontal"))
                rotation = 1;
            else if(!strcmp(argv[i], "vertical"))
                rotation = 2;
            else
                fprintf(stderr, "%s:unknown option '%s'\n", argv[0], argv[i+1]);
        }else if(!strcmp(argv[i], "-transpose")){
            rotation=3;
        }else if (*argv[i] == '-') {
            fprintf(stderr, "%s: unknown option '%s'\n", argv[0], argv[i]);
            exit(1);
        } else if (argc - i > 2) {
            fprintf(stderr, "Usage: %s [-rotate <angle>] "
                    "[-{row,col,block}-major] [filename]\n", argv[0]);
            exit(1);
        } else { //Case: input from file
            fp = fopen(argv[i], "r");
            if(fp == NULL)
                fprintf(stderr, "%s:Could not open file from stdin\n", argv[0]);
        }
    }

if(fp == NULL) //Case: input from stdin
        fp = stdin;

    //Reads in image
    Pnm_ppm originalImage;
    originalImage = Pnm_ppmread(fp, methods);

    //Creates destination for rotated image
    Pnm_ppm destImage; 
    NEW(destImage);
    destImage->methods = methods;
    destImage->denominator=originalImage->denominator;
 
    //Sets rotation specific variables
    if(rotation==90 || rotation==270){
        destImage->width = methods->height(originalImage->pixels);
        destImage->height = methods->width(originalImage->pixels);
    }
    if(rotation==180 || rotation == 0 || rotation == 1 || \
            rotation == 2 || rotation == 3) {
        destImage->width = methods->width(originalImage->pixels);
        destImage->height = methods->height(originalImage->pixels);
    }     

    destImage->pixels = methods->new(destImage->width, destImage->height,
            methods->size(originalImage->pixels));

    //rotation of image
    closure cl;
    NEW(cl);
    cl->methods = methods;
    cl->destination = &destImage;
    cl->rotation = rotation;
    map(originalImage->pixels, applyRotate, cl);

    //writing file and freeing memory
    Pnm_ppmwrite(stdout, destImage);
    Pnm_ppmfree(&originalImage);
    Pnm_ppmfree(&destImage);
    FREE(cl);
    if(fp!=stdin)
        fclose(fp);
    return 0;
}

//Rotates image
void applyRotate(int x, int y, A2 array2, A2Methods_Object *ptr, void *cl) 
{
    closure cltemp = cl;
    struct Pnm_rgb * temp;
    struct Pnm_rgb * ptr_pix= ptr;   

    switch(cltemp->rotation)
    {
        case 0:
            temp = cltemp->methods->at((*(cltemp->destination))->pixels, 
                    x,y);
            break;
        case 1: // Flip horizontal 
            temp = cltemp->methods->at((*(cltemp->destination))->pixels, 
                    (cltemp->methods->width(array2) - x - 1) ,y);
            break;
        case 2: // Flip vertical 
            temp = cltemp->methods->at((*(cltemp->destination))->pixels, 
                    x ,cltemp->methods->height(array2) - y - 1);
            break;
        case 3: // Transpose
            temp = cltemp->methods->at((*(cltemp->destination))->pixels, 
                   cltemp->methods->width(array2) - x - 1, 
                   cltemp->methods->height(array2) - y - 1);
            break;
        case 90:
            temp = cltemp->methods->at((*(cltemp->destination))->pixels, 
                    (cltemp->methods->height(array2) - y - 1) ,x);
            break;
        case 180:
            temp = cltemp->methods->at((*(cltemp->destination))->pixels, 
                    (cltemp->methods->width(array2) - x - 1) , 
                    (cltemp->methods->height(array2) - y - 1));
            break;
        case 270:
            temp = cltemp->methods->at((*(cltemp->destination))->pixels, 
                    y, cltemp->methods->width(array2) - x - 1);
            break;
    }
    *temp = *ptr_pix;
}
