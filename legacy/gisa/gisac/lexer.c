
/* Gisa compiler
 *
 * Copyright (C) Kamila Palaiologos Szewczyk, 2017-2019.
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

#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>

int ch = ' ', sym, int_val, line, col;
char id_name[128];

void syntax_error() {
    fprintf(stderr, "[stdin %d:%d] syntax error - token #%d\n", line, col, sym);
	abort();
}

void next_ch() {
    col++;
    ch = getchar();
}

void next_sym() {
    int i = 0;
again:
    switch (ch) {
        case '\n': line++; col = 0;
        case '\r':
        case '\t':
        case ' ': next_ch(); goto again;
        case EOF: sym = EOI; break;
        case '@':
            sym = ASM;
            next_ch();

            if(ch != '"') {
                syntax_error();
            }
        case '"':
            if(sym != ASM)
                sym = STRING;
            
            next_ch();

            while(ch != '"') {
                id_name[i++] = ch;
                next_ch();
            }
            
            id_name[i++] = 0;

            next_ch();
            break;
        case '\'':
            sym = INT;
            next_ch();
            int_val = ch;
            next_ch();
            if(ch != '\'')
                syntax_error();
            next_ch();
            break;
        case '(': next_ch(); sym = LPAR; break;
        case ')': next_ch(); sym = RPAR; break;
        case '{': next_ch(); sym = LBRA; break;
        case '}': next_ch(); sym = RBRA; break;
        case '[': next_ch(); sym = LDER; break;
        case ']': next_ch(); sym = RDER; break;
        case '+': next_ch(); sym = PLUS; break;
        case '*': next_ch(); sym = MUL; break;
        case '/':
            next_ch();
            if(ch == '*') {
                while(1) {
                    next_ch();
                    
                    if(ch == EOF) {
                        fprintf(stderr, "Unterminated comment.");
                        abort();
                    }
                    
                    if(ch == '*') {
                        next_ch();
                        if(ch == '/')
                            break;
                    }
                }
                
                next_ch();
                
                goto again;
            } else
                sym = DIV;
            break;
        case '-': next_ch(); sym = MINUS; break;
        case '<': next_ch(); sym = LESS; break;
        case '=': next_ch(); sym = EQUAL; break;
        default:
            if (ch >= '0' && ch <= '9') {
                int_val = 0; /* missing overflow check */
                while (ch >= '0' && ch <= '9')
                { int_val = int_val*10 + (ch - '0'); next_ch(); }
                sym = INT;
            } else if (ch >= 'a' && ch <= 'z') {
                int i = 0; /* missing overflow check */
                while ((ch >= 'a' && ch <= 'z') || ch == '_')
                { id_name[i++] = ch; next_ch(); }
                id_name[i] = '\0';
                sym = 0;
                while (words[sym] != NULL && strcmp(words[sym], id_name) != 0)
                    sym++;
                if (words[sym] == NULL)
                    sym = ID;
            } else
                syntax_error();
    }
}
