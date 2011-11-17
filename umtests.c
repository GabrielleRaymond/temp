#include <emitTest.h>

static struct test_info {
    const char *name;
    const char *test_input;          // NULL means no input needed
    const char *expected_output;
    void (*emit_test)(Seq_T stream); // writes instructions into sequence
} tests[] = {
    { "halt", NULL, "",               emit_halt_test },
    { "goto", NULL, "GOTO passed.\n", emit_goto_test },
    { "condM", NULL, "CondMove passed.\n", emit_conditionalMove_test },
    { "output", NULL, " \n.\n^\n~\nOutput passed.\n", emit_output_test },
    { "input", "c", "Input passed.\n", emit_input_test }, 
    { "nand", NULL, "NAND passed.\n", emit_nand_test }, 
    { "add" , "3", "1 + 2 = _\nAdd passed.\n", emit_add_test },
    { "mult" , "2" ,"1 x 2 = _\nMult passed.\n", emit_mult_test },
    { "div" , "2" , "2 / 1 = _\nDiv passed.\n", emit_div_test },
    { "mapUnmap", NULL, NULL, emit_MapUnmap_test },
    { "segLoadUnload", NULL, "Seg load and store passed. \n", \
        emit_segLoadUnload_test },
    { "loadP" , NULL , "HELLO\n" , emit_loadP_test },
};

#define NTESTS (sizeof(tests)/sizeof(tests[0]))

static FILE *open_and_free_pathname(char *path);
// open file 'path' for writing, then free the pathname;
// if anything fails, checked runtime error

static void write_or_remove_file(char *path, const char *contents);
// if contents is NULL or empty, remove the given 'path', 
// otherwise write 'contents' into 'path'.  Either way, free 'path'.

static void Um_write_sequence(FILE *output, Seq_T stream);
//writes the um sequence to the file pointed to by the file pointer

static void write_test_files(struct test_info *test);
//writes the individual files associated with each test



int main (int argc, char *argv[]) {
    bool failed = false;
    if (argc == 1)
        for (unsigned i = 0; i < NTESTS; i++) {
            printf("***** Writing test '%s'.\n", tests[i].name);
            write_test_files(&tests[i]);
        }
    else
        for (int j = 1; j < argc; j++) {
            bool tested = false;
            for (unsigned i = 0; i < NTESTS; i++)
                if (!strcmp(tests[i].name, argv[j])) {
                    tested = true;
                    write_test_files(&tests[i]);
                }
            if (!tested) {
                failed = true;
                fprintf(stderr, "***** No test named %s *****\n", argv[j]);
            }
        }
    return failed; // failed nonzero == exit nonzero == failure
}


static void write_or_remove_file(char *path, const char *contents) {
    if (contents == NULL || *contents == '\0') {
        remove(path);
    } else {
        FILE *input = fopen(path, "wb");
        assert(input);
        fputs(contents, input);
        fclose(input);
    }
    free(path);
}

static FILE *open_and_free_pathname(char *path) {
    FILE *fp = fopen(path, "wb");
    assert(fp);
    free(path);
    return fp;
}

static void Um_write_sequence(FILE *output, Seq_T stream) {
    Um_instruction inst;
    while(Seq_length(stream) != 0) {
        inst = (Um_instruction)(uintptr_t)(Seq_remlo(stream));
        putc(Bitpack_getu(inst, 8, 24), output);
        putc(Bitpack_getu(inst, 8, 16), output);
        putc(Bitpack_getu(inst, 8, 8), output);
        putc(Bitpack_getu(inst, 8, 0), output);
    } 
}

static void write_test_files(struct test_info *test) {
    FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
    Seq_T stream = Seq_new(0);
    test->emit_test(stream);
    Um_write_sequence(binary, stream);
    Seq_free(&stream);
    fclose(binary);

    write_or_remove_file(Fmt_string("%s.0", test->name), test->test_input);
    write_or_remove_file(Fmt_string("%s.1", test->name), test->expected_output);
}
