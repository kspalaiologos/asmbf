
#include <stdlib.h>

#include "int.h"
#include "bp.h"

#define IS_CONDITIONAL(b) ((b)->code_location == -1)

static struct breakpoint * breakpoints;

// TODO: Breakpoint count cache.

struct breakpoint * cbp_nth(unsigned int n) {
    struct breakpoint * l = breakpoints;
    
    for(int i = 0; i <= n; i++) {
        if(l && IS_CONDITIONAL(l))
            if(i != n)
                l = l->next;
            else
                return l;
        else if(l)
            l = l->next, n++;
        else
            return NULL;
    }

    // Case 1: No such breakpoint found, or the list
    // contains only unconditional breakpoints.
    if(!l || (l && !IS_CONDITIONAL(l)))
        return NULL;
    
    // Case 2: Everything went smoothly.
    return l;
}

int cbp_delete(unsigned int n) {
    struct breakpoint * l = breakpoints,
                      * p = NULL;
    
    for(int i = 0; i < n; i++) {
        if(l && IS_CONDITIONAL(l)) {
            p = l;
            l = l->next;
        } else if(l)
            n++;
        else
            return 1;
    }

    // The breakpoint list is too short.
    if(!l)
        return 1;

    // Two cases. We may be removing the first element
    // of the breakpoint list, so take care of that.
    if(p) {
        p->next = l->next;
    } else {
        breakpoints = l->next;
    }

    free(l);

    return 0;
}

unsigned int cbp_count(void) {
    struct breakpoint * l = breakpoints;
    unsigned int n = 0;

    while(l) {
        if(IS_CONDITIONAL(l))
            n++;
        l = l->next;
    }

    return n;
}

void bp_offset(uint32_t (*callback)(uint32_t)) {
    struct breakpoint * l = breakpoints;
    struct breakpoint * p = NULL;

    while(l) {
        if(IS_CONDITIONAL(l)) {
            l = l->next;
            continue;
        }

        l->code_location += callback(l->code_location);
        
        if(l->code_location < 0) {
            if(p) {
                p->next = l->next;
            } else {
                breakpoints = l->next;
            }
            
            free(l);
        }

        l = l->next;
    }
}

void cbp_add(int type1, int cond, int type2, int s1, int s2) {
    struct breakpoint * b = malloc(sizeof(struct breakpoint));
    b->code_location = -1;
    b->next = NULL;

    b->type1 = type1;
    b->imm1 = s1;

    b->relop = cond;

    b->type2 = type2;
    b->imm2 = s2;

    struct breakpoint * p = breakpoints;
    
    if(p) {
        while(p->next)
            p = p->next;
        
        p->next = b;
    } else {
        breakpoints = b;
    }
}

struct breakpoint * bp_get(void) {
    return breakpoints;
}

void bp_clear(void) {
    while(breakpoints) {
        struct breakpoint * next = breakpoints->next;
        free(breakpoints);
        breakpoints = next;
    }
}

struct breakpoint * mk_bp(unsigned int code_loc) {
    struct breakpoint * b = malloc(sizeof(struct breakpoint));
    b->next = NULL;
    b->code_location = code_loc;
    return b;
}

int bp_toggle(unsigned int loc) {
    if(breakpoints) {
        struct breakpoint * bp = breakpoints, * prev = NULL;
        
        while(bp) {
            if(bp->code_location == loc) {
                if(prev) {
                    prev->next = bp->next;
                    free(bp);
                } else {
                    breakpoints = bp->next;
                    free(bp);
                }

                return 0;
            }

            if(bp->next) {
                prev = bp;
                bp = bp->next;
            } else
                break;
        }

        bp->next = mk_bp(loc);
    } else {
        breakpoints = mk_bp(loc);
    }

    return 1;
}

static int condition(uint16_t g1, int c, uint16_t g2) {
    switch(c) {
        case CBP_CMP_EQ: return g1 == g2;
        case CBP_CMP_NE: return g1 != g2;
        case CBP_CMP_GT: return g1 > g2;
        case CBP_CMP_LT: return g1 < g2;
        case CBP_CMP_LE: return g1 <= g2;
        case CBP_CMP_GE: return g1 >= g2;
        default:
            /* TODO: do something */
            return 0;
    }
}

int bp_check(int render, int loc) {
    struct breakpoint * b = breakpoints;
    
    while(b) {
        if(IS_CONDITIONAL(b) && !render) {
            if(condition(int_evalbp_l(b), b->relop, int_evalbp_r(b)))
                return 1;
        } else if(b->code_location == loc)
            return 2;
        b = b->next;
    }

    return 0;
}
