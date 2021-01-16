
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

%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../config.h"

char * chomp(char * s) {
    while(isspace(*s))
        s++;
    
    return s;
}

char * allowed_instructions[] = {
    "sto", "amp", "smp", "cst", "cam",
    "csm", "cot", "ots", "spt", NULL
};

void virtual_call(char * text) {
    text = chomp(chomp(text)+6);
    
    /* text now contains the entire linetext. */
    /* now, parse the instruction. */
    
    for(int i = 0; allowed_instructions[i]; i++) {
        if(!strncmp(text, allowed_instructions[i], strlen(allowed_instructions[i]))) {
            text += strlen(allowed_instructions[i]);
            text = chomp(text);
            
            if(!isdigit(*text) && *text != '.') {
                fprintf(stderr, "Error: vxcall not required for `%s'.", text);
                exit(1);
            }
            
            printf("\nmov f3, ");
            
            if(*text == '.') {
                text++;
                printf("%d", *text);
                text++;
            } else {
                while(isdigit(*text)) {
                    putchar(*text);
                    text++;
                }
            }
            
            /* we are at the comma now. */
            
            printf("\n%s f3%s\n", allowed_instructions[i], text);
            return;
        }
    }
    
    fprintf(stderr, "Error: vxcall is not supported: `%s'\n", text);
    exit(1);
}

%}

%option nounput noinput noyywrap nodefault

%%
^[\t ]*vxcall[\t ]+.* { virtual_call(yytext); }
.|\n { putchar(yytext[0]); }
%%

int main(void) {
    yylex();
}
