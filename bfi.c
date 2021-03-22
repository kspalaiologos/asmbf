
/* asm2bf
 *
 * Copyright (C) K. Palaiologos Szewczyk 2017-2020 & maviek 2019.
 * License: MIT
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"

char * src;
long lof, ip = -1;

void match(int dir) {
    int level = 1;
    char ipvalue;
    do {
        ip += dir;
        #ifndef BFI_NOCHECKS
        if (ip < 0 || ip >= lof) {
            fputs("Mismatched brackets", stderr);
            exit(EXIT_FAILURE);
        }
        #endif
        ipvalue = src[ip];
        if (ipvalue == 91)
            level += dir;
        else if (ipvalue == 93)
            level -= dir;
    } while (level > 0);
}

int main(int argc, char * argv[]) {
    FILE * infile = NULL;
    long mp = 0, maxmp = 1023;
    int n, ic, nc = 0;
    bool xflag = false, dflag = false, cycles = false;
    unsigned short int * mem;

    if (argc < 2) {
        fprintf(stderr, "Error: no input.\n");
        return EXIT_FAILURE;
    }

    for (n = 1; n < argc; n++) {
        if (argv[n][0] == '-') {
            switch (argv[n][1]) {
                case 'x':
                    xflag = true;
                    break;
                
                case 'd':
                    dflag = true;
                    break;

                case 'c':
                    cycles = true;
                    break;
                
                default:
                    fprintf(stderr, "Error: unrecognized command line option '-%c'\n", argv[n][1]);
                    return EXIT_FAILURE;
            }
        } else {
            if (infile == NULL) {
                char *fn = argv[n];
                infile = fopen(fn, "rb");
                if (infile == NULL) {
                    fprintf(stderr, "Error: unable to open file %s\n", fn);
                    return EXIT_FAILURE;
                }
            } else {
                fprintf(stderr, "Error: multiple input files.\n");
                return EXIT_FAILURE;
            }
        }
    }

    #ifndef BFI_NOCHECKS
    if (infile == NULL) {
        fputs("Error opening input file", stderr);
        return EXIT_FAILURE;
    }
    
    if (fseek(infile, 0L, SEEK_END) != 0) {
        fputs("Error determining length of input file", stderr);
        return EXIT_FAILURE;
    }
    #else
    fseek(infile, 0L, SEEK_END);
    #endif
    lof = ftell(infile);
    #ifndef BFI_NOCHECKS
    if (lof == -1) {
        fputs("Error determining length of input file", stderr);
        return EXIT_FAILURE;
    }
    if (fseek(infile, 0L, SEEK_SET) != 0) {
        fputs("Error determining length of input file", stderr);
        return EXIT_FAILURE;
    }
    if (lof == 0)
        return EXIT_SUCCESS;
    #else
    fseek(infile, 0L, SEEK_SET);
    #endif
    src = (char *) calloc(lof + 2, sizeof(char));
    #ifndef BFI_NOCHECKS
    if (src == NULL) {
        fputs("Program too big to fit in memory", stderr);
        return EXIT_FAILURE;
    }
    if (fread(src, sizeof(char), lof, infile) < (unsigned) lof) {
        fputs("Error reading input file", stderr);
        return EXIT_FAILURE;
    }
    if (fclose(infile) == -1) {
        fputs("Error closing input file", stderr);
        return EXIT_FAILURE;
    }
    #else
        fread(src, sizeof(char), lof, infile);
        fclose(infile);
    #endif
    mem = calloc(1024, sizeof(unsigned short int));
    #ifndef BFI_NOCHECKS
    if (mem == NULL) {
        fputs("Out of memory", stderr);
        return EXIT_FAILURE;
    }
    #endif
    while (++ip < lof) {
        if(cycles) nc++;
        switch (src[ip]) {
            case '>':
                if (mp >= maxmp) {
                    mem = realloc(mem, (maxmp + 1024) * sizeof(unsigned short int));
                    #ifndef BFI_NOCHECKS
                    if (mem == NULL) {
                        fputs("Out of memory", stderr);
                        return EXIT_FAILURE;
                    }
                    #endif
                    for (n = 1; n < 1024; n++)
                        mem[maxmp + n] = 0;
                    maxmp += 1024;
                }
                mp++;
                break;
            case '<':
                #ifndef BFI_NOCHECKS
                if (mp <= 0) {
                    fprintf(stderr, "Access Violation, ip=%ld", ip);
                    return EXIT_FAILURE;
                }
                #endif
                mp--;
                break;
            case '+':
                mem[mp]++;
                break;
            case '-':
                mem[mp]--;
                break;
            case '.':
                if(dflag)
                    printf("%u ", mem[mp]);
                else
                    putchar(mem[mp]);
                
                fflush(stdout);
                break;
            case ',':
                n = getchar();
                if (n < 0)
                    n = 0;
                mem[mp] = n;
                break;
            case '[':
                if (src[ip + 1] == '-' && src[ip + 2] == ']') {
                    mem[mp] = 0;
                    ip += 2;
                } else if (mem[mp] == 0)
                    match(1);
                break;
            case ']':
                match(-1);
                ip--;
                break;
            case '*':
                if (xflag) {
                    for (ic = 0; ic < 32; ic += 16) {
                        for (n = 0; n < 16; n++) {
                            fprintf(stderr, "%4X", mem[ic+n]);
                        }
                        fputc('\n', stderr);
                    }
                }
                break;
        }
    }
    free(mem);
    if(cycles) fprintf(stderr, "Done, %d cycles.", nc);
    return EXIT_SUCCESS;
}
