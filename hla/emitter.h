
#ifndef _EMITTER_H
#define _EMITTER_h

#include <stdio.h>

enum {
    Txt, Db, Seg, Off, Stk
};

struct code_chunk {
    char * instruction;
    char * param1, * param2;

    struct code_chunk * next;
};

struct data_chunk {
    unsigned char linkage;
    int type;
    char * identifier;

    union {
        char * char_sequence;
        int constant;
    } value;

    struct data_chunk * next;
};

void emit(char * instruction, char * param1, char * param2);
void emit_txt(unsigned char linkage, char * identifier, char * string);
void emit_data(unsigned char linkage, char * identifier, int type, int n);
void flush_code(FILE * f);

#endif
