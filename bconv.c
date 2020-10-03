
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

char * bconv_commands = "><+-[]";
char * bconv_snippets[] = {
    #ifdef DOUBLE_NOCOPY
    /* > */ ">>>>",
    /* < */ "<<<<",
    /* + */ ">+<+[>-]>[->>+<]<<",
    /* - */ ">+<[>-]>[->>-<]<<-",
    /* [ */ ">+<[>-]>[->+>[<-]<[<]>[-<+>]]<-[+<",
    /* ] */ ">+<[>-]>[->+>[<-]<[<]>[-<+>]]<-]<"
    #else
    /* > */ ">>>",
    /* < */ "<<<",
    /* + */ "+>+[<->[->>+<<]]>>[-<<+>>]<<<[->>+<<]",
    /* - */ "+>[<->[->>+<<]]>>[-<<+>>]<<<[->>-<<]>-<",
    /* [ */ ">[<+>[->>+<<]]>>[-<<+>>]<[<<+>>[->+<]]>[-<+>]<<<[[-]",
    /* ] */ ">[<+>[->>+<<]]>>[-<<+>>]<[<<+>>[->+<]]>[-<+>]<<<]"
    #endif
};

int main(void) {
    int c;
    
    while((c = getchar()) != EOF) {
        char * x = strchr(bconv_commands, c);
        
        if(x == NULL)
            putchar(c);
        else
            printf("%s", bconv_snippets[x - bconv_commands]);
    }
}
