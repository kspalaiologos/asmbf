
#ifndef _BFVM_RUNTIME
#define _BFVM_RUNTIME

/* Expects: _BFVM_TYPE - either uint32_t or uint16_t (default).
            _BFVM_FREESTANDING - if building in freestanding mode. */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

_BFVM_TYPE mp, sp;

#ifndef _BFVM_FREESTANDING
    _BFVM_TYPE * tape;

    static void debug(void) {
        fprintf(stderr, "\n~ BFVM Breakpoint ~\n");
        
        for(int i = 0; i < 10; i++) {
            for(int j = 0; j < 10; j++)
                if(i * 10 + j == mp)
                    fprintf(stderr, " !%X! ", tape[i * 10 + j]);
                else
                    fprintf(stderr, "  %X  ", tape[i * 10 + j]);
            fputs("\n", stderr);
        }

        fprintf(stderr, "\n~   registers   ~\n");
        fprintf(stderr, "G:  %04X IP: %04X\n", tape[0], tape[1]);
        fprintf(stderr, "R1: %04X R2: %04X\n", tape[5], tape[6]);
        fprintf(stderr, "R3: %04X R4: %04X\n", tape[7], tape[8]);
        fprintf(stderr, "IM: %04X R5: %04X\n", tape[9], tape[17]);
        fprintf(stderr, "R6: %04X\n\n", tape[18]);
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
    if(tape[0]) tape[1] = 0; mp = 1; }

#define asmbf_pre \
    mp = 0; if(tape[mp]) {

#define asmbf_post \
    }

#define asmbf_end \
    return 0;

static void asmbf_immed(int offset) { mp += offset; }
static void asmbf_clear(int offset) { mp += offset; tape[mp] = 0; }

#define _BFVM_DYAD(name, oper) \
    static void asmbf_ ## name(int from1, int from2) {\
        mp += from1; \
        _BFVM_TYPE * cell = tape + mp; \
        *cell = (*cell) oper (tape[mp += from2]); \
    }

_BFVM_DYAD(add, +)
_BFVM_DYAD(and, &&)
_BFVM_DYAD(div, /)
_BFVM_DYAD(eq, ==)
_BFVM_DYAD(ge, >=)
_BFVM_DYAD(gt, >)
_BFVM_DYAD(le, <=)
_BFVM_DYAD(lt, <)
_BFVM_DYAD(mod, %)
_BFVM_DYAD(mul, *)
_BFVM_DYAD(ne, !=)
_BFVM_DYAD(or, ||)
_BFVM_DYAD(sub, -)

static void asmbf_pow(int from1, int from2) {
    mp += from1;
    _BFVM_TYPE * cell = tape + mp;
    *cell = bfvm_pow(*cell, tape[mp += from2]);
}

static void asmbf_dec(int offset) { mp += offset; tape[mp]--; }
static void asmbf_inc(int offset) { mp += offset; tape[mp]++; }
static void asmbf_asr(int offset) { mp += offset; tape[mp] >>= 1; }
static void asmbf_asl(int offset) { mp += offset; tape[mp] <<= 1; }
static void asmbf_jmp(int loc) { mp += loc; tape[0] = 0; tape[1] = tape[mp]; }
static void asmbf_neg(int offset) { mp += offset; tape[mp] = 0 - tape[mp]; }
static void asmbf_not(int offset) { mp += offset; tape[mp] = 1 - tape[mp]; }
static void asmbf_log(int offset) { mp += offset; tape[mp] = tape[mp] > 0; }

static void asmbf_in(int offset) {
    mp += offset;

    #ifndef _FREESTANDING
        int8_t v = getchar();
        tape[mp] = v == EOF ? 0 : v;
    #else
        tape[mp] = 0;
    #endif
}

static void asmbf_out(int offset) {
    mp += offset;

    #ifndef _FREESTANDING
        putchar(tape[mp]);
    #endif
}

static void asmbf_jnz(int offset, int loc) {
    mp += offset; _BFVM_TYPE val = tape[mp]; mp += loc;
    if(val == 0) return; tape[0] = 0; tape[1] = tape[mp];
}

static void asmbf_jz(int offset, int loc) {
    mp += offset; _BFVM_TYPE val = tape[mp]; mp += loc;
    if(val != 0) return; tape[0] = 0; tape[1] = tape[mp];
}

static void asmbf_mov(int from1, int from2) {
    mp += from1;
    _BFVM_TYPE * cell = tape + mp;
    *cell = tape[mp += from2];
}

static void asmbf_push(int from, int stack_off) {
    mp += from;
    _BFVM_TYPE to_push = tape[mp];
    mp += stack_off;
    tape[mp + 2 * sp++] = to_push;
}

static void asmbf_srv(int stack_off) {
    mp += stack_off;
    _BFVM_TYPE val1 = tape[mp + 2 * --sp];
    _BFVM_TYPE val2 = tape[mp + 2 * --sp];
    tape[mp + 2 * sp++] = val1;
    tape[mp + 2 * sp++] = val2;
}

static void asmbf_pop(int dest, int stack_off) {
    mp += dest;
    _BFVM_TYPE * cell = tape + mp;
    mp += stack_off;
    *cell = tape[mp + 2 * --sp];
}

static void asmbf_rcl(int dest, int addr, int ram_off) {
    mp += dest;
    _BFVM_TYPE * data = tape + mp;
    mp += addr;
    _BFVM_TYPE xaddr = tape[mp];
    mp += ram_off;
    *data = tape[mp + 2 + 2 * xaddr];
}

static void asmbf_sto(int addr, int src, int ram_off) {
    mp += addr;
    _BFVM_TYPE xaddr = tape[mp];
    mp += src;
    _BFVM_TYPE xsrc = tape[mp];
    mp += ram_off;
    tape[mp + 2 + 2 * xaddr] = xsrc;
}

static void asmbf_amp(int addr, int src, int ram_off) {
    mp += addr;
    _BFVM_TYPE xaddr = tape[mp];
    mp += src;
    _BFVM_TYPE xsrc = tape[mp];
    mp += ram_off;
    tape[mp + 2 + 2 * xaddr] += xsrc;
}

static void asmbf_smp(int addr, int src, int ram_off) {
    mp += addr;
    _BFVM_TYPE xaddr = tape[mp];
    mp += src;
    _BFVM_TYPE xsrc = tape[mp];
    mp += ram_off;
    tape[mp + 2 + 2 * xaddr] -= xsrc;
}

static void asmbf_swp(int opr1, int opr2) {
    mp += opr1; _BFVM_TYPE * addr1 = tape + mp;
    mp += opr2; _BFVM_TYPE * addr2 = tape + mp;

    _BFVM_TYPE tmp = *addr1;
    *addr1 = *addr2;
    *addr2 = tmp;
}

static void asmbf_ret(int stack_off) {
    mp += stack_off;
    tape[0] = 0;
    tape[1] = tape[mp + 2 * --sp];
}

#endif
