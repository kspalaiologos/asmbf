
#ifndef __BREAKPOINT_H
#define __BREAKPOINT_H

#include <stdint.h>

struct breakpoint {
    struct breakpoint * next;
    unsigned int code_location;
    unsigned int type1, type2, relop, imm1, imm2;
};

enum {
    CBP_TYPE_REG, CBP_TYPE_IMM, CBP_TYPE_MEM
};

enum {
    CBP_CMP_EQ, CBP_CMP_NE,
    CBP_CMP_GT, CBP_CMP_LT,
    CBP_CMP_LE, CBP_CMP_GE
};

/**
 * @brief Return the n-th conditional breakpoint defined.
 */
struct breakpoint * cbp_nth(unsigned int n);

/**
 * @brief Add a new conditional breakpoint. Type can be either
 * CBP_TYPE_REG, CBP_TYPE_IMM or CBP_TYPE_MEM.
 * 
 * @param t1 Type 1
 * @param c Condition
 * @param t2 Type 2
 * @param s1 Value 1
 * @param s2 Value 2
 * @return unsigned int 
 */
void cbp_add(int t1, int c, int t2, int s1, int s2);

/**
 * @brief Return the amount of conditional breakpoints defined.
 */
unsigned int cbp_count(void);

/**
 * @brief Delete a conditional breakpoint
 * 
 * @param n Breakpoint index.
 * @return 1 on error, 0 if successful.
 */
int cbp_delete(unsigned int n);

/**
 * @brief Update the position of all breakpoints, based on
 * the predicate.
 * 
 * @param callback returns the position delta, accepts the current location
 * as a parameter.
 */

void bp_offset(unsigned int (*callback)(unsigned int));

/**
 * @brief Get the breakpoint list root.
 * 
 * @deprecated
 */
struct breakpoint * bp_get(void);

/**
 * @brief Remove all breakpoints.
 */
void bp_clear(void);

/**
 * @brief Check whenever the breakpoint fires.
 * 
 * @param render Set to a truthy value if we're doing a render
 * pass (don't evaluate conditional breakpoints).
 * @param loc The current location (significant only for unconditional
 * breakpoints)
 */
int bp_check(int render, int loc);

/**
 * @brief Construct an unconditional breakpoint node.
 * 
 * @param code_loc The breakpoint location
 */
struct breakpoint * mk_bp(unsigned int code_loc);

/**
 * @brief Toggle an unconditional breakpoint.
 * 
 * @param loc Location of the breakpoint.
 * @return The breakpoint status.
 */
int bp_toggle(unsigned int loc);

#endif
