
/* asm2bf
 *
 * Copyright (C) K. Palaiologos Szewczyk, 2017-2020.
 * License: MIT
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config.h"

struct delta_match {
    int32_t coefficient;
    uint8_t type;
};

struct replacement {
    char * src;
    char * dest;
};

struct delta_match queue[16];
unsigned int qp = 0, pos = 0;

struct delta_match suck_delta(void) {
    int c;
    struct delta_match m;
    
    m.coefficient = 0;
    
    if(qp) {
        return queue[--qp];
    }
    
    while(isdigit(c = getchar())) {
        pos++;
        m.coefficient *= 10;
        m.coefficient += c - '0';
    }
    
    if(!m.coefficient)
        m.coefficient = 1;
    
    if(c == '<' || c == '-')
        m.coefficient = -m.coefficient;
    
    if(c != '>' && c != '+' && c != '<' && c != '-') {
        ungetc(c, stdin);
        
        // probs no value.
        m.type = 0; m.coefficient = 0;
        
        return m;
    } else
        pos++;
    
    m.type = c == '<' || c == '>';
    
    fprintf(stderr, "\033[32mDebug: sucked delta: %d, %d\033[37m\n", m.coefficient, m.type);
    
    return m;
}

void match_ignore(void) { suck_delta(); }
void match_const(void) { printf("%d", suck_delta().coefficient); }
void match_value(void) { int32_t x = suck_delta().coefficient; if(x) printf("tape[mp] += %d;\n", x); }
void match_mp(void) { int32_t x = suck_delta().coefficient; if(x) printf("mp += %d;\n", x); }
void match_flexy(void) {
    struct delta_match m = suck_delta();
    
    if(m.type == 2) {
        fprintf(stderr, "\033[31m*** Severe: Incorrect flexy match type.\033[37m\n");
        abort();
    }
    
    if(!m.coefficient) return;
    
    printf(!m.type ? "tape[mp] += %d;\n" : "mp += %d;\n", m.coefficient);
}

void emit(const char * template) {
    uint8_t match_swap = 0;

    while(*template) {
        if(match_swap) {
            match_swap = 0;
            
            switch(*template) {
                case 'V': match_value(); break;
                case 'M': match_mp(); break;
                case 'C': match_const(); break;
                case 'I': match_ignore(); break;
                case 'T': printf("tape[mp]"); break;
                case 'S': pos++; getchar(); break;
                default: putchar('$'); putchar(*template);
            }
            
            template++;

            continue;
        }

        if(*template == '$')
            match_swap = 1;
        else
            putchar(*template);
        
        template++;
    }
}

struct replacement vm_model[] = {
    #include "microcode/bfvm.c"
    ,
    {NULL, NULL}
};

int try_match(char buf[]) {
    struct replacement current_row;
    int i = 0;
    
    while((current_row = vm_model[i]).src) {
        if(!strcmp(buf, current_row.src)) {
            emit(current_row.dest);
            return 1;
        }
        
        i++;
    }
    
    return 0;
}

int main(int argc, char * argv[]) {
    char match[32] = {0};
    int c, mp = 0, b32 = 0, freestanding = 0, heap = 65536;

    for(int arg = 1; arg < argc; arg++) {
        char * s = argv[arg];

        if(!strcmp("-32", s)) {
            b32 = 1;
        } else if(!strcmp("-freestanding", s)) {
            freestanding = 1;
        } else if(!strcmp("-heap", s)) {
            if(arg + 1 < argc) {
                heap = atoi(argv[++arg]);
            } else {
                fprintf(stderr, "bfvm: -heap expects an argument.\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "bfvm: unrecognized switch: `%s'\n", s);
            exit(1);
        }
    }
    
    if(!freestanding) {
        printf(
            "#include <stdio.h>\n"
            "#include <stdlib.h>\n"
            "#include <stdint.h>\n"
            "uint8_t inchar(void) {\n"
                "uint8_t v = getchar();\n"
                "return v < 0 ? 0 : v;\n"
            "}\n"
            "type*tape=calloc(sizeof(type),%d),mp,t0,t1,t2,t3,sp;\n"
        , heap);
    } else {
        printf(
            "type*tape=0x7000,mp,t0,t1,t2,t3,sp;\n"
        );
    }

    printf(
        "#define OFF(x) ((x) - 'a')\n"
        "#define G (tape[0])\n"
        "#define IP (tape[1])\n"
        "#define type %s\n"
        "type bfpow(type x, type y) {\n"
            "type i = 1, s = x; for(; i < y; i++) s *= x; return s;\n"
        "}\n"
        "type par(type x) {\n"
            "type parity = 0;\n"
            "while(x) {\n"
                "parity ^= x;\n"
                "x >>= 1;\n"
            "}\n"
            "return parity & 1;\n"
        "}\n"
        "int main(void) {\n"
    , b32 ? "uint32_t" : "uint16_t");
    
    while((c = getchar()) != EOF) {
        pos++;
        switch(c) {
            case '0' ... '9':
                fprintf(stderr, "\033[36mDebug: found a backfill delta.\033[37m\n");
                ungetc(c, stdin);
                match_flexy();
                break;
            case '+': fprintf(stderr, "\033[31mDebug: Stray BF @%d\033[37m\n", pos); printf("tape[mp]++;"); break;
            case '>': fprintf(stderr, "\033[31mDebug: Stray BF @%d\033[37m\n", pos); printf("mp++;"); break;
            case '-': fprintf(stderr, "\033[31mDebug: Stray BF @%d\033[37m\n", pos); printf("tape[mp]--;"); break;
            case '<': fprintf(stderr, "\033[31mDebug: Stray BF @%d\033[37m\n", pos); printf("mp++;"); break;
            case 'Z': fprintf(stderr, "\033[31mDebug: !! Z not matched @%d\033[37m\n", pos); case '\n': case '\r': case ' ': break;
            default:
                match[mp++] = c;

                if(mp > 32) {
                    fprintf(stderr, "\033[31m*** Severe: BFVM matcher jammed.\033[37m\n");
                    exit(1);
                }
                
                if(!try_match(match)) {
                    fprintf(stderr, "\033[33mDebug: tried matching '%s'.\033[37m\n", match);
                } else {
                    fprintf(stderr, "\033[33mDebug: matched '%s'.\033[37m\n", match);
                    memset(match, 0, 32);
                    mp = 0;
                }
        }
    }
    
    puts("}");
}
