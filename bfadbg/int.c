
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "int.h"
#include "status.h"

static struct int_state status;

static int match(int dir) {
    int level = 1;
    char ipvalue;

    do {
        status.ip += dir;
        
        if (status.ip < 0 || status.ip > status.code_len) {
            broadcast_status("Mismatched brackets.");
            return 1;
        }
        
        ipvalue = status.code[status.ip];
        
        if (ipvalue == 91)
            level += dir;
        else if (ipvalue == 93)
            level -= dir;
    } while (level > 0);

    return 0;
}

struct int_state * int_query_state(void) {
    return &status;
}

void int_broadcast_code(char * c, uint32_t len) {
    status.code_len = len;
    status.code = c;
}

void int_initialize(void) {
    free(status.tape);
    status.tape = malloc(sizeof(uint16_t) * 65536);
    memset(status.tape, 0, sizeof(uint16_t) * 65536);
    status.mp = 0;
    status.ip = 0;
    status.maxmp = 65536;
}

int int_step(void) {
    if(status.code == NULL)
        return 0;
    else if(status.ip > status.code_len) {
        broadcast_status("Finished.");
        return 0;
    }

    switch(status.code[status.ip]) {
        case '>':
            if (status.mp >= status.maxmp) {
                status.tape = realloc(status.tape, (status.maxmp + 1024) * sizeof(uint16_t));
                
                if (status.tape == NULL) {
                    broadcast_status("Resizing tape: Out of memory.");
                    return 0;
                }
                
                for (int n = 1; n < 1024; n++)
                    status.tape[status.maxmp + n] = 0;
                
                status.maxmp += 1024;
            }
            status.mp++;
            status.cycles++;
            break;
        case '<':
            if (status.mp <= 0) {
                broadcast_status("Access Violation.");
                
                for(int i = 0; i < 5; i++)
                    beep();
                return 0;
            }
            
            status.mp--;
            status.cycles++;
            break;
        case '+':
            status.tape[status.mp]++;
            status.cycles++;
            break;
        case '-':
            status.tape[status.mp]--;
            status.cycles++;
            break;
        case '[':
            if (status.code[status.ip + 1] == '-' && status.code[status.ip + 2] == ']') {
                status.cycles += status.tape[status.mp] * 3;
                status.tape[status.mp] = 0;
                status.ip += 2;
            } else if (status.tape[status.mp] == 0)
                if(match(1))
                    return 0;
            status.cycles++;
            break;
        case ']':
            if(match(-1))
                return 0;
            status.ip--;
            status.cycles++;
            break;
        case '.':
            if(status.output_p < 32) {
                status.bfoutput[status.output_p++] = status.tape[status.mp];
            } else {
                for(int i = 0; i < 30; i++) {
                    status.bfoutput[i] = status.bfoutput[i + 1];
                }

                status.bfoutput[31] = status.tape[status.mp];
            }

            break;
        case ',':
            status.tape[status.mp] = status.input_q[0];
            for(int i = 0; i < 12; i++)
                status.input_q[i] = status.input_q[i + 1];
            status.input_q[12] = 0;
            break;
        case '#':
            if(status.ip + 1 >= status.code_len) {
                return 0;
            }
            
            if(!status.brkcmd) {
                status.ip++;
                return 2;
            }
        default:
            while(strchr("+-><[].,#", status.code[status.ip]) == NULL)
                status.ip++;
            return 1;
    }

    status.ip++;

    return 1;
}

static uint16_t int_evalbp_side(int type, int imm) {
    // TODO: Externalize this
    const unsigned int offset = 23;

    switch(type) {
        case CBP_TYPE_REG: return status.tape[imm];
        case CBP_TYPE_IMM: return imm;
        case CBP_TYPE_MEM: return status.tape[imm * 2 + offset];
        default: /* TODO: Do something */ return 0;
    }
}

uint16_t int_evalbp_l(struct breakpoint * b) {
    return int_evalbp_side(b->type1, b->imm1);
}

uint16_t int_evalbp_r(struct breakpoint * b) {
    return int_evalbp_side(b->type2, b->imm2);
}

void int_toggle_brkcmd(void) {
    status.brkcmd = !status.brkcmd;
}
