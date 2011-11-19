#include "UM.h"

#define T UM_T
#define regVal uint32_t
#define regNum uint32_t
#define UMinstruction uint32_t
#define segmentID uint32_t
#define instructPtr uint32_t
typedef struct ThreeRegs ThreeRegs;
typedef struct RegAndVal RegAndVal;
static int wordCount;

enum OP { cMov = 0, sLoad, sStore, add, mult, divi, nand, end, mapS,
    unmapS, out, in, loadP, loadV };

struct T {
    UMSegs_T mem;
    regNum regs[8];
    instructPtr progCount;
};

struct ThreeRegs{
    regNum rA;
    regNum rB;
    regNum rC;
};

struct RegAndVal{
    regNum rA;
    regVal val;
};

static ThreeRegs unpackThreeRegs(UMinstruction word)
{
    ThreeRegs inUse;
    inUse.rA = (word >> 6) & (uint32_t)7;
    inUse.rB = (word >> 3) & (uint32_t)7;
    inUse.rC = word & (uint32_t)7;
    /*
    inUse.rA = Bitpack_getu(word, 3, 6);
    inUse.rB = Bitpack_getu(word, 3, 3);
    inUse.rC = Bitpack_getu(word, 3, 0);
    */
    return inUse;
}

static struct RegAndVal unpackValue(UMinstruction word)
{
    RegAndVal toStore;
    toStore.rA = (word >> 25) & (uint32_t)7;
    toStore.val = word & (uint32_t)33554431;
    /*
    toStore.rA = Bitpack_getu(word, 3, 25);
    toStore.val = Bitpack_getu(word, 25, 0);
    */
    return toStore;
}  

/* $rA:= $rB if $rC != 0 */ 
static void condMove(UM_T UM, UMinstruction word){
    ThreeRegs R = unpackThreeRegs(word);
    if(UM->regs[R.rC]!= (regNum)0)
        UM->regs[R.rA] = UM->regs[R.rB];
}

/* $rA gets the word at segment $rB at offset $rC */
static void segLoad(UM_T UM, UMinstruction word){
    ThreeRegs R = unpackThreeRegs(word);
    UM->regs[R.rA]=UMSegs_getWord(UM->mem, UM->regs[R.rB], UM->regs[R.rC]); 
}

/* Stores the value in $rC in segment $rA at offset $rB */
static void segStore(UM_T UM, UMinstruction word){
    ThreeRegs R = unpackThreeRegs(word);
    UMSegs_putWord(UM->mem, UM->regs[R.rA], UM->regs[R.rB], UM->regs[R.rC]);
}

/* Stores the sum of $rB and %rC in $rA*/
static void addition(UM_T UM, UMinstruction word){
    ThreeRegs R = unpackThreeRegs(word);
    UM->regs[R.rA] = (regVal)(UM->regs[R.rB] + UM->regs[R.rC]);
}

/* Stores the product of $rB and $rC in $rA */
static void multiplication(UM_T UM, UMinstruction word){
    ThreeRegs R = unpackThreeRegs(word);
    UM->regs[R.rA] = (regVal)(UM->regs[R.rB] * UM->regs[R.rC]);
}

/* Stores the quotient of %rB and %rC in $rA */
static void divide(UM_T UM, UMinstruction word){
    ThreeRegs R = unpackThreeRegs(word);
    UM->regs[R.rA] = (UMinstruction)(UM->regs[R.rB] / UM->regs[R.rC]);
}

/* Stores $rB NAND $rC in $rA */
static void NAND(UM_T UM, UMinstruction word){
    ThreeRegs R = unpackThreeRegs(word);
    UM->regs[R.rA] = ~(UM->regs[R.rB] & UM->regs[R.rC]);
}

/* Frees the segments held by the UM */
static void halt(UM_T UM, UMinstruction word){
    (void)word;
    UMSegs_free(&(UM->mem));
}

/* Maps the segment at offset $rC and stores the segID in $rB */
static void mapSeg(UM_T UM, UMinstruction word){
    ThreeRegs R = unpackThreeRegs(word);
    UM->regs[R.rB] = UMSegs_mapSeg(UM->mem, UM->regs[R.rC]);
}

/* Unmaps segment %rC */
static void unmapSeg(UM_T UM, UMinstruction word){
    ThreeRegs R = unpackThreeRegs(word);
    UMSegs_unmapSeg(UM->mem, UM->regs[R.rC]);
}

/* Prints the character stored in $rC */
static void output(UM_T UM, UMinstruction word){
    ThreeRegs R = unpackThreeRegs(word);
    assert(R.rC < (regNum)8 &&  UM->regs[R.rC] <= 255);
    printf("%c", UM->regs[R.rC]);
}

/* $rC stores a character from input */
static void input(UM_T UM, UMinstruction word){
    ThreeRegs R = unpackThreeRegs(word);
    char c = getc(stdin);
    if(c==EOF) 
        UM->regs[R.rC] = ~((regVal)0);
    else
        UM->regs[R.rC] = (regVal)c; 
}

/* The program counter is set to $rC. If $rB != 0, segment 0 is replaced with 
 * segment $rB. This acts as a goto when not loading a new program ($rB!=0). 
 * INVARIANT: The SegID of the most recent unmapped segment is the SegID
 * returned for the next mapSeg call. 
 */
static void loadPro(UM_T UM, UMinstruction word){
    ThreeRegs R = unpackThreeRegs(word);
    segmentID segID;
    UMinstruction newWord;
    int length;
    
    if(UM->regs[R.rB] != (regNum)0){
        Seg_T newProg = UMSegs_getSeg(UM->mem, UM->regs[R.rB]);
        UMSegs_unmapSeg(UM->mem, (segmentID)0);
        length = Seg_length(newProg);
        segID = UMSegs_mapSeg(UM->mem, length);
        assert(segID == (segmentID)0);

        for(int i = 0; i < length; i++){
            newWord = Seg_get(newProg, i);
            UMSegs_putWord(UM->mem, 0, i, newWord);
        }
        Seg_free(&newProg);
    }
    UM->progCount = UM->regs[R.rC];
}

/* Loads val parsed from the word and stores it in the determined reg */
static void loadVal(UM_T UM, UMinstruction word){
    RegAndVal V = unpackValue(word); 
    assert(V.rA < (regNum)8);
    UM->regs[V.rA] = V.val; 
}

/* Creates a fully initialized UM. Assumes that *fp will not to be NULL. 
 * INVARIANT: The first segment mapped from an intialized UMSegs_T will have
 * a segID of 0 
*/
static T load(FILE *fp) 
{
    Seq_T instructions = Seq_new(1024);
    uint64_t word64;
    word64 = 0;
    UMinstruction *word32;
    int64_t toAdd;
    int counter = 0;
    // Stores instruction words in a sequence 
    do {
        toAdd = getc(fp);
        if(toAdd != EOF)
            word64 = Bitpack_newu(word64, 8, ((3 -(counter%4))*8), toAdd);
        counter++;
        if(counter%4 == 0) { 
            NEW(word32);
            *word32 = (UMinstruction)word64; 
            Seq_addhi(instructions, word32);
        }
    } while (toAdd != EOF);

    segmentID segID;
    UMinstruction *instruct;
    T UM;
    NEW(UM);
    UM->mem = UMSegs_new(1024);
    segID = UMSegs_mapSeg(UM->mem, Seq_length(instructions));
    assert(segID == (segmentID)0);
   
    counter = 0;
    int wordsLeft2Add = Seq_length(instructions);
    wordCount = wordsLeft2Add;
    // When the sequence is empty, segment 0 contains all of the instructions
    for(UMinstruction i = wordsLeft2Add; i > 0; i--) {
        instruct = Seq_remlo(instructions);
        UMSegs_putWord(UM->mem, segID, counter, *instruct);
        FREE(instruct);
        counter++;
    }
    // Initialize registers 
    for(regNum i = 0; i < 8; i++){
        UM->regs[i] = (regVal)0;
    }
    // Initialize the program counter and clean up memory
    UM->progCount = (instructPtr)0;
    fclose(fp);
    Seq_free(&instructions);
    return UM;
}

/* Carries out instructions and iterates the progCount until Halt is called */
void run(FILE *fp){
    UM_T UM = load(fp);
    enum OP opcode;
    UMinstruction instruction;
    do{

        instruction = UMSegs_getWord(UM->mem, 0, UM->progCount);
        opcode = (enum OP) ((instruction >> 28) & (uint32_t)15);
        //opcode = (enum OP)Bitpack_getu(instruction, 4, 28);
        switch(opcode) {
            case cMov:
                condMove(UM, instruction);
                break;
            case sLoad:
                segLoad(UM, instruction);
                break;
            case sStore:
                segStore(UM, instruction);
                break;
            case add:
                addition(UM, instruction);
                break;
            case mult:
                multiplication(UM, instruction);
                break;
            case divi:
                divide(UM, instruction);
                break;
            case nand:
                NAND(UM, instruction);
                break;
            case end:
                halt(UM, instruction);
                break;
            case mapS:
                mapSeg(UM, instruction);
                break;
            case unmapS:
                unmapSeg(UM, instruction);
                break;
            case out:
                output(UM, instruction);
                break;
            case in:
                input(UM, instruction);
                break;
            case loadP:
                loadPro(UM, instruction);
                break;
            case loadV:
                loadVal(UM, instruction);
                break;
            default:
                exit(1);
                break;
        }
        if(opcode != loadP)
              UM->progCount++;
    }   while(opcode != end); 
    FREE(UM);
}

#undef T
#undef regVal 
#undef regNum 
#undef instruction 
#undef segmentID 
#undef instructPtr 
