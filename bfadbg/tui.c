
#include <string.h>

#include <ncurses.h>

#include "tui.h"

void mkwindow(int x1, int y1, int x2, int y2, int shadow) {
    // Clear the area for the box (we may be temporarily drawing over something)
    for(int i = y1; i < y2; i++)
        for(int j = x1; j < x2; j++)
            mvaddch(i, j, ' ');

    // Draw the borders.
    mvhline(y1, x1, 0, x2-x1);
    mvhline(y2, x1, 0, x2-x1);
    mvvline(y1, x1, 0, y2-y1);
    mvvline(y1, x2, 0, y2-y1);

    // Draw corners.
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
    
    // Draw optional shadow.
    if(shadow) {
        mvhline(y2 + 1, x1 + 1, ' ', x2 - x1);
        mvvline(y1 + 1, x2 + 1, ' ', y2 - y1 + 1);
    }
}

// TODO: Allow formatted output?
int msgbox(char * msg, char * responses) {
    int lines = 0, longestline = 0;
    int boldstatus = 0, currentline = 0;
    char * p = msg;

    // First of all, find the longest line and the total amount of
    // lines, to calculate where to put the message box.
    while(*p) {
        if(*p == '\n') {
            lines++;

            if(currentline > longestline)
                longestline = currentline;
            
            currentline = 0;
        } else if(*p == '%' && p[1] == '%')
            currentline++, p++;
        else if(*p == '%')
            ;
        else
            currentline++;
        p++;
    }

    // Guard against overflowing the box if the last line is the
    // longest of them all.
    if(currentline > longestline)
        longestline = currentline;

    // Add 3 characters extra for the border alignment
    // and 4 lines extra for the ok button, frames, and the spacing
    // between the OK button and the content.
    int totalwidth = longestline + 3;
    int totalheight = lines + 4;

    // Calculate a pair of points where to draw the box.
    int x1 = (EDITOR_WIDTH - totalwidth) / 2;
    int y1 = (EDITOR_HEIGHT - totalheight) / 2;
    int x2 = x1 + totalwidth;
    int y2 = y1 + totalheight;

    // Draw the window.
    mkwindow(x1, y1, x2, y2, 1);
    
    // Render the OK button.
    mvprintw(y2 - 1, x2 - 8, " < OK > ");
    
    // Reset the pointer we'll operate on.
    p = msg;

    for(int i = 0; i <= lines; i++) {
        move(y1 + 1 + i, x1 + 2);
        
        // Display until the end of the line
        while(*p != '\n' && *p != '\0') {
            // Use %% as an escape to print a normal percent.
            if(*p == '%' && p[1] == '%') {
                addch(*p);
                p+=2;
            } else if(*p == '%') {
                // Switch the color attribute.
                if(!boldstatus) {
                    attron(COLOR_PAIR(6));
                } else {
                    attroff(COLOR_PAIR(6));
                }

                boldstatus = !boldstatus;
                p++;
            } else {
                addch(*p);
                p++;
            }
        }

        p++;
    }

    // Move the cursor to the OK button location.

    move(y2 - 1, x2 - 5);
    refresh();

    // Start accepting input.

    int c;

    while(strchr(responses, c=getch()) == NULL);

    return c;
}

int kbhit(void) {
    int ch, r;
    nodelay(stdscr, TRUE);

    ch = getch();
    if(ch == ERR)
        r = FALSE;
    else {
        r = TRUE;
        ungetch(ch);
    }

    nodelay(stdscr, FALSE);
    return r;
}
