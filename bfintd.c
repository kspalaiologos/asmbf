
/* BFI version intended to be used as debugger for bfasm. */
/* It may have some weird glitches because I sticked it up in couple of minutes. */

/* asm2bf
 *
 * Copyright (C) Kamila Palaiologos Szewczyk, 2019.
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
#include <string.h>

FILE * infile;
long mp = 0, maxmp = 1023;
int n, c;
unsigned short int * mem;
char * src;
long lof, ip = -1, gip = 0;

int step() {
    ++ip;
    switch (src[ip]) {
        case '>':
            if (mp >= maxmp) {
                mem = realloc(mem, (maxmp + 1024) * sizeof(unsigned short int));
                if (mem == NULL) {
                    puts("Out of memory");
                    return EXIT_FAILURE;
                }
                for (n = 1; n <= 1024; n++)
                    mem[maxmp + n] = 0;
                maxmp += 1024;
            }
            mp++;
            break;
        case '<':
            if (mp <= 0) {
                puts("Access Violation");
                return EXIT_FAILURE;
            }
            mp--;
            break;
        case '+':
            mem[mp]++;
            break;
        case '-':
            mem[mp]--;
            break;
        case '.':
            putchar(mem[mp]);
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
        case '!':
            return ++gip,0;
    }
    if(ip < lof)
        return step();
    else return 0;
}

void match(int dir) {
    int level = 1;
    char ipvalue;
    do {
        ip += dir;
        if (ip < 0 || ip >= lof) {
            puts("Mismatched brackets");
            exit(EXIT_FAILURE);
        }
        ipvalue = src[ip];
        if (ipvalue == 91)
            level += dir;
        else if (ipvalue == 93)
            level -= dir;
    } while (level > 0);
}

int main(int argc, char * argv[]) {
    puts("bfdbg v1.0\n");
    if (argc != 2 || argv[1][0] == '-' || argv[1][0] == '/') {
        puts("Usage: bfi src.b");
        return EXIT_FAILURE;
    }
    infile = fopen(argv[1], "rb");
    if (infile == NULL) {
        puts("Error opening input file");
        return EXIT_FAILURE;
    }
    if (fseek(infile, 0L, SEEK_END) != 0) {
        puts("Error determining length of input file");
        return EXIT_FAILURE;
    }
    lof = ftell(infile);
    if (lof == -1) {
        puts("Error determining length of input file");
        return EXIT_FAILURE;
    }
    if (fseek(infile, 0L, SEEK_SET) != 0) {
        puts("Error determining length of input file");
        return EXIT_FAILURE;
    }
    if (lof == 0)
        return EXIT_SUCCESS;
    src = (char *) calloc(lof + 2, sizeof(char));
    if (src == NULL) {
        puts("Program too big to fit in memory");
        return EXIT_FAILURE;
    }
    if (fread(src, sizeof(char), lof, infile) < (unsigned) lof) {
        puts("Error reading input file");
        return EXIT_FAILURE;
    }
    if (fclose(infile) == -1) {
        puts("Error closing input file");
        return EXIT_FAILURE;
    }
    mem = calloc(1024, sizeof(unsigned short int));
    if (mem == NULL) {
        puts("Out of memory");
        return EXIT_FAILURE;
    }
    while (ip < lof) {
        if(c != 10)
            printf("\n[dbg] ip=%d > ", gip);
        c = getchar();
        if (c == 's') {
            step();
        } else if(c == 'n') {
            int n, i;
            scanf("%d",&n);
            for(i = 0; i < n; i++)
                step();
        } else if(c == 'd') {
            printf("G = %d\n", mem[0]);
            printf("IP = %d\n", mem[1]);
            printf("T0 = %d\n", mem[2]);
            printf("T1 = %d\n", mem[3]);
            printf("T2 = %d\n", mem[4]);
            printf("R1 = %d\n", mem[5]);
            printf("R2 = %d\n", mem[6]);
            printf("R3 = %d\n", mem[7]);
            printf("R4 = %d\n", mem[8]);
            printf("IM = %d\n", mem[9]);
            printf("T3 = %d\n", mem[10]);
            printf("T4 = %d\n", mem[11]);
            printf("T5 = %d\n", mem[12]);
            printf("T6 = %d\n", mem[13]);
            printf("T7 = %d\n", mem[14]);
            printf("A = %d\n", mem[16]);
        } else if(c == 'p') {
            int n, i;
            scanf("%d",&n);
            printf("mem[%d]=%d",n,mem[n]);
        } else if(c == 'c') {
            for(; ip < lof;)
                step();
        } else if(c == 'h') {
            printf("Commands:\n");
            printf(" h: display this text\n");
            printf(" c: continue execution until the end\n");
            printf(" p[x]: peek memory cell #x\n");
            printf(" d: show contents of all registers\n");
            printf(" n[x]: run #x instructions\n");
            printf(" s: step once\n");
        } else if(c != 10) {
            printf("Please use 'h' for help.");
        }
    }
    return EXIT_SUCCESS;
}
