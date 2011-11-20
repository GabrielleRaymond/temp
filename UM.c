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
    Seg_T seg0;
    regNum regs[8];
    instructPtr progCount;
};

/* $rA:= $rB if $rC != 0 */ 
static inline void condMove(UM_T UM, regNum rA, regNum rB, regNum rC){
    if(UM->regs[rC]!= (regNum)0)
        UM->regs[rA] = UM->regs[rB];
}

/* $rA gets the word at segment $rB at offset $rC */
static inline void segLoad(UM_T UM, regNum rA, regNum rB, regNum rC){
    UM->regs[rA]=UMSegs_getWord(UM->mem, UM->regs[rB], UM->regs[rC]); 
}

/* Stores the value in $rC in segment $rA at offset $rB */
static inline void segStore(UM_T UM, regNum rA, regNum rB, regNum rC){
    UMSegs_putWord(UM->mem, UM->regs[rA], UM->regs[rB], UM->regs[rC]);
}

/* Stores the sum of $rB and %rC in $rA*/
static inline void addition(UM_T UM, regNum rA, regNum rB, regNum rC){
    UM->regs[rA] = (regVal)(UM->regs[rB] + UM->regs[rC]);
}

/* Stores the product of $rB and $rC in $rA */
static inline void multiplication(UM_T UM, regNum rA, regNum rB, regNum rC){
    UM->regs[rA] = (regVal)(UM->regs[rB] * UM->regs[rC]);
}

/* Stores the quotient of %rB and %rC in $rA */
static inline void divide(UM_T UM, regNum rA, regNum rB, regNum rC){
    UM->regs[rA] = (UMinstruction)(UM->regs[rB] / UM->regs[rC]);
}

/* Stores $rB NAND $rC in $rA */
static inline void NAND(UM_T UM, regNum rA, regNum rB, regNum rC){
    UM->regs[rA] = ~(UM->regs[rB] & UM->regs[rC]);
}

/* Frees the segments held by the UM */
static inline void halt(UM_T UM){
    UMSegs_free(&(UM->mem));
}

/* Maps the segment at offset $rC and stores the segID in $rB */
static inline void mapSeg(UM_T UM, regNum rB, regNum rC){
    UM->regs[rB] = UMSegs_mapSeg(UM->mem, UM->regs[rC]);
}

/* Unmaps segment %rC */
static inline void unmapSeg(UM_T UM, regNum rC){
    UMSegs_unmapSeg(UM->mem, UM->regs[rC]);
}

/* Prints the character stored in $rC */
static inline void output(UM_T UM, regNum rC){
    printf("%c", UM->regs[rC]);
}

/* $rC stores a character from input */
static inline void input(UM_T UM, regNum rC){
    char c = getc(stdin);
    if(c==EOF) 
        UM->regs[rC] = ~((regVal)0);
    else
        UM->regs[rC] = (regVal)c; 
}

/* The program counter is set to $rC. If $rB != 0, segment 0 is replaced with 
 * segment $rB. This acts as a goto when not loading a new program ($rB!=0). 
 * INVARIANT: The SegID of the most recent unmapped segment is the SegID
 * returned for the next mapSeg call. 
 */
static inline void loadPro(UM_T UM, regNum rB, regNum rC){
    segmentID segID;
    UMinstruction newWord;
    int length;
    
    if(UM->regs[rB] != (regNum)0){
        Seg_T newProg = UMSegs_getSeg(UM->mem, UM->regs[rB]);
        UMSegs_unmapSeg(UM->mem, (segmentID)0);
        length = Seg_length(newProg);
        segID = UMSegs_mapSeg(UM->mem, length);
        
        for(int i = 0; i < length; i++){
            newWord = Seg_get(newProg, i);
            UMSegs_putWord(UM->mem, 0, i, newWord);
        }
        Seg_free(&newProg); 
        Seg_free(&(UM->seg0)); 
        UM->seg0 = UMSegs_getSeg(UM->mem, 0);
    }
    UM->progCount = UM->regs[rC];
}

/* Loads val parsed from the word and stores it in the determined reg */
static inline void loadVal(UM_T UM, regNum rA, regVal val){
    UM->regs[rA] = val; 
}

/* Creates a fully initialized UM. Assumes that *fp will not to be NULL. 
 * INVARIANT: The first segment mapped from an intialized UMSegs_T will have
 * a segID of 0 
*/
static T load(FILE *fp) 
{
    Seq_T instructions = Seq_new(1024);
    UMinstruction *word32;
    int32_t toAdd; 
    // Stores instruction words in a sequence 
    do {
        toAdd = getc(fp);
        if(toAdd != EOF){
            NEW(word32);
            *word32 = ((toAdd << 24) | ((uint32_t)getc(fp) << 16) |
                    (uint32_t)getc(fp) << 8) | (uint32_t)getc(fp);
            Seq_addhi(instructions, word32);
        }

    } while (toAdd != EOF);

    segmentID segID;
    UMinstruction *instruct;
    T UM;
    NEW(UM);
    UM->mem = UMSegs_new(1024);
    segID = UMSegs_mapSeg(UM->mem, Seq_length(instructions));
   
    int counter = 0;
    int wordsLeft2Add = Seq_length(instructions);
    wordCount = wordsLeft2Add;
    // When the sequence is empty, segment 0 contains all of the instructions
    for(UMinstruction i = wordsLeft2Add; i > 0; i--) {
        instruct = Seq_remlo(instructions);
        UMSegs_putWord(UM->mem, segID, counter, *instruct);
        FREE(instruct);
        counter++;
    }

    // Initialize pointer to segment 0 CHECK
    UM->seg0 = UMSegs_getSeg(UM->mem, 0);
    
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

/* Carries out a single instruction  */
static inline int instructionLoop(UM_T UM, enum OP opcode, regNum rA, regNum rB,
        regNum rC, regVal val) { 
        switch(opcode) {
            case cMov:
                condMove(UM, rA, rB, rC);
                break;
            case sLoad:
                segLoad(UM, rA, rB, rC);
                break;
            case sStore:
                segStore(UM, rA, rB, rC);
                break;
            case add:
                addition(UM, rA, rB, rC);
                break;
            case mult:
                multiplication(UM, rA, rB, rC);
                break;
            case divi:
                divide(UM, rA, rB, rC);
                break;
            case nand:
                NAND(UM, rA, rB, rC);
                break;
            case end:
                halt(UM);
                break;
            case mapS:
                mapSeg(UM, rB, rC);
                break;
            case unmapS:
                unmapSeg(UM, rC);
                break;
            case out:
                output(UM, rC);
                break;
            case in:
                input(UM, rC);
                break;
            case loadP:
                loadPro(UM, rB, rC);
                break;
            case loadV:
                loadVal(UM, rA, val);
                break;
            default:
                exit(1);
                break;
        }
        if(opcode != loadP)
            UM->progCount++;
        if(opcode == end)
            return end;
        return 1;
} 

/* Carries out instructions and iterates the progCount until Halt is called */
void run(FILE *fp){
    UM_T UM = load(fp);
    UMinstruction instruction;
    regVal val = 0;
    regNum rA, rB, rC = 0;
    enum OP opcode;
    do {  
        instruction = Seg_get(UM->seg0, UM->progCount);
        opcode = (enum OP) ((instruction >> 28) & (uint32_t)15);
        if(opcode == loadV) {
            rA = (instruction >> 25) & (uint32_t)7;
            val = instruction & (uint32_t)33554431;

        }
        else {
        rA = (instruction >> 6) & (uint32_t)7;
        rB = (instruction >> 3) & (uint32_t)7;
        rC = instruction & (uint32_t)7;
        }

    } while((enum OP)instructionLoop(UM, opcode, rA, rB, rC, val) != end);
    FREE(UM);
}

#undef T
#undef regVal 
#undef regNum 
#undef instruction 
#undef segmentID 
#undef instructPtr 
