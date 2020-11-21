
/* asm2bf
 *
 * Copyright (C) K. Palaiologos Szewczyk, 2017-2020.
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

/**
 *  This program will double cell range (it's size in bits).
 *  In case of asm2bf, by standard, this program will extend cells
 *  to the 32-bit (4 bytes) size.
 */

#include <stdio.h>
#include <string.h>
#include "config.h"
#include "microcode/bconv.c"

int main(int argc, char * argv[]) {
    int c, rules = 0;
    
    if(argc == 2) {
        if(!strcmp(argv[1], "-a"))
            rules = 1;
        else if(!strcmp(argv[1], "-s"))
            rules = 0;
        else
            fprintf(stderr, "bconv: nrecognized flag: `%s'.\nexpected `-a' (alternative) or `-s' (standard)\n", argv[1]);
    }

    while((c = getchar()) != EOF) {
        char * x = strchr(bconv_commands, c);
        
        if(x == NULL)
            putchar(c);
        else
            printf("%s", bconv_snippets[rules][x - bconv_commands]);
    }
}
