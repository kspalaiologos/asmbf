
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

#define BASE_POINTER 17

char * src;
long lof, ip = -1;

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
    FILE * infile;
    long mp = 0, maxmp = 1023;
    int n;
    unsigned short int * mem;
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
    while (++ip < lof) {
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
                    printf("Access Violation, ip=%d", ip);
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
			case '*': {
				const char nul[16] = {0};
				unsigned short int * ptr = mem + BASE_POINTER, a = 0;
				fprintf(stderr, "--- BEGIN REGISTER DUMP ---\n");
				fprintf(stderr, "G  = %04X\t", mem[0]);
				fprintf(stderr, "IP = %04X\n", mem[1]);
				fprintf(stderr, "T0 = %04X\t", mem[2]);
				fprintf(stderr, "T1 = %04X\n", mem[3]);
				fprintf(stderr, "T2 = %04X\t", mem[4]);
				fprintf(stderr, "R1 = %04X\n", mem[5]);
				fprintf(stderr, "R2 = %04X\t", mem[6]);
				fprintf(stderr, "R3 = %04X\n", mem[7]);
				fprintf(stderr, "R4 = %04X\t", mem[8]);
				fprintf(stderr, "IM = %04X\n", mem[9]);
				fprintf(stderr, "T3 = %04X\t", mem[10]);
				fprintf(stderr, "T4 = %04X\n", mem[11]);
				fprintf(stderr, "T5 = %04X\t", mem[12]);
				fprintf(stderr, "T6 = %04X\n", mem[13]);
				fprintf(stderr, "T7 = %04X\t", mem[14]);
				fprintf(stderr, "A  = %04X\n", mem[15]);
				fprintf(stderr, "--- BEGIN STACK DUMP ---\n");
				fprintf(stderr, "BP: %d\n", BASE_POINTER);
				while(*ptr == 1 && ptr[1] == 0) {
					ptr += 2;
					fprintf(stderr, "[%04X] = *(BP + %04X) = %04X\n", ptr - mem, ptr - mem - BASE_POINTER, *ptr);
					ptr += 2;
				}
				fprintf(stderr, "--- BEGIN MEMORY DUMP ---\n");
				while(1) {
					if(!memcmp(nul, ptr, 16))
						break;
					for(a = 0; a < 16; a++)
						fprintf(stderr, "%04X ", *ptr++);
					ptr -= 16;
					for(a = 0; a < 16; a++, ptr++)
						fprintf(stderr, "%c", isprint(*ptr)?*ptr:'.');
					fprintf(stderr, "\n");
				}
				fprintf(stderr, "Found empty block, quitting.\n");
				fprintf(stderr, "--- END ---");
				getchar();
			}
        }
    }
    return EXIT_SUCCESS;
}
