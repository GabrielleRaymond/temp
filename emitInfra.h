#include <seq.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "assert.h"
#include "fmt.h"
#include "seq.h"
#include "inttypes.h"
#include "bitpack.h"

#ifndef _EMITINFRA_INCLUDE 
#define _EMITINFRA_INCLUDE

typedef uint32_t Um_instruction;
typedef uint32_t Um_opcode;

extern void add_label(Seq_T stream, int location_to_patch, int label_value); 
extern Um_instruction three_register(Um_opcode op, int rA, int rB, int rC);
extern void put_inst(Seq_T stream, int location_to_patch, Um_instruction inst);
extern void emit(Seq_T stream, Um_instruction inst);
extern Um_instruction get_inst(Seq_T stream, int location_to_patch);
extern Um_instruction loadval(unsigned rA, unsigned val);
extern void emit_out_string(Seq_T stream, const char* toPrint, int reg);

extern Um_instruction condM(int rA, int rB, int rC);
extern Um_instruction segL(int rA, int rB, int rC);
extern Um_instruction segS(int rA, int rB, int rC);
extern Um_instruction add(int rA, int rB, int rC);
extern Um_instruction mult(int rA, int rB, int rC);
extern Um_instruction divi(int rA, int rB, int rC);
extern Um_instruction nand(int rA, int rB, int rC);
extern Um_instruction halt();
extern Um_instruction mapS(int rB, int rC);
extern Um_instruction unmapS(int rC);
extern Um_instruction output(int rC);
extern Um_instruction input(int rC);
extern Um_instruction loadP(int rB, int rC);
extern Um_instruction loadV(unsigned rA, unsigned val);

#endif
