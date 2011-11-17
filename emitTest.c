#include <emitTest.h>
enum ops { CONDM = 0, SEGL, SEGST, ADD, MULT, DIVI, NAND, HALT, MAPS, UNMAPS,
    OUT, IN, LOADP, LOADV };  

enum regs { r0 = 0, r1, r2, r3, r4, r5, r6, r7 };

extern void emit_halt_test(Seq_T stream) {
    emit(stream, halt());
    emit(stream, loadV(r1, 'B'));
    emit(stream, output(r1));
    emit(stream, loadV(r1, 'a'));
    emit(stream, output(r1));
    emit(stream, loadV(r1, 'd'));
    emit(stream, output(r1));
    emit(stream, loadV(r1, '!'));
    emit(stream, output(r1));
    emit(stream, loadV(r1, '\n'));
    emit(stream, output(r1));
}

extern void emit_goto_test(Seq_T stream) {
    int patch_L = Seq_length(stream);
    emit(stream, loadV(r7, 0));  // will be patched to 'r7 := L'
    emit(stream, loadP(r7, r0)); // should goto label L
    emit_out_string(stream, "GOTO failed.\n", r1);
    emit(stream, halt());
    add_label(stream, patch_L, Seq_length(stream)); // define 'L' to be here
    emit_out_string(stream, "GOTO passed.\n", r1);
    emit(stream, halt());
} 

extern void emit_output_test(Seq_T stream) {
    emit(stream, loadV(r2, 32));
    emit(stream, output(r2));
    emit(stream, loadV(r2, '\n'));
    emit(stream, output(r2));
    emit(stream, loadV(r2, 46));
    emit(stream, output(r2));
    emit(stream, loadV(r2, '\n'));
    emit(stream, output(r2));
    emit(stream, loadV(r2, 94));
    emit(stream, output(r2));
    emit(stream, loadV(r2, '\n'));
    emit(stream, output(r2));
    emit(stream, loadV(r2, 126));
    emit(stream, output(r2));
    emit(stream, loadV(r2, '\n'));
    emit(stream, output(r2));
    emit_out_string(stream, "Output passed.\n", r1);
    emit(stream, halt());
}

extern void emit_conditionalMove_test(Seq_T stream) {

    emit(stream, loadV(r1, 1)); 
    int patch_L1 = Seq_length(stream);
    emit(stream, loadV(r2, 0)); //will be patched to r2 := L1
    int patch_L2 = Seq_length(stream);
    emit(stream, loadV(r3, 0)); //will be patched to r3 := L2
    emit(stream, condM(r2, r3, r1)); //evaluates to true
    emit(stream, loadP(r2, r0));
    add_label(stream, patch_L1, Seq_length(stream)); // define L1 here
    emit_out_string(stream, "CondMove failed.\n", r4);
    emit(stream, halt());
    add_label(stream, patch_L2, Seq_length(stream)); // define L2 here
    emit_out_string(stream, "CondMove passed.\n", r4);
    emit(stream, halt());
}

extern void emit_input_test(Seq_T stream) {
    emit(stream, input(r1));
    emit(stream, loadV(r2, 99));
    emit(stream, nand(r2, r2, r2));
    emit(stream, nand(r3, r1, r2));
    emit(stream, loadV(r7, 0));
    emit(stream, nand(r7, r7, r7));
    emit(stream, nand(r3, r3, r7));
    int patch_L1 = Seq_length(stream);
    emit(stream, loadV(r4, 0)); //will be patched to r4 := L1 "passed"
    int patch_L2 = Seq_length(stream);
    emit(stream, loadV(r5, 0)); //will be patched to r5 := L2 "failed"
    emit(stream, condM(r4, r5, r3));//should evaluate to false
    emit(stream, loadP(r4, r0)); 
    add_label(stream, patch_L1, Seq_length(stream)); //define L1 here
    emit_out_string(stream, "Input passed.\n", r7);
    emit(stream, halt());
    add_label(stream, patch_L2, Seq_length(stream)); //define L2 here
    emit_out_string(stream, "Input failed. \n", r7);
    emit(stream, halt());
}

extern void emit_nand_test(Seq_T stream) {
    emit(stream, loadV(r1, 42));
    emit(stream, loadV(r2, 42));
    emit(stream, nand(r2, r2, r2));
    emit(stream, nand(r3, r1, r2));
    emit(stream, loadV(r7, 0));
    emit(stream, nand(r7, r7, r7));
    emit(stream, nand(r3, r3, r7));
    int patch_L1 = Seq_length(stream);
    emit(stream, loadV(r4, 0)); //will be patched to r4 := L1 "passed"
    int patch_L2 = Seq_length(stream);
    emit(stream, loadV(r5, 0)); //will be patched to r5 := L2 "failed"
    emit(stream, condM(r4, r5, r3));//should evaluate to false
    emit(stream, loadP(r4, r0)); 
    add_label(stream, patch_L1, Seq_length(stream)); //define L1 here
    emit_out_string(stream, "NAND passed.\n", r7);
    emit(stream, halt());
    add_label(stream, patch_L2, Seq_length(stream)); //define L2 here
    emit_out_string(stream, "NAND failed. \n", r7);
    emit(stream, halt());
}

extern void emit_add_test(Seq_T stream) {
    emit_out_string(stream, "1 + 2 = _\n", r1);
    emit(stream, input(r2));
    emit(stream, loadV(r1, 50));
    emit(stream, loadV(r7, 1));
    emit(stream, add(r1, r7, r1));
    emit(stream, nand(r2, r2, r2));
    emit(stream, nand(r3, r1, r2));
    emit(stream, loadV(r7, 0));
    emit(stream, nand(r7, r7, r7));
    emit(stream, nand(r3, r3, r7));
    int patch_L1 = Seq_length(stream);
    emit(stream, loadV(r4, 0)); //will be patched to r4 := L1 "passed"
    int patch_L2 = Seq_length(stream);
    emit(stream, loadV(r5, 0)); //will be patched to r5 := L2 "failed"
    emit(stream, condM(r4, r5, r3));//should evaluate to false
    emit(stream, loadP(r4, r0)); 
    add_label(stream, patch_L1, Seq_length(stream)); //define L1 here
    emit_out_string(stream, "Add passed.\n", r7);
    emit(stream, halt());
    add_label(stream, patch_L2, Seq_length(stream)); //define L2 here
    emit_out_string(stream, "Add failed. \n", r7);
    emit(stream, halt());
}

extern void emit_mult_test(Seq_T stream) {
    emit_out_string(stream, "1 x 2 = _\n", r1);
    emit(stream, input(r2));
    emit(stream, loadV(r1, 25));
    emit(stream, loadV(r7, 2));
    emit(stream, mult(r1, r7, r1));
    emit(stream, nand(r2, r2, r2));
    emit(stream, nand(r3, r1, r2));
    emit(stream, loadV(r7, 0));
    emit(stream, nand(r7, r7, r7));
    emit(stream, nand(r3, r3, r7));
    int patch_L1 = Seq_length(stream);
    emit(stream, loadV(r4, 0)); //will be patched to r4 := L1 "passed"
    int patch_L2 = Seq_length(stream);
    emit(stream, loadV(r5, 0)); //will be patched to r5 := L2 "failed"
    emit(stream, condM(r4, r5, r3));//should evaluate to false
    emit(stream, loadP(r4, r0)); 
    add_label(stream, patch_L1, Seq_length(stream)); //define L1 here
    emit_out_string(stream, "Mult passed.\n", r7);
    emit(stream, halt());
    add_label(stream, patch_L2, Seq_length(stream)); //define L2 here
    emit_out_string(stream, "Mult failed. \n", r7);
    emit(stream, halt());
}

extern void emit_MapUnmap_test(Seq_T stream) {
    emit(stream, loadV(r6, 500));
    emit(stream, nand(r6, r6, r6));
    emit(stream, loadV(r7, 0));
    emit(stream, nand(r7, r7, r7));
    emit(stream, loadV(r1, 1));
    emit(stream, loadV(r2, 0));
    emit(stream, loadV(r3, 6)); //start of loop;
    emit(stream, add(r2, r2, r1)); //incrementing counter
    emit(stream, mapS(r0, r2));
    emit(stream, unmapS(r0));
    emit(stream, nand(r5, r2, r6));
    emit(stream, nand(r5, r5, r7)); 
    int patch_L2 = Seq_length(stream);
    emit(stream, loadV(r4, 0));
    emit(stream, condM(r4, r3, r5)); // loop until r2 == 500000
    emit(stream, loadV(r0, 0));
    emit(stream, loadP(r4, r0));
    add_label(stream, patch_L2, Seq_length(stream)); // define L2 here
    emit(stream, halt());
}

extern void emit_div_test(Seq_T stream) {
    emit_out_string(stream, "2 / 1 = _\n", r1);
    emit(stream, input(r2));
    emit(stream, loadV(r1, 2));
    emit(stream, loadV(r7, 100));
    emit(stream, divi(r1, r7, r1));
    emit(stream, nand(r2, r2, r2));
    emit(stream, nand(r3, r1, r2));
    emit(stream, loadV(r7, 0));
    emit(stream, nand(r7, r7, r7));
    emit(stream, nand(r3, r3, r7));
    int patch_L1 = Seq_length(stream);
    emit(stream, loadV(r4, 0)); //will be patched to r4 := L1 "passed"
    int patch_L2 = Seq_length(stream);
    emit(stream, loadV(r5, 0)); //will be patched to r5 := L2 "failed"
    emit(stream, condM(r4, r5, r3));//should evaluate to false
    emit(stream, loadP(r4, r0)); 
    add_label(stream, patch_L1, Seq_length(stream)); //define L1 here
    emit_out_string(stream, "Div passed.\n", r7);
    emit(stream, halt());
    add_label(stream, patch_L2, Seq_length(stream)); //define L2 here
    emit_out_string(stream, "Div failed. \n", r7);
    emit(stream, halt());
}

extern void emit_segLoadUnload_test(Seq_T stream) {
    emit(stream, loadV(r6, 12));
    emit(stream, loadV(r0, 121212));
    emit(stream, nand(r6, r6, r6));
    emit(stream, loadV(r7, 0));
    emit(stream, nand(r7, r7, r7));
    emit(stream, loadV(r1, 1));
    emit(stream, loadV(r2, 0));
    emit(stream, loadV(r3, 7)); //start of loop;
    emit(stream, add(r2, r2, r1)); //incrementing counter
    emit(stream, mapS(r3, r0)); 
    emit(stream, loadV(r4, 0));
    emit(stream, segS(r3, r2, r6));
    emit(stream, segL(r6, r3, r4)); 
    emit(stream, nand(r5, r2, r6));
    emit(stream, nand(r5, r5, r7)); 
    int patch_L2 = Seq_length(stream);
    emit(stream, loadV(r4, 0));
    emit(stream, condM(r4, r3, r5)); // loop until r2 == 12
    emit(stream, loadV(r0, 0));
    emit(stream, loadP(r4, r0));
    add_label(stream, patch_L2, Seq_length(stream)); // define L2 here
    emit_out_string(stream, "Seg load and store passed. \n", r6);
    emit(stream, halt());
}

//writes a program to output HELLO/n in a segment and then loads that program
extern void emit_loadP_test(Seq_T stream){
    emit(stream, loadV(r1, 100));
    emit(stream, mapS(r2, r1));
    emit(stream, loadV(r4, 0));
    emit(stream, loadV(r5, 1));
    int patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0)); 
    emit(stream, loadP(r6, r0));
    emit(stream, loadV(r7, 'H')); //loading H
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2)); 
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));
    emit(stream, add(r4, r4, r5));

    patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0));
    emit(stream, loadP(r6, r0));
    emit(stream, output(r7)); //outputing H
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2)); 
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));
    emit(stream, add(r4, r4, r5));

    patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0));
    emit(stream, loadP(r6, r0));
    emit(stream, loadV(r7, 'E')); //loading E
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2)); 
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));
    emit(stream, add(r4, r4, r5));

    patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0));
    emit(stream, loadP(r6, r0));
    emit(stream, output(r7)); //outputing E
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2)); 
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));
    emit(stream, add(r4, r4, r5)); 

    patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0));
    emit(stream, loadP(r6, r0));
    emit(stream, loadV(r7, 'L')); //loading L
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2)); 
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));
    emit(stream, add(r4, r4, r5)); 

    patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0));
    emit(stream, loadP(r6, r0));
    emit(stream, output(r7)); //outputing L
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2)); 
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));
    emit(stream, add(r4, r4, r5)); 

    patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0));
    emit(stream, loadP(r6, r0));
    emit(stream, loadV(r7, 'L')); //loading L
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2)); 
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));
    emit(stream, add(r4, r4, r5)); 

    patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0));
    emit(stream, loadP(r6, r0));
    emit(stream, output(r7)); //outputing L
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2)); 
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));
    emit(stream, add(r4, r4, r5)); 

    patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0));
    emit(stream, loadP(r6, r0));
    emit(stream, loadV(r7, 'O')); //loading O
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2)); 
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));
    emit(stream, add(r4, r4, r5));  

    patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0));
    emit(stream, loadP(r6, r0));
    emit(stream, output(r7)); //outputing O
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2)); 
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));
    emit(stream, add(r4, r4, r5)); 

    patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0));
    emit(stream, loadP(r6, r0));
    emit(stream, loadV(r7, '\n')); //loading \n
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2)); 
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));
    emit(stream, add(r4, r4, r5));  

    patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0));
    emit(stream, loadP(r6, r0));
    emit(stream, output(r7)); //outputing \n
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2)); 
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));
    emit(stream, add(r4, r4, r5)); 

    patch_L = Seq_length(stream);
    emit(stream, loadV(r6, 0));
    emit(stream, loadP(r6, r0));
    emit(stream, halt());  //halt
    add_label(stream, patch_L, Seq_length(stream));
    emit(stream, loadV(r3, patch_L + 2));
    emit(stream, segL(r3, r0, r3));
    emit(stream, segS(r2, r4, r3));

    emit(stream, loadP(r0, r2));
    emit_out_string(stream, "LoadP failed.\n", r4);
    emit(stream, halt());
}  
