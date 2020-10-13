
#ifndef _STATUS_H
#define _STATUS_H

/**
 * @brief Broadcast a message to the statusbar.
 */
void broadcast_status(char * message);

/**
 * @brief Broadcast a message with a single numeric parameter to the statusbar.
 */
void broadcast_status2(char * message, int nval);

/**
 * @brief Clear the status (restore RAM usage / scroll percentage)
 */
void clear_status(void);

/**
 * @brief Enter the command entry mode
 * 
 * @return command buffer pointer, guaranteed to be at
 * least 60 and at most 70 bytes big.
 */
char * command_status(void);

/**
 * @brief Initialize the statusbar.
 */
void init_status(void);

/**
 * @brief Repaint the statusbar.
 */
void repaint_status(void);

#endif
