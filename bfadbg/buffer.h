
#ifndef _BUFFER_H
#define _BUFFER_H

#include <stdint.h>

enum {
    BUFFER_SIG_PGUP, BUFFER_SIG_PGDOWN,
    BUFFER_SIG_UP, BUFFER_SIG_DOWN,
    BUFFER_SIG_LEFT, BUFFER_SIG_RIGHT
};

/**
 * @brief Broadcast a new buffer to the editor.
 * 
 * @param code Buffer contents
 * @param length Buffer length
 */
void buffer_broadcast_code(uint8_t * code, uint32_t length);

/**
 * @brief Replace a single byte in the buffer.
 * 
 * @param loc Byte location
 * @param c Character to replace with
 * @return 0 on success, 1 in case of failure.
 */
int buffer_replace(unsigned int loc, uint8_t c);

/**
 * @brief Insert a single byte to the buffer.
 * 
 * @param loc Byte location
 * @param c Character to insert
 * @return 0 on success, 1 in case of failure.
 */
int buffer_insert(unsigned int loc, uint8_t c);

/**
 * @brief Remove a byte from the buffer.
 * 
 * @param loc Byte location
 * @return 0 on success, 1 in case of failure.
 */
int buffer_remove(unsigned int loc);

/**
 * @brief Repaint the code buffer.
 */
void buffer_repaint(void);

/**
 * @brief Reset the position of the cursor.
 */
void buffer_reset_cursor(void);

/**
 * @brief Return the length of the current buffer.
 */
uint32_t buffer_length(void);

/**
 * @brief Calculate the position of the cursor within the file.
 */
uint32_t buffer_calc(void);

/**
 * @brief Handle a movement signal
 * 
 * @param signal Look BUFFER_SIG_*
 * @return int8_t 
 */
int8_t buffer_handle(uint8_t signal);

/**
 * @brief Query the X coordinate of the cursor.
 */
uint16_t buffer_queryX(void);

/**
 * @brief Query the Y coordinate of the cursor. 
 */
uint16_t buffer_queryY(void);

/**
 * @brief Query the scrolling offset of the file.
 */
uint32_t buffer_offset(void);

#endif
