
#include <string.h>
#include <stdlib.h>

#include <ncurses.h>

#include "tui.h"
#include "buffer.h"
#include "bp.h"
#include "int.h"

static struct {
    uint8_t * code;
    uint32_t code_len;
    uint32_t target_loc;
    uint16_t cX, cY, file_offset;
} status;

static unsigned int comparator1(unsigned int loc) {
    return (loc >= status.target_loc);
}

static unsigned int comparator2(unsigned int loc) {
    return -(loc >= status.target_loc);
}

void buffer_broadcast_code(uint8_t * code, uint32_t length) {
    status.code = code;
    status.code_len = length;
}

int buffer_replace(unsigned int loc, uint8_t c) {
    if(loc >= status.code_len)
        return 1;

    if(bp_check(1, loc) && strchr("+-<>][.,", c) == NULL)
        bp_toggle(loc);

    status.code[loc] = c;
    
    return 0;
}

void buffer_reset_cursor(void) {
    status.cX = status.cY = 0;
}

int buffer_insert(unsigned int loc, uint8_t c) {
    if(loc < status.code_len + 2) {
        // 5 is a totally arbitrary number. Just keep some fucking space.
        status.code = realloc(status.code, ++status.code_len + 5);
        
        if(status.code_len != 1) {
            for(int i = status.code_len; i > loc; i--) {
                status.code[i] = status.code[i - 1];
            }

            status.code[loc] = c;
        } else {
            status.code[0] = c;
        }

        int_broadcast_code(status.code, status.code_len);

        status.target_loc = loc;
        bp_offset(comparator1);
    } else
        return 1;
    return 0;
}

int buffer_remove(unsigned int loc) {
    if(loc >= status.code_len)
        return 1;

    status.target_loc = loc;
    bp_offset(comparator2);

    for(int i = loc; i < status.code_len; i++) {
        status.code[i] = status.code[i + 1];
    }

    status.code_len--;
    
    return 0;
}

uint32_t buffer_length(void) {
    return status.code_len;
}

uint32_t buffer_calc(void) {
    return EDITOR_WIDTH * (status.file_offset + status.cY) + status.cX;
}

uint16_t buffer_queryX(void) {
    return status.cX;
}

uint16_t buffer_queryY(void) {
    return status.cY;
}

uint32_t buffer_offset(void) {
    return status.file_offset;
}

int8_t buffer_handle(uint8_t signal) {
    switch(signal) {
        case BUFFER_SIG_DOWN:
            if(!status.code)
                return 1;
            else if(status.cY > EDITOR_HEIGHT - 6) {
                if(status.file_offset * EDITOR_WIDTH < status.code_len)
                    status.file_offset++;
                else
                    return 1;
            } else
                status.cY++;
            return 0;
        case BUFFER_SIG_LEFT:
            if(status.cX > 0 && status.code) {
                status.cX--;
                return 0;
            }

            return 1;
        case BUFFER_SIG_PGDOWN:
            if(!status.code)
                return 1;
            else if((status.file_offset + 20) * EDITOR_WIDTH < status.code_len)
                status.file_offset += 20;
            else
                return 1;
            return 0;
        case BUFFER_SIG_PGUP:
            if(!status.code)
                return 1;
            else if(status.file_offset > 20)
                status.file_offset -= 20;
            else if(status.file_offset != 0)
                status.file_offset = 0, status.cY = 0;
            else
                return 1;
            return 0;
        case BUFFER_SIG_RIGHT:
            if(status.cX < EDITOR_WIDTH - 1 && status.code) {
                status.cX++;
                return 0;
            }

            return 1;
        case BUFFER_SIG_UP:
            if(!status.code)
                return 1;
            else if(status.cY == 0) {
                if(status.file_offset > 0)
                    status.file_offset--;
                else
                    return 1;
            } else
                status.cY--;
            return 0;
        default:
            return 1;
    }
}

void buffer_repaint(void) {
    if(!status.code) {
        for(int i = 3; i < EDITOR_HEIGHT; i++) {
            move(i, 0);
            for(int j = 0; j < EDITOR_WIDTH; j++)
                addch(' ');
        }

        return;
    }

    struct int_state * s = int_query_state();
    
    int marked_eof = 0;
    int buffer_pos = EDITOR_WIDTH * status.file_offset;
    int highlight_next = 0;
    
    for(int i = 3; i < EDITOR_HEIGHT; i++) {
        move(i, 0);

        for(int j = 0; j < EDITOR_WIDTH; j++) {
            if(buffer_pos >= status.code_len) {
                if(!marked_eof) {
                    if(s->ip == buffer_pos) {
                        attron(COLOR_PAIR(4));
                        addch('#');
                        attroff(COLOR_PAIR(4));
                    } else {
                        addch('#');
                    }

                    marked_eof = 1;
                } else addch(' ');

                continue;
            }

            int c = status.code[buffer_pos];

            if(status.cY == i - 3 && status.cX == j && c == '[') {
                highlight_next = 1;
            } else if(c == '[' && highlight_next) {
                highlight_next++;
            }

            if(s->ip == buffer_pos) {
                attron(COLOR_PAIR(4));
                addch(c);
                attroff(COLOR_PAIR(4));
            } else if(bp_check(1, buffer_pos) == 2) {
                attron(COLOR_PAIR(5));
                addch(c);
                attroff(COLOR_PAIR(5));
            } else if(c == ']' && highlight_next == 1) {
                attron(A_STANDOUT);
                addch(c);
                attroff(A_STANDOUT);

                highlight_next = 0;
            } else if(strchr("+-<>[]#.,", c) == NULL) {
                attroff(A_BOLD);
                addch(c);
                attron(A_BOLD);
            } else {
                addch(c);
            }

            if(c == ']' && highlight_next != 1) {
                highlight_next--;
            }
            
            buffer_pos++;
        }
    }
}
