
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config.h"

static unsigned int col;
#define wrap_putchar(x) do { \
    if(col == lw) { \
        putchar('\n'); \
        col = 0; \
    } \
    putchar(x); \
    col++; \
} while(0);

void process(int c, int type, int lw) {
    long i;
    long count = c - type;

    while ((c = getchar()) != EOF) {
        if(!strchr("+-<>[].,", c))
            continue;

        if ((c != type + 1) && (c != type - 1)) {
            ungetc(c, stdin);
            break;
        }

        count += c - type;
    }

    if(c == EOF)
        return;

    if (count < 0)
        for (i = -count; i; i--)
            wrap_putchar(type - 1);
    
    if (count > 0)
        for (i = count; i; i--)
            wrap_putchar(type + 1);
}

int main(int argc, char * argv[]) {
    int c, lw = -1;

    if(argc == 2) {
        lw = atoi(argv[1]);
    }

    while ((c = getchar()) != EOF) {
        switch (c) {
            case '<': case '>': process(c, '=', lw); break;
            case '+': case '-': process(c, ',', lw); break;
            case '[': case ']': case ',': case '.': wrap_putchar(c); break;
        }
    }
}
