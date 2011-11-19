/* Universal Machine capable of running a program written in a set of 14
 * instructions */
#include <stdio.h>
#include <stdlib.h>
#include "segment.h"
#include "UMSegments.h"
#include "segment.h"
#include "assert.h"
#include "mem.h"
#include "uarray.h"
#include "seq.h"
#include "bitpack.h"

#ifndef _UM_INCLUDE
#define _UM_INCLUDE
#define T UM_T
typedef struct T* T;

/* It is a run time error to pass in a non valid file pointer. The um program
 * located in the file is loaded into the UM and run. */
void run(FILE *fp);

#undef T
#endif
