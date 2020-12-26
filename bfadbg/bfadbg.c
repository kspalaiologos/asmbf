
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#include "status.h"
#include "bfadbg.h"
#include "tui.h"
#include "help.h"
#include "bp.h"
#include "buffer.h"
#include "int.h"

// Note: Indeed. This source file requires a refactor.

static struct {
    uint16_t cur_off;

    uint8_t iomode;

    uint8_t * code;
    uint32_t code_len;
} status;

uint8_t * get_code(void) {
    return status.code;
}

static void extended_memory(void) {
    // display ascii dump, detect stack, taperam etc... in a window
    // allow segment switching / stack hiding

    // generally, treat this as a TODO to be finished after this file is cleaned.
}

static void interpreter_state(void) {
    const int y1 = 7, y2 = 13, x1 = 20, x2 = 39;
    char b1[13], b2[13], b3[13];

    // A morphable copy of the interpreter state.
    struct int_state * s = int_query_state();

    memset(b1, '.', 12); b1[12] = 0;
    memset(b2, '.', 12); b2[12] = 0;
    memset(b3, '.', 12); b3[12] = 0;

    b1[sprintf(b1, "%d", s->ip)] = '.';
    b2[sprintf(b2, "%d", s->mp)] = '.';
    b3[sprintf(b3, "%s", s->input_q)] = '.';

    if(b1[12] != 0) b1[12] = 0;
    if(b2[12] != 0) b2[12] = 0;
    if(b3[12] != 0) b3[12] = 0;

    attron(COLOR_PAIR(7));

    mkwindow(x1, y1, x2, y2, 1);

    mvprintw( 8, 22, "IP:");
    mvprintw( 9, 22, "MP:");
    mvprintw(10, 22, "IN:");

    move(12, 33);
    refresh();

    int selid = 3;

    while(1) {
        if(selid == 0) attron(A_STANDOUT);
        mvprintw( 8, 26, "%s", b1);
        if(selid == 0) attroff(A_STANDOUT);
        
        if(selid == 1) attron(A_STANDOUT);
        mvprintw( 9, 26, "%s", b2);
        if(selid == 1) attroff(A_STANDOUT);

        if(selid == 2) attron(A_STANDOUT);
        mvprintw(10, 26, "%s", b3);
        if(selid == 2) attroff(A_STANDOUT);

        if(selid == 3) attron(A_STANDOUT);
        mvprintw(12, 32, "< OK >");
        if(selid == 3) attroff(A_STANDOUT);

        move(12, 33);
        refresh();

        int c = getch();

        if(isprint(c) && selid != 3) {
            if(selid == 0) {
                char * p = strchr(b1, '.');

                if(p)
                    *p = c;
                else
                    beep();
            } else if(selid == 1) {
                char * p = strchr(b2, '.');
                
                if(p)
                    *p = c;
                else
                    beep();
            } else if(selid == 2) {
                char * p = strchr(b3, '.');
                
                if(p)
                    *p = c;
                else
                    beep();
            }
        } else if(c == KEY_BACKSPACE && selid != 3) {
            if(selid == 0) {
                char * p = strchr(b1, '.');
                if(!p) {
                    b1[11] = '.';
                } else if(p > b1) {
                    p[-1] = '.';
                } else beep();
            } else if(selid == 1) {
                char * p = strchr(b2, '.');
                if(!p) {
                    b2[11] = '.';
                } else if(p > b2) {
                    p[-1] = '.';
                } else beep();
            } else if(selid == 2) {
                char * p = strchr(b3, '.');
                if(!p) {
                    b3[11] = '.';
                } else if(p > b3) {
                    p[-1] = '.';
                } else beep();
            }
        } else if(c == '\n' && selid == 3) {
            // save changes
            char * x;

            x = strchr(b1, '.'); if(x) *x = 0;
            x = strchr(b2, '.'); if(x) *x = 0;

            while(x = strrchr(b3, '.'))
                *x = 0;

            int newip = atoi(b1);
            int newmp = atoi(b2);

            if(newmp >= s->maxmp) {
                beep();
                return;
            }

            if(newip > status.code_len) {
                beep();
                return;
            }

            s->ip = newip;
            s->mp = newmp;

            memcpy(s->input_q, b3, 13);

            break;
        } else if(c == '\t' || c == '\n')
            selid = (selid + 1) % 4;
        else if(c == 'q') {
            break;
        }
    }

    attroff(COLOR_PAIR(7));
}

static void cond_brk_list(void) {
    const int y1 = 7, y2 = 13, x1 = 17, x2 = 45;
    const char * locs[] = {
        "G", "IP", "C", "D", "E", "R1", "R2", "R3", "R4", "IM",
        "K", "L", "M", "N", "O", "P", "F", "R5", "R6"
    };

    attron(COLOR_PAIR(6));

    int offset = 0, sel = 0, nb = cbp_count() - 1;
    
    while(1) {
        mkwindow(x1, y1, x2, y2, 1);

        for(int i = 0; i < 5; i++) {
            move(8 + i, 18);
            printw(" #%02d ", offset + i);

            struct breakpoint * b = cbp_nth(offset + i);

            if(!b) {
                if(sel && i == 0) attron(A_STANDOUT);
                printw("(none)");
                if(sel && i == 0) attroff(A_STANDOUT);
            } else {
                int len = 0;

                if(sel && i == 0) attron(A_STANDOUT);

                if(b->type1 == 0)
                    len += printw("%s", locs[b->imm1]);
                else if(b->type1 == 1)
                    len += printw("%d", b->imm1);
                else
                    len += printw("M(%d)", b->imm1);
                
                addch(' '); len++;

                switch(b->relop) {
                case 0: len+=printw("=="); break;
                case 1: len+=printw("!="); break;
                case 2: len+=printw(">"); break;
                case 3: len+=printw("<"); break;
                case 4: len+=printw("<="); break;
                case 5: len+=printw(">="); break;
                default: len+=printw("?");
                }

                addch(' '); len++;

                if(b->type2 == 0)
                    len+=printw("%s", locs[b->imm2]);
                else if(b->type2 == 1)
                    len+=printw("%d", b->imm2);
                else
                    len+=printw("M(%d)", b->imm2);
                
                if(sel && i == 0) attroff(A_STANDOUT);
            }
        }

        int c = getch();
        if(c == 'q')
            break;
        if(c == 'i') {
            sel = 1;
            if(offset > 0)
                offset--;
            else beep();
        } else if(c == 'o') {
            sel = 1;
            if(offset < nb)
                offset++;
            else beep();
        } else if(c == '\n') {
            if(cbp_nth(offset) == NULL) {
                beep();
            } else {
                cbp_delete(offset);

                nb = cbp_count() - 1;
            }

            sel = 1;
        }
    }

    attroff(COLOR_PAIR(6));
}

static int r2n(char * s1) {
         if(!strcmp(s1, "g"))  return  0;
    else if(!strcmp(s1, "ip")) return  1;
    else if(!strcmp(s1, "c"))  return  2;
    else if(!strcmp(s1, "d"))  return  3;
    else if(!strcmp(s1, "e"))  return  4;
    else if(!strcmp(s1, "r1")) return  5;
    else if(!strcmp(s1, "r2")) return  6;
    else if(!strcmp(s1, "r3")) return  7;
    else if(!strcmp(s1, "r4")) return  8;
    else if(!strcmp(s1, "im")) return  9;
    else if(!strcmp(s1, "k"))  return 10;
    else if(!strcmp(s1, "l"))  return 11;
    else if(!strcmp(s1, "m"))  return 12;
    else if(!strcmp(s1, "n"))  return 13;
    else if(!strcmp(s1, "o"))  return 14;
    else if(!strcmp(s1, "p"))  return 15;
    else if(!strcmp(s1, "f"))  return 16;
    else if(!strcmp(s1, "r5")) return 17;
    else if(!strcmp(s1, "r6")) return 18;
    else beep();               return -1;
}

static int parse_type(int type, char * s) {
    if(type == 0 && strlen(s) > 0) {
        s[0] = tolower(s[0]);
        s[1] = tolower(s[1]);
        
        return r2n(s);
    } else if(type == 1 || type == 2) {
        return atoi(s);
    }

    beep();
    return -1;
}

static void add_breakpoint(int type1, int cond, int type2, char * s1, char * s2) {
    // types: 0 - reg, 1 - imm, 2 - mem
    // cond: 0 - Eq 1 - Ne 2 - Gt 3 - Lt 4 - Le 5 - Ge

    int is1, is2;

    is1 = parse_type(type1, s1);
    is2 = parse_type(type2, s2);

    if(is1 == -1 || is2 == -1) {
        beep();
        return;
    }

    cbp_add(type1, cond, type2, is1, is2);
}

static void cond_brk(void) {
    attron(COLOR_PAIR(6));
    const int y1 = 7, y2 = 13, x1 = 17, x2 = 45;
    int c, t1 = 0, t2 = 4, t3 = 10, sf = 0;
    char ts1[10], ts2[10];
    memset(ts1, '.', 9); ts1[9] = 0;
    memset(ts2, '.', 9); ts2[9] = 0;
    mkwindow(x1, y1, x2, y2, 1);
    while(1) {
        mvprintw( 8, 18, "If                         ");

        #define TUI_SWITCH(tn,id,name) \
            if(sf == id) attron(A_STANDOUT); \
            if(tn == id) printw("["name"]"); else printw(name); \
            if(sf == id) attroff(A_STANDOUT);

        move(9, 18);
        printw(" ");

        TUI_SWITCH(t1, 0, "Reg"); printw("/");
        TUI_SWITCH(t1, 1, "Imm"); printw("/");
        TUI_SWITCH(t1, 2, "Mem");
        printw(" : ");
        if(sf == 3) attron(A_STANDOUT);
        printw("%s", ts1);
        if(sf == 3) attroff(A_STANDOUT);
        addch(' ');

        move(10, 18);
        printw("  ");

        TUI_SWITCH(t2, 4, "eq"); printw("/");
        TUI_SWITCH(t2, 5, "ne"); printw("/");
        TUI_SWITCH(t2, 6, "gt"); printw("/");
        TUI_SWITCH(t2, 7, "lt"); printw("/");
        TUI_SWITCH(t2, 8, "le"); printw("/");
        TUI_SWITCH(t2, 9, "ge"); printw("      ");

        move(11, 18);
        printw(" ");

        TUI_SWITCH(t3, 10, "Reg"); printw("/");
        TUI_SWITCH(t3, 11, "Imm"); printw("/");
        TUI_SWITCH(t3, 12, "Mem");
        printw(" : ");
        
        if(sf == 13) attron(A_STANDOUT);
        printw("%s", ts2);
        if(sf == 13) attroff(A_STANDOUT);
        addch(' ');

        mvprintw(12, 18, "                        ");
        if(sf == 14) attron(A_STANDOUT);
        mvprintw(12, 39, "< OK >");
        if(sf == 14) attroff(A_STANDOUT);

        if(sf == 3) {
            char * z = strchr(ts1, '.');
            unsigned int o = (z - ts1);
            move(9, z ? (35 + o) : 44);
            refresh();
        } else if(sf == 13) {
            char * z = strchr(ts2, '.');
            unsigned int o = (z - ts2);
            move(11, z ? (35 + o) : 44);
            refresh();
        } else {
            move(7, 17);
            refresh();
        }

        c = getch();

        if(c == '\t')
            sf = (sf + 1) % 15;
        else if(sf == 3) {
            // writing to text field 1
            if(c == '\n')
                sf++;
            else {
                char * p = strchr(ts1, '.');

                if(p && isprint(c))
                    *p = c;
                else if(c == KEY_BACKSPACE) {
                    if(p > ts1)
                        p[-1] = '.';
                    else if(!p)
                        ts1[8] = '.';
                    else
                        beep();
                } else
                    beep();
            }
        } else if(sf == 13) {
            // writing to text field 2
            if(c == '\n')
                sf++;
            else {
                char * p = strchr(ts2, '.');
                
                if(p && isprint(c))
                    *p = c;
                else if(c == KEY_BACKSPACE) {
                    if(p > ts2)
                        p[-1] = '.';
                    else if(!p)
                        ts2[8] = '.';
                    else
                        beep();
                } else
                    beep();
            }
        } else if(sf == 14 && c == '\n') {
            // add the breakpoint
            char * z = strchr(ts1, '.');
            if(z) *z = 0;
            char * x = strchr(ts2, '.');
            if(x) *x = 0;

            add_breakpoint(t1, t2 - 4, t3 - 10, ts1, ts2);
            break;
        }
        else if(c == ' ' && sf < 3)
            t1 = sf;
        else if(c == ' ' && sf > 3 && sf < 10)
            t2 = sf;
        else if(c == ' ' && sf > 9 && sf < 13)
            t3 = sf;
        else if(c == 'q')
            break;
    }

    attroff(COLOR_PAIR(6));
}

static void repaint_top(void) {
    const unsigned int offset = 23;

    struct int_state * s = int_query_state();

    attron(COLOR_PAIR(3));
    mvhline(2, 0, 0, 62);
    
    if(status.cur_off == 0) {
        move(0, 0);
        
        for(int i = 0; i < 12; i++)
            printw(" % 4X", status.cur_off + i);

        printw(" >");

        move(1, 0);

        for(int i = 0; i < 12; i++)
            printw(" %04X", s->tape[2 * status.cur_off + offset + i * 2]);

        printw(" >");
    } else {
        mvprintw(0, 0, "<");
        
        for(int i = 0; i < 12; i++)
            printw("% 4X ", status.cur_off + i);

        printw(">");

        mvprintw(1, 0, "<");

        for(int i = 0; i < 12; i++) {
            printw("%04X ", s->tape[2 * status.cur_off + offset + i * 2]);
        }

        printw(">");
    }

    attroff(COLOR_PAIR(3));
}

static void handle(char * command) {
    char * arg = strchr(command, ' ');
    
    struct int_state * s = int_query_state();

    if(arg != NULL)
        *arg = 0, arg++;

    if(!strcmp("load", command)) {
        if(!arg) {
            broadcast_status("Invalid invocation.");
            return;
        }

        // Note: code may be null here, but free shouldn't care.
        // C11 standard, chapter 7, section 22.3.3, point 3.
        free(status.code); status.code = NULL;

        int_initialize();

        FILE * infile = fopen(arg, "rb");
        int lof;
        
        if (infile == NULL) {
            broadcast_status("Error opening input file");
            return;
        }
        
        if (fseek(infile, 0L, SEEK_END) != 0) {
            broadcast_status("Error determining length of input file");
            return;
        }
        
        lof = ftell(infile);
        
        if (lof == -1) {
            broadcast_status("Error determining length of input file");
            return;
        }

        if (fseek(infile, 0L, SEEK_SET) != 0) {
            broadcast_status("Error determining length of input file");
            return;
        }

        if (lof == 0)
            return;
        
        status.code = (char *) calloc(lof + 2, sizeof(char));
        
        if (status.code == NULL) {
            broadcast_status("Program too big to fit in memory");
            return;
        }

        for(int i = 0; !feof(infile);) {
            status.code[i] = fgetc(infile);

            if(!isprint(status.code[i])) {
                status.code[i] = 0;
                lof--;
            } else {
                i++;
            }
        }

        if(status.code[lof++] == 0) {
            lof--;
        }

        status.code_len = lof;

        int_broadcast_code(status.code, status.code_len);
        buffer_broadcast_code(status.code, status.code_len);

        fclose(infile);
    } else if(!strcmp("unload", command)) {
        free(status.code);
        status.code = NULL;
        status.code_len = 0;

        int_broadcast_code(NULL, 0);
        buffer_broadcast_code(status.code, status.code_len);
        buffer_reset_cursor();

        bp_clear();
    } else if(!strcmp("mcl", command)) {
        int_initialize();
        broadcast_status("Memory cleared.");
    } else if(!strcmp("clc", command)) {
        s->output_p = 0;
        memset(s->bfoutput, 0, 32);
    } else if(!strcmp("clb", command)) {
        bp_clear();
    } else if(!strcmp("help", command)) {
        if(!arg)
            help_pager(0);
        else
            help_pager(atoi(arg));
    } else if(!strcmp("rc", command)) {
        s->cycles = 0;
    } else if(!strcmp("ci", command)) {
        if(!arg) {
            broadcast_status("Invalid invocation.");
            return;
        }

        int loc = buffer_calc();
        int stat;

        while(*arg && !(stat = buffer_insert(loc, *arg))) {
            arg++, loc++;
        }

        if(stat) {
            beep();
        }
    } else if(!strcmp("cr", command)) {
        if(!arg) {
            broadcast_status("Invalid invocation.");
            return;
        }

        int loc = buffer_calc();
        int stat;

        while(*arg && !(stat = buffer_replace(loc, *arg))) {
            arg++, loc++;
        }

        if(stat) {
            beep();
        }
    } else if(!strcmp("cp", command)) {
        if(!arg) {
            broadcast_status("Invalid invocation.");
            return;
        }

        int loc = buffer_calc();
        int n = atoi(arg), stat;

        for(int i = 0; i < n; i++) {
            stat = buffer_remove(loc);
            if(stat) break;
        }

        if(stat) {
            beep();
        }
    } else if(!strcmp("tgh", command)) {
        int_toggle_brkcmd();
    } else {
        broadcast_status("Unknown command.");
    }
}

static void repaint_side(void) {
    const int y1 = 0, y2 = 22, x1 = 62, x2 = 79;

    static uint16_t pG=0, pIP=0, pR1=0, pR2=0, pR3=0, pR4=0,
                    pR5=0, pR6=0, pF=0, pIM=0, pC=0, pD=0, pE=0,
                    pK=0, pL=0, pM=0, pN=0, pO=0, pP=0;
    
    // A read-only pointer to the interpreter status.
    struct int_state * s = int_query_state();

    attron(COLOR_PAIR(2));
    mkwindow(x1, y1, x2, y2, 0);

#define PRINT_ROW(Y, S1, S2, P1, P2, V1, V2) \
    move(Y, 63); \
    if(s->tape[P1] != V1) { \
        attron(A_STANDOUT); \
        printw(S1"%04X", s->tape[P1]); \
        V1 = s->tape[P1]; \
        attroff(A_STANDOUT); \
    } else { \
        printw(S1"%04X", s->tape[P1]); \
    } \
    printw(" | "); \
    if(P2 != -1) \
        if(s->tape[P2] != V2) { \
            attron(A_STANDOUT); \
            printw(S2"%04X", s->tape[P2]); \
            V2 = s->tape[P2]; \
            attroff(A_STANDOUT); \
        } else { \
            printw(S2"%04X", s->tape[P2]); \
        }

    PRINT_ROW( 1, " G ", "C ",  0,  2,  pG, pC)
    PRINT_ROW( 2, "IP ", "D ",  1,  3, pIP, pD)
    PRINT_ROW( 3, "R1 ", "E ",  5,  4, pR1, pE)
    PRINT_ROW( 4, "R2 ", "  ",  6, -1, pR2, pG)
    PRINT_ROW( 5, "R3 ", "K ",  7, 10, pR3, pK)
    PRINT_ROW( 6, "R4 ", "L ",  8, 11, pR4, pL)
    PRINT_ROW( 7, "R4 ", "M ", 17, 12, pR5, pM)
    PRINT_ROW( 8, "R6 ", "N ", 18, 13, pR6, pN)
    PRINT_ROW( 9, " F ", "O ", 16, 14,  pF, pO)
    PRINT_ROW(10, "IM ", "P ",  9, 15, pIM, pP)

#undef PRINT_ROW

    mvprintw(11, 63, "--------+-------");
    mvprintw(12, 63, "  T=%010X  ", s->cycles);
    mvprintw(13, 63, "  IP=%09X  ", s->ip);
    mvprintw(14, 63, "  MP=%09X  ", s->mp);
    mvprintw(15, 63, "----------------");
    mvprintw(16, 63, "Output:         ");

    if(status.iomode == 0) {
        int j = 0;

        for(int i = 17; i < 21; i++) {
            move(i, 63);
            
            for(int k = 0; k < 8; k++) {
                if(j >= s->output_p) {
                    printw(" ");
                    continue;
                }
                
                printw("%02X", s->bfoutput[j++]);
            }
        }
    } else if(status.iomode == 1) {
        int j = 0;

        for(int i = 17; i < 19; i++) {
            move(i, 63);
            
            for(int k = 0; k < 16; k++) {
                if(j >= s->output_p) {
                    printw(" ");
                    continue;
                }
                
                if(isprint(s->bfoutput[j]))
                    printw("%c", s->bfoutput[j]);
                else
                    printw(".");

                j++;
            }
        }
    }

    mvvline(0, 71, 0, 11);
    mvaddch(0, 71, ACS_TTEE);
    mvhline(11, 63, 0, 16);
    mvaddch(11, 71, ACS_BTEE);

    mvaddch(11, 62, ACS_LTEE);
    mvaddch(11, 79, ACS_RTEE);

    mvhline(15, 63, 0, 16);
    mvaddch(15, 62, ACS_LTEE);
    mvaddch(15, 79, ACS_RTEE);

    attroff(COLOR_PAIR(2));
}

void repaint_all(int do_cursor, int do_right) {
    repaint_top();
    repaint_status();
    
    if(do_right) {
        repaint_side();
    }

    buffer_repaint();
    
    if(do_cursor) {
        move(3 + buffer_queryY(), buffer_queryX());
        refresh();
    }
}

static void handle_mem_replace(char * str) {
    const unsigned int offset = 23;

    struct int_state * s = int_query_state();

    #define TAPE s->tape[2 * status.cur_off + offset]

         if(!strncmp( "=", str, 1)) TAPE  = atoi(str + 1);
    else if(!strncmp("+=", str, 2)) TAPE += atoi(str + 2);
    else if(!strncmp("-=", str, 2)) TAPE -= atoi(str + 2);
    else if(!strncmp("*=", str, 2)) TAPE *= atoi(str + 2);
    else beep();

    #undef TAPE
}

int main(void) {
    int c;
    struct int_state * s = int_query_state();

    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    start_color();

    init_status();
    int_broadcast_code(NULL, 0);
    buffer_broadcast_code(NULL, 0);

    attron(A_BOLD);

    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_RED);
    init_pair(5, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
    
    int_initialize();
    repaint_all(1, 1);

    while(c = getch()) {
        if(c == 'k') {
            status.cur_off += 1;
        } else if(c == 'l') {
            status.cur_off += 2;
        } else if(c == 'j') {
            if(status.cur_off > 0)
                status.cur_off -= 1;
            else
                beep();
        } else if(c == 'h') {
            if(status.cur_off > 1)
                status.cur_off -= 2;
            else
                beep();
        } else if(c == ':') {
            int ptr = 1;
            char * command = command_status();

            memset(command, 0, 70);
            command[0] = ':';

            repaint_all(1, 0);

            move(23, 1);
            refresh();

            while(c = getch()) {
                if(c == 10) {
                    clear_status();
                    handle(command + 1);
                    break;
                } else if(c == 17) {
                    // ctrl+q
                    clear_status();
                    break;
                } else if(c == KEY_BACKSPACE) {
                    if(ptr > 1)
                        command[--ptr] = 0;
                    else beep();
                } else if(isprint(c)) {
                    if(ptr >= 65) {
                        beep();
                    } else
                        command[ptr++] = c;
                } else beep();

                repaint_all(0, 0);

                move(23, strlen(command));
                refresh();
            }
        } else if(c == 'q') {
            break;
        } else if(c == 'u') {
            if(buffer_handle(BUFFER_SIG_LEFT))
                beep();
        } else if(c == 'p') {
            if(buffer_handle(BUFFER_SIG_RIGHT))
                beep();
        } else if(c == 'i') {
            if(buffer_handle(BUFFER_SIG_UP))
                beep();
        } else if(c == 'o') {
            if(buffer_handle(BUFFER_SIG_DOWN))
                beep();
        } else if(c == 's') {
            if(int_step() != 1) beep();
        } else if(c == 'a' || c == 'z') {
            int isbp, isbph;

            while(!kbhit() && (isbph=int_step()) == 1 && !(isbp=bp_check(0, s->ip))) {
                repaint_all(1, 1);
                usleep(c == 'a' ? 10000 : 100);
            }

            if(isbp) {
                broadcast_status("Breakpoint hit.");
            }

            if(isbph == 2) {
                broadcast_status("'#' breakpoint hit.");
            }

            if(kbhit())
                getch();
        } else if(c == 'c') {
            status.iomode = !status.iomode;
        } else if(c == 'b') {
            int loc = buffer_calc();

            if(loc > status.code_len || !status.code || strchr("+-<>[].,", status.code[loc]) == NULL)
                beep();
            else
                bp_toggle(loc);
        } else if(c == 'n') {
            if(buffer_handle(BUFFER_SIG_PGUP))
                beep();
        } else if(c == 'm') {
            if(buffer_handle(BUFFER_SIG_PGDOWN))
                beep();
        } else if(c == 'f') {
            int val,isbp,t=clock(),d=clock(),nval=0;

            broadcast_status2("Running... %d kIPS", 0);

            while(!kbhit() && (val=int_step()) == 1 && !(isbp=bp_check(0, s->ip))) {
                if((t-d)%CLOCKS_PER_SEC!=0)
                    nval++;
                else {
                    nval /= 1000;
                    broadcast_status2("Running... %d kIPS", 0);
                    repaint_status();
                    nval = 0;
                }

                t=clock();
            }
            
            if(kbhit()) {
                broadcast_status("Manual break.");
                getch();
            } else if(val == 0) {
                broadcast_status("Finished.");
            } else if(isbp) {
                broadcast_status("Breakpoint hit.");
            } else if(val == 2) {
                broadcast_status("'#' breakpoint hit.");
            }
        } else if(c == 'v') {
            clear_status();
        } else if(c == 'd') {
            cond_brk();
        } else if(c == 'y') {
            cond_brk_list();
        } else if(c == 'r') {
            int ptr = 4;
            char * command = command_status();

            memset(command, 0, 70);
            strcpy(command, "M(x)");

            repaint_all(1, 0);

            move(23, 4);
            refresh();

            while(c = getch()) {
                if(c == 10) {
                    clear_status();
                    handle_mem_replace(command + 4);
                    break;
                } else if(c == 17) {
                    // ctrl+q
                    clear_status();
                    break;
                } else if(c == KEY_BACKSPACE) {
                    if(ptr > 1)
                        command[--ptr] = 0;
                    else beep();
                } else if(isprint(c)) {
                    if(ptr >= 65) {
                        beep();
                    } else
                        command[ptr++] = c;
                } else beep();

                repaint_all(0, 0);

                move(23, strlen(command));
                refresh();
            }
        } else if(c == 'g') {
            int loc = buffer_calc();

            int c = getch();

            if(buffer_replace(loc, c))
                beep();
        } else if(c == 'w') {
            int loc = buffer_calc();
            
            int c = getch();

            if(buffer_insert(loc, c))
                beep();
        } else if(c == 'e') {
            interpreter_state();
        } else if(c == 'x') {
            int loc = buffer_calc();
            if(buffer_remove(loc))
                beep();
        } else if(c == 't') {
            extended_memory();
        }

        repaint_all(1, 1);
    }

    endwin();
}
