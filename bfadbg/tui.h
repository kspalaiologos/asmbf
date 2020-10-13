
#ifndef _TUI_H
#define _TUI_H

#define EDITOR_WIDTH 62
#define EDITOR_HEIGHT 22

/**
 * @brief Display a popup box in the middle of the editor screen.
 * 
 * @param msg The message to display. It can be multiline.
 * @param accepted_responses List of all keys the user can strike while the message
 *                           box is open to produce effect. 
 * @return The response.
 */
int msgbox(char * msg, char * responses);

/**
 * @brief Draw an empty window.
 * 
 * @param x1 beginning X coordinate of the window.
 * @param y1 beginning Y coordinate of the window.
 * @param x2 finishing X coordinate of the window.
 * @param y2 finishing Y coordinate of the window.
 * @param shadow whether or not draw the box shadow.
 */
void mkwindow(int x1, int y1, int x2, int y2, int shadow);

/**
 * @brief Return true whenever there's input pending in the buffer.
 */
int kbhit(void);

#endif
