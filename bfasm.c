
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

/*
 * --------------------------------------------------------------------------------------------------------------------------
 * A VERY VERY IMPORTANT NOTE! THIS FILE MAY SEEM OBFUSCATED OR OVERALL UNREADABLE. HERE'S WHY.
 * Back in the day when I started the asm2bf project (early 2017), I thought that I'd write this reference implementation,
 * and then write an asm2bf version of it, so that my self compiler actually runs the project.
 * Later on, I thought that I'll rather write asm2bf in C, so I froze the asm2bf self compiler in v0.9 and started to tweak
 * this program, that was meant to be the reference implementation.
 * There's no need for running away in terror, as the most of the code is just parsing the input and outputting the premade
 * instruction microcode in the `s' string, containing instruction names and the microcode.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

#define IC 137

/* db command location: instruction count - 4 */
#define C1 (IC-4)

/* total mnemonic length: instruction count * 3 */
#define C2 (IC*3)

/* txt command location: instruction count - 3 */
#define C3 (IC-3)

/* location of various instructions */
#define BTS (IC-8)
#define RSE (IC-7)
#define STK (IC-6)
#define ORG (IC-5)
#define DB (IC-4)
#define RAW (IC-2)
#define SEG (IC-1)

#define STACK 23

/* no idea what this is */
#define C4 ((IC+7)*3)

unsigned int inchar();
void outbf();
void outrep();

static unsigned long bits = 16;
static unsigned int skipped_inits = 0;
static unsigned char disable_opt = 0, shutup = 0, rle_prefix = 0, rle_postfix = 0, vm = 0, tiny = 0;
static unsigned long m[12000], off, freecell, rseg;
static char s[] =
    #include "microcode/bfasm-instructions.c"
    ;

static char ubf[] = 
    #include "microcode/emit-bf.c"
    ;

static char uvm[] =
    #include "microcode/emit-bfvm.c"
    ;

int best_base(int n);
void translate(int n, int base);

int main(int argc, char * argv[]) {
    unsigned int n;
    
    for(int arg = 1; arg < argc; arg++) {
        if(!strcmp("-O0", argv[arg])) {
            disable_opt = 1;
        } else if(!strcmp("-w", argv[arg])) {
            shutup = 1;
        } else if(!strcmp("-zb", argv[arg])) {
            rle_prefix = 1;
        } else if(!strcmp("-ze", argv[arg])) {
            rle_postfix = 1;
        } else if(!strcmp("-vm", argv[arg])) {
            rle_prefix = 1;
            vm = 1;
        } else if(!strcmp("-t", argv[arg])) {
            tiny = 1;
        }
    }

    if(rle_postfix && rle_prefix) {
        fprintf(stderr, " *** ERROR: Settle on your RLE style!\n");
        exit(1);
    }

    if(vm && rle_postfix) {
        fprintf(stderr, " *** ERROR: Can't produce postfix-compressed BFVM bytecode.");
        exit(1);
    }

    for (n = 0; n < sizeof(s); n++)  m[n + 20] = s[n];
    if(vm) for (n = 0; n < sizeof(uvm); n++) m[n + 20 + sizeof(s)] = uvm[n];
    else   for (n = 0; n < sizeof(ubf); n++) m[n + 20 + sizeof(s)] = ubf[n];
    m[6] = 0;
    m[8] = tiny * 2;
    m[9] = STACK;
    m[10] = m[9] + 2;
    outbf();
    m[11] = 1;
    m[12] = 0;
    m[1] = 1;
Laa:;
    m[0] = inchar();
    if (m[1] == 1) m[0] = (m[0] >= 'A' && m[0] <= 'Z') ? (m[0]) - 'A' + 'a' : m[0];
Laj:;
    if (m[0] != '\0') goto Lam;
    if (m[1] == 2) goto Lai;
    goto Lab;
Lam:;
    if (m[0] != '\n' && m[0] != '\v' && m[0] != '\f' && m[0] != '\r') goto Lad;
    if (m[1] == 2) goto Lai;
    m[1] = 1;
    goto Laa;
Lad:;
    if (m[1] == 4) {
        if (m[0] == '\"') {
            m[1] = 1;
            goto Laa;
        }
        m[2] = C1;
        m[3] = m[0];
        goto Lai;
    }
    if (m[0] == ' ' || m[0] == '\t' || m[1] == 0) goto Laa;
    if (m[1] != 3) goto Lay;
    
    if (m[0] != '\"') {
        if(!shutup) fprintf(stderr, "\n** ERROR: Closing quote expected.\n");
        goto Laz;
    }
    
    m[1] = 4;
    goto Laa;
Lay:;
    if (m[0] != ';') goto Lac; /* comment */
    if (m[1] == 2) goto Lai;
    m[1] = 0;
    goto Laa;
Lac:;
    if (m[1] != 1) goto Lae; /* get 3 letter instruction */
    m[2] = inchar();
    m[3] = inchar();
    m[2] = (m[2] >= 'A' && m[2] <= 'Z') ? (m[2]) - 'A' + 'a' : m[2];
    m[3] = (m[3] >= 'A' && m[3] <= 'Z') ? (m[3]) - 'A' + 'a' : m[3];
    if(m[3] == ' ') m[3] = '_';
    m[4] = 0;
Laf:;
    m[5] = m[4] + 20;
    m[6] = m[m[5]];
    if (m[0] != m[6]) goto Lag;
    m[5]++;
    m[6] = m[m[5]];
    if (m[2] != m[6]) goto Lag;
    m[5]++;
    m[6] = m[m[5]];
    if (m[3] == m[6]) goto Lah;
Lag:;
    m[4]++;
    m[4]++;
    m[4]++;
    if (m[4] == C2) { if(!shutup) fprintf(stderr, "\n** ERROR: No such instruction: %c%c%c\n", m[0], m[2], m[3]); goto Laz; } /* not found, quit */
    goto Laf;
Lah:;
    m[1] = 2;
    m[2] = m[4] / 3;
    m[3] = 0;
    m[4] = 0;
    m[5] = 0;
    goto Laa;
Lae:; /* find operands */
    if (m[2] != C3) goto Lax; /* txt command */
    m[1] = 3;
    goto Laj;
Lax:;
    if(m[0] == 'f' || m[0] == 'F') {
        m[0] = inchar();
        m[0] = m[0] - '1';
        if (m[0] > 2) { if(!shutup) fprintf(stderr, "\n** ERROR: Register unavailable: f%d\n", m[0] + 1); goto Laz; }
        m[4] = m[0] == 0 ? 'q' : (m[0] == 1 ? 'v' : 't');
        goto Laa;
    }
    
    if (m[0] != 'r' && m[0] != 'R') goto Lak; /* r_ operand */
    m[0] = inchar();
    m[0] = m[0] - '1';
    if (m[0] > 5) { if(!shutup) fprintf(stderr, "\n** ERROR: Register unavailable: r%d\n", m[0] + 1); goto Laz; }
    m[4] = m[0] >= 4 ? (m[0] - 4 + 'r') : (m[0] + 'f');
    goto Laa;
Lak:;
    if (m[0] != ',') goto Lan; /* , */
    m[5] = m[4];
    m[4] = 0;
    goto Laa;
Lan:;
    if (m[0] != '.') goto Lal; /* .char */
    m[3] = inchar();
    goto Laa;
Lal:; /* number */
    m[0] = m[0] - '0';
    if (m[0] > 9) { if(!shutup) fprintf(stderr, "\n** ERROR: Expected digit, got `%c'\n", m[0] + '0'); goto Laz; }
    m[3] = m[3] * 10;
    m[3] = m[3] + m[0];
    goto Laa;
Lai:;
    if (m[4] && m[4] == m[5]) {
        m[6] = 22;
        m[5] = 'j';
        outbf();
        m[5] = m[4];
        m[4] = 'j';
    }
    if (m[11] == 1 && m[2] != 12) {
        m[6] = 2;
        outbf(); /* pre */
        m[11] = 0;
    }
    switch (m[2]) {
        case 0: /* add */
            if (m[4] == 0) {
                m[6] = 4;
                m[4] = m[5];
                outbf();
                m[6] = '+';
                outrep();
                goto Lap;
            }
            goto Lao;
        case 9: /* jmp */
            if(tiny) {
                if(!shutup)
                    fprintf(stderr, "\n** ERROR: jmp not allowed in tiny mode.\n");
                exit(1);
            }

            m[11] = 1;
            m[12] = 1;
            goto Lao;
        case 10: case 46: /* jnz */
            if(tiny) {
                if(!shutup)
                    fprintf(stderr, "\n** ERROR: jnz not allowed in tiny mode.\n");
                exit(1);
            }

            m[11] = 1;
            m[12] = 1;
            goto Lao;
        case 11: case 47: /* jz_ */
            if(tiny) {
                if(!shutup)
                    fprintf(stderr, "\n** ERROR: jz not allowed in tiny mode.\n");
                exit(1);
            }

            m[11] = 1;
            m[12] = 1;
            goto Lao;
        case 12: /* lbl */
            if(tiny) {
                if(!shutup)
                    fprintf(stderr, "\n** ERROR: lbl not allowed in tiny mode.\n");
                exit(1);
            }

            if (m[11] == 0) {
                m[6] = 3;
                outbf(); /* post */
            }
            
            if(m[3] == 0) {
                if(!shutup)
                    fprintf(stderr, "\n** WARNING: 0 isn't a valid label identifier.\n");
            }

            if(vm) {
                putchar('A');
                putchar('R');
                m[11] = 1;
                m[6] = 4;
                m[4] = 'e';
                outbf();
                m[6] = '+';
                outrep(); // note: potential optimalization, << & < free
                m[6] = 18; // ???
                outbf();
            } else {
                m[11] = 1;
                m[6] = 4;
                m[4] = 'e';
                outbf();
                m[6] = '+';
                outrep(); // note: potential optimalization, << & < free
                m[6] = 18; // ???
                outbf();
            }

            goto Lap;
        case 16: /* mov */
            if (m[4] == 0) {
                m[6] = 5;
                m[4] = m[5];
                outbf();
                // if moving to r4...
                if(m[5] == 'i')
                    freecell = 1;
                m[6] = '+';
                outrep();
                goto Lap;
            }
            goto Lao;
        case 27: /* sub */
            if (m[4] == 0) {
                m[6] = 4;
                m[4] = m[5];
                outbf();
                m[6] = '-';
                outrep();
                goto Lap;
            }
            goto Lao;
        case 30: /* ret */
            if(tiny) {
                if(!shutup)
                    fprintf(stderr, "\n** ERROR: ret not allowed in tiny mode.\n");
                exit(1);
            }

            m[11] = 1;
            m[12] = 1;
            goto Lao;
        case 31: /* end */
            if(tiny) {
                if(!shutup)
                    fprintf(stderr, "\n** ERROR: end not allowed in tiny mode.\n");
                exit(1);
            }

            m[11] = 1;
            m[12] = 1;
            goto Lao;
        case STK: /* stk */
            if(!rseg) m[9] = m[3] * 2 + STACK; // ???
            else m[9] = m[3] + STACK;
            goto Lap;
        case ORG: /* org */
            if(!rseg) m[10] = m[3] * 2 + m[9] + 2;
            else m[10] = m[3] + m[9] + 2;
            goto Lap;
        case DB: /* db_ */
            if(m[3] == '\\' && m[1] == 4) {
                m[3] = getchar();
                
                switch(m[3]) {
                    case 'n': m[3] = '\n'; break;
                    case 'f': m[3] = '\f'; break;
                    case 'r': m[3] = '\r'; break;
                    case '0': m[3] = '\0'; break;
                    case '\\': m[3] = '\\'; break;
                    default:
                        if(!shutup)
                            fprintf(stderr, "\n** ERROR: No such escape sequence: \\%c\n", m[3]);
                        goto Laz;
                }
            }
            
            m[6] = 4;
            m[4] = '^';
            outbf();
            m[6] = '+';
            freecell = 1;
            outrep();
            m[10] = m[10] + 2;
            if (m[1] == 4) goto Laa;
            goto Lap;
        case RAW: /* raw */
            putchar(m[3]);
            goto Lap;
        case SEG: /* seg */
            off=m[3];
            m[10]=m[9] + 2;
            goto Lap;
        case BTS:
            bits=m[3];
            goto Lap;
        case RSE:
            if(vm) {
                rseg = !rseg;
            } else if(!shutup) {
                fprintf(stderr, " *** WARNING: Brainfuck target; `rse' ignored.\n");
            }

            goto Lap;
    }
Lao:;
    if (m[4] == 0) {
        m[6] = 4; /* immed */
        m[4] = 'j';
        outbf();
        m[6] = '+';
        freecell = 1;
        outrep();
        m[3]++;
    }
    m[6] = m[2] + 6;
    outbf();
    if (m[3] == 0) goto Lap;
    m[6] = 5; /* immed clear */
    outbf();
Lap:;
    m[1] = 1;
    if (m[12] == 1) {
        m[6] = 3;
        outbf(); /* post */
        m[12] = 0;
    }
    goto Laj;
Laz:;
    putchar(vm ? '?' : '#');
    return 0;
Lab:;
    if (m[11] == 0 || m[12] == 1) {
        m[6] = 3;
        outbf(); /* post */
    }
    m[6] = 2;
    outbf(); /* pre */
    m[6] = 3;
    outbf(); /* post */
    m[6] = 1;
    outbf(); /* last */
    
    if(skipped_inits > 6 && tiny) {
        fprintf(stderr, "*** ERROR: Skipped %d prologues; more than 6. Can't build tiny code.\n", skipped_inits);
        return 1;
    }

    return 0;
}

void outrep() {
    if(m[3] > (2 << (bits - 1)) - 1) {
        if(!shutup)
            fprintf(stderr, "*** WARNING: Exceeding bitwidth: %lu can't be stored with %lu bits, max is %lu.\n", m[3], bits, (2 << (bits - 1)) - 1);
    }
    
    if(rle_prefix || rle_postfix) {
        if(m[3] > 1) {
            if(rle_prefix) printf("%d%c", m[3], m[6]);
            else printf("%c%d", m[6], m[3]);
            m[3] = 0;
        } else if(m[3]) {
            putchar(m[6]);
            m[3] = 0;
        }
    } else {
        if(m[3] < 15 || !freecell || m[6] != '+' || disable_opt) {
            while (m[3]) {
                putchar(m[6]);
                m[3]--;
            }
        } else {
            translate(m[3], best_base(m[3]));
            m[3] = 0;
        }
    }
    
    freecell = 0;
}

void outbf() {
    unsigned int r1, r4, skip_this = 0;

    if(m[6] >= 0 && m[6] <= 3 && tiny) {
        // first, last, pre, post
        skipped_inits++;
        skip_this = 1;
        return;
    }

    m[7] = C4;
    r4 = 0;
o1:;
    if (r4 >= m[6]) goto o2;
o3:;
    r1 = m[m[7]];
    m[7]++;
    if (r1 != '\0') goto o3;
    r4++;
    goto o1;
o2:;
    r1 = m[m[7]];
    if (r1 == '\0') goto o4;
    if (r1 != '1') goto o5;
    r1 = m[5];
o5:;
    if (r1 != '2') goto o6;
    r1 = m[4];
o6:;
    if (r1 == '#') { r1 = STACK-2; goto o11; }
    if (r1 != '*') goto o7;
    if (rseg == 0) r1 = m[9] + off * 2; // ???
    else r1 = m[9] + off;
    goto o11;
o7:;
    if (r1 != '^') goto o8;
    if (rseg == 0) r1 = m[10] + off * 2; // ???
    else r1 = m[10] + off;
    goto o11;
o8:;
    if (r1 < 'a') goto o9;
    if (r1 > 'z') goto o9;
    r1 = r1 - 'a';
o11:;
    if (r1 < m[8]) goto o12;
    r4 = 0;
    m[15] = r1 - m[8];
o14:;
    if(rle_prefix || rle_postfix) {
        if (m[15] <= r4) goto o13;
        if (m[15] > 1) {
            if(rle_prefix)
                printf("%d>", m[15]);
            else
                printf(">%d", m[15]);
            r4 = m[15];
        } else if(m[15]) {
            putchar('>');
            
            r4 = m[15];
        }
        goto o14;
    } else {
        if (m[15] <= r4)
            goto o13;
        putchar('>');
        r4++;
        goto o14;
    }
o12:;
    r4 = 0;
    m[15] = m[8] - r1;
o16:;
    if(rle_prefix || rle_postfix) {
        if (m[15] <= r4) goto o13;
        if (m[15] > 1) {
            if(rle_prefix)
                printf("%d<", m[15]);
            else
                printf("<%d", m[15]);
            r4 = m[15];
        } else if(m[15]) {
            putchar('<');
            
            r4 = m[15];
        }
        goto o16;
    } else {
        if (m[15] <= r4) goto o13;
        putchar('<');
        r4++;
        goto o16;
    }
o13:;
    m[8] = r1;
    goto o10;
o9:;
    putchar(r1);
o10:;
    m[7]++;
    goto o2;
o4:;
}

unsigned int inchar() {
    int c = getchar();
    if (c <= 0)
        c = 0;
    return c;
}

int stack_usage(int n, int base) {
    int sp = 0;
    
    while(n != 0) {
        sp++;
        n /= base;
    }
    
    return sp;
}

int grade(int n, int base) {
    int sp = 0, norm = 0;
    
    while(n != 0) {
        sp++;
        norm += n % base;
        n /= base;
    }
    
    return norm + (6 + base) * sp + (sp % 2 == 1 ? 4 : 0);
}

int best_base(int n) {
    int v = 0, b = 0, i = 2;
    
    for(; i < 60; i++) {
        int cv = grade(n, i);
        
        if(v == 0 || v > cv) {
            v = cv;
            b = i;
        }
    }
    
    return b;
}

void translate(int n, int base) {
    int stack[stack_usage(n, base) + 1], sp = 0, flip = 1;
    
    while(n != 0) {
        stack[sp++] = n % base;
        n /= base;
    }
    
    putchar('>');
    
    while(sp != 0) {
        sp--;
        
        int bc = base;
        
        while(stack[sp]--)
            putchar('+');
        
        if(sp != 0) {
            if(!flip) {
                putchar('[');
                putchar('>');

                while(bc--)
                    putchar('+');

                putchar('<');
                putchar('-');
                putchar(']');
                putchar('>');
            } else {
                putchar('[');
                putchar('<');

                while(bc--)
                    putchar('+');

                putchar('>');
                putchar('-');
                putchar(']');
                putchar('<');
            }
        }
        
        flip = !flip;
    }
    
    if(!flip) {
        putchar('[');
        putchar('-');
        putchar('<');
        putchar('+');
        putchar('>');
        putchar(']');
        putchar('<');
    }
}
