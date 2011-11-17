//list of unit tests
#include <emitInfra.h>

#ifndef _EMITTEST_INCLUDE
#define _EMITTEST_INCLUDE

extern void emit_halt_test(Seq_T stream);
extern void emit_goto_test(Seq_T stream); 
extern void emit_output_test(Seq_T stream); 
extern void emit_conditionalMove_test(Seq_T stream);
extern void emit_input_test(Seq_T stream);
extern void emit_nand_test(Seq_T stream);
extern void emit_add_test(Seq_T stream); 
extern void emit_mult_test(Seq_T stream);
extern void emit_MapUnmap_test(Seq_T stream); 
extern void emit_div_test(Seq_T stream);
extern void emit_segLoadUnload_test(Seq_T stream);
extern void emit_loadP_test(Seq_T stream);

#endif
