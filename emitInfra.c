#include <emitInfra.h>

enum ops { CONDM = 0, SEGL, SEGST, ADD, MULT, DIVI, NAND, HALT, MAPS, UNMAPS,
    OUT, IN, LOADP, LOADV };  

enum regs { r0 = 0, r1, r2, r3, r4, r5, r6, r7 };


extern void add_label(Seq_T stream, int location_to_patch, int label_value) {
    Um_instruction inst = get_inst(stream, location_to_patch);
    unsigned k = Bitpack_getu(inst, 25, 0);
    inst       = Bitpack_newu(inst, 25, 0, label_value+k);
    put_inst(stream, location_to_patch, inst);
}

//Packs the register number and opcode into a new word;
extern Um_instruction three_register(Um_instruction op, int rA, int rB, int rC)
{
    Um_instruction word = 0;
    word = Bitpack_newu(word, 4, 28, op);
    word = Bitpack_newu(word, 3, 0, rC);
    word = Bitpack_newu(word, 3, 3, rB);
    word = Bitpack_newu(word, 3, 6, rA);
    return word;
}

//Packs the value, register number, and the opcode into the new word
extern Um_instruction loadval(unsigned rA, unsigned val)
{
    Um_instruction word = 0;
    word = Bitpack_newu(word, 4, 28, LOADV);
    word = Bitpack_newu(word, 3, 25, rA);
    word = Bitpack_newu(word, 25, 0, val); 
    return word;
}

//Puts the newly formed instruction in the sequence to be written to file
extern void put_inst(Seq_T stream, int location_to_patch, Um_instruction inst) {
    Seq_put(stream, location_to_patch, (void *)(uintptr_t)inst);
}

//Gets word in the sequence to patch a value
extern Um_instruction get_inst(Seq_T stream, int location_to_patch) {
    return (Um_instruction)(uintptr_t)(Seq_get(stream, location_to_patch));
}


extern void emit(Seq_T stream, Um_instruction inst){
    assert(sizeof(inst) <= sizeof(uintptr_t));
    Seq_addhi(stream, (void *)(uintptr_t)inst);
}

//writes the code to output each individual character of a string
extern void emit_out_string(Seq_T stream, const char* toPrint, int reg)
{
    int length = strlen(toPrint);
    for(int i = 0; i < length; i++) {
        emit(stream, loadV(reg, toPrint[i]));
        emit(stream, output(reg));
    }
}

extern Um_instruction condM(int rA, int rB, int rC) {
    return three_register(CONDM, rA, rB, rC);
}

extern Um_instruction segL(int rA, int rB, int rC) {
    return three_register(SEGL, rA, rB, rC);
}

extern Um_instruction segS(int rA, int rB, int rC) {
    return three_register(SEGST, rA, rB, rC);
}

extern Um_instruction add(int rA, int rB, int rC) {
    return three_register(ADD, rA, rB, rC);
}

extern Um_instruction mult(int rA, int rB, int rC) {
    return three_register(MULT, rA, rB, rC);
}

extern Um_instruction divi(int rA, int rB, int rC) {
    return three_register(DIVI, rA, rB, rC);
}

extern Um_instruction nand(int rA, int rB, int rC) {
    return three_register(NAND, rA, rB, rC);
}

extern Um_instruction halt() {
    return three_register(HALT, 0, 0, 0);
}

extern Um_instruction mapS(int rB, int rC) {
    return three_register(MAPS, 0, rB, rC);
}

extern Um_instruction unmapS(int rC) {
    return three_register(UNMAPS, 0, 0, rC);
}

extern Um_instruction output(int rC) {
    return three_register(OUT, 0, 0, rC);
}

extern Um_instruction input(int rC) {
    return three_register(IN, 0, 0, rC);
}

extern Um_instruction loadP(int rB, int rC) {
    return three_register(LOADP, 0, rC, rB);
}

extern Um_instruction loadV(unsigned rA, unsigned val) {
    return loadval(rA, val);
}
