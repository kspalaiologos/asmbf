
#ifndef __INT_H
#define __INT_H

#include "bp.h"

struct int_state {
    uint16_t * tape;

    uint32_t ip, mp, maxmp, cycles, code_len;

    uint8_t bfoutput[32], output_p;
    uint8_t input_q[13];

    uint8_t * code;

    uint8_t brkcmd;
};

/**
 * @brief Evaluate the LHS of a breakpoint.
 */
uint16_t int_evalbp_l(struct breakpoint * b);

/**
 * @brief Evaluate the RHS of a breakpoint.
 */
uint16_t int_evalbp_r(struct breakpoint * b);

/**
 * @brief Single-step the interpreter.
 */
int int_step(void);

/**
 * @brief Initialize the interpreter.
 */
void int_initialize(void);

/**
 * @brief Broadcast new code buffer to the interpreter.
 */
void int_broadcast_code(char *, uint32_t);

/**
 * @brief Query the interpreter state.
 */
struct int_state * int_query_state(void);

/**
 * @brief Toggle the break-on-# behaviour.
 */
void int_toggle_brkcmd(void);

#endif
