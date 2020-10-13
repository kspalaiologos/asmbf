
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <ncurses.h>

#include "tui.h"
#include "status.h"
#include "bfadbg.h"

enum {
    STATUS_HELLO, STATUS_ENTER, STATUS_EMPTY, STATUS_CUSTOM,
    STATUS_CUSTOM_N
};

static struct {
    int type;
    char * command;
    char * message;
    unsigned int nval;
} status;

// TODO: Check is the status is within the bounds.
void broadcast_status(char * message) {
    status.message = message;
    status.type = STATUS_CUSTOM;
}

// TODO: Check if the status is within the bounds.
void broadcast_status2(char * message, int nval) {
    status.message = message;
    status.nval = nval;
    status.type = STATUS_CUSTOM_N;
}

void clear_status(void) {
    status.type = STATUS_EMPTY;
}

char * command_status(void) {
    status.type = STATUS_ENTER;
    return status.command;
}

void init_status(void) {
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    status.command = malloc(sizeof(char) * 70);
    status.type = STATUS_HELLO;
}

static void mem_usage(int * currMem, int * peakMem) {
    int currRealMem = 0, peakRealMem = 0, currVirtMem = 0, peakVirtMem = 0;

    char buffer[1024] = "";

    FILE* file = fopen("/proc/self/status", "r");

    if(!file)
        goto set;

    while (fscanf(file, " %1023s", buffer) == 1) {
        if (strcmp(buffer, "VmRSS:") == 0) {
            fscanf(file, " %d", &currRealMem);
        }

        if (strcmp(buffer, "VmHWM:") == 0) {
            fscanf(file, " %d", &peakRealMem);
        }

        if (strcmp(buffer, "VmSize:") == 0) {
            fscanf(file, " %d", &currVirtMem);
        }

        if (strcmp(buffer, "VmPeak:") == 0) {
            fscanf(file, " %d", &peakVirtMem);
        }
    }

    fclose(file);

    set:
        *currMem = currRealMem + currVirtMem;
        *peakMem = peakRealMem + peakVirtMem;
}

void repaint_status(void) {
    int q;

    attron(COLOR_PAIR(1));

    mvhline(EDITOR_HEIGHT, 0, 0, EDITOR_WIDTH);

    move(EDITOR_HEIGHT + 1, 0);

    switch(status.type) {
        case STATUS_HELLO:
            printw("bfadbg: a debugger for asm2bf. (C) 2020 Kamila Szewczyk. Try :help!     | READY ");
            break;

        case STATUS_EMPTY: {
            printw("                                                                   ");

            int cM, pM;
            mem_usage(&cM, &pM);
            
            if(cM && pM) {
                printw("%2dM/%2dM  ", cM/1024, pM/1024);
            } else {
                printw("       ");
            }

            if(buffer_length()) {
                int x = (buffer_offset() * 6200 / buffer_length());
                if(x > 100) x = 100;
                printw("%d%% ", x);
            } else {
                printw("    ");
            }
            break;
        }

        case STATUS_CUSTOM:
            addstr(status.message);
            for(int i = 0; i < 81 - strlen(status.message); i++)
                addch(' ');
            break;

        case STATUS_CUSTOM_N:
            q=printw(status.message, status.nval);
            for(int i = 0; i < 81 - q; i++)
                addch(' ');
            break;

        case STATUS_ENTER:
            addstr(status.command);
            for(int i = 0; i < 81 - strlen(status.command); i++)
                addch(' ');
            break;
    }
    
    attroff(COLOR_PAIR(1));
}
