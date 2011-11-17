#include <stdio.h>
#include <stdlib.h>
#include <UM.h>

/* recieves input and passes input to UM */
int main(int argc, char* argv[]){
    assert(argc<=2);
    FILE *input;
    input = fopen(argv[1], "r");
    if (!input) {
        perror(argv[1]);
        exit(1);
    }
    run(input);
}
