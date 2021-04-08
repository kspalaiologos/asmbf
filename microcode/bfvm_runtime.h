
#ifndef _BFVM_RUNTIME
#define _BFVM_RUNTIME

/* Expects: _BFVM_TYPE - either uint32_t or uint16_t (default).
            _BFVM_FREESTANDING - if building in freestanding mode. */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

_BFVM_TYPE t0, t1, t2, t3, F0, F1, F2, F3, mp, sp;

#ifndef _BFVM_FREESTANDING
    static int8_t inchar(void) {
        int8_t v = getchar();
        return v == EOF ? 0 : v;
    }

    _BFVM_TYPE * tape;

    static void debug(void) {
        printf("\n~ BFVM Breakpoint ~\n");
        
        for(int i = 0; i < 10; i++) {
            for(int j = 0; j < 10; j++)
                if(i * 10 + j == mp)
                    printf("  %X  ", tape[i * 10 + j]);
                else
                    printf(" !%X! ", tape[i * 10 + j]);
            puts("");
        }

        printf("\n~   registers   ~\n");
        printf("G:  %04X IP: %04X\n", tape[0], tape[1]);
        printf("R1: %04X R2: %04X\n", tape[5], tape[6]);
        printf("R3: %04X R4: %04X\n", tape[7], tape[8]);
        printf("IM: %04X R5: %04X\n", tape[9], tape[17]);
        printf("R6: %04X\n\n", tape[18]);
    }
#else
    _BFVM_TYPE * tape = (_BFVM_TYPE *) 0x7000;
#endif

#define _BFVM_OFF(x) ((x) - 'a')

/* unsigned pow */
static _BFVM_TYPE bfvm_pow(_BFVM_TYPE x, _BFVM_TYPE y) {
    _BFVM_TYPE i = 1, s = x;

    for(; i < y; i++)
        s *= x;
    
    return s;
}

static _BFVM_TYPE bfvm_par(_BFVM_TYPE x) {
    _BFVM_TYPE parity = 0;
    
    while(x) {
        parity ^= x;
        x >>= 1;
    }

    return parity & 1;
}

static _BFVM_TYPE gcd(_BFVM_TYPE a, _BFVM_TYPE b) {
    _BFVM_TYPE temp;

    while(b) {
        temp = a % b;
        a = b;
        b = temp;
    }

    return a;
}

#define asmbf_first \
    tape[0] = 1; tape[1] = 1; \
    mp = 1; while(tape[mp]) {

#define asmbf_last \
    mp = 1; }

#define asmbf_pre \
    if(a) {

#define asmbf_post \
    }

static void asmbf_immed(int offset) { mp += offset; }
static void asmbf_clear(int offset) { mp += offset; tape[mp] = 0; }

static void asmbf_add(int from1, int from2) {
    mp += from1;
    _BFVM_TYPE * cell = tape + mp;
    *cell += tape[mp += from2];
}

static void asmbf_and(int from1, int from2) {
    mp += from1;
    _BFVM_TYPE * cell = tape + mp;
    *cell = *cell && tape[mp += from2];
}

static void asmbf_dec(int offset) {
    mp += offset; tape[mp]--;
}

static void asmbf_div(int from1, int from2) {
    mp += from1;
    _BFVM_TYPE * cell = tape + mp;
    *cell /= tape[mp += from2];
}

#endif
