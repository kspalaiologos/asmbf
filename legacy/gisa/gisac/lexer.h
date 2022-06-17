
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

#ifndef __LEXER_H_
#define __LEXER_H_

#include <stddef.h>

enum {
    DEFMACRO_SYM, DEFUN_SYM, EXPORTS_SYM, INVOKE_SYM,
	LOOP_SYM, WHILE_SYM, EQ_SYM, NEQ_SYM, OUTB_SYM, INB_SYM, /* Keywords */
	
    LPAR, RPAR, LBRA, RBRA, LDER, RDER, /* Symbols */
	
    PLUS, MINUS, LESS, MUL, DIV, EQUAL, /* Operators */
	
    INT, ASM, ID, STRING, EOI /* Other */
};

static const char * words[] = {
	"defmacro", "defun", "exports", "invoke",
	"do", "while", "eq", "neq", "outb", "inb", NULL
};

extern int ch, sym, int_val, line, col;
extern char id_name[128];

void syntax_error();
void next_ch();
void next_sym();

#endif
