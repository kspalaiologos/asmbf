
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
#include <assert.h>
#include <ctype.h>
#include "../config.h"

#if __STDC_VERSION__ >= 201112L
    #include <stdnoreturn.h>
    #define NORETURN noreturn
#else
    #define NORETURN
#endif

struct def_t {
    char * find, * replace;
    struct def_t * next;
};

static int lineno = 0;

static struct def_t * ctx, * ctx_ptr;

static struct def_t * new_def(void) {
    struct def_t * ret = malloc(sizeof(struct def_t));
    assert(ret);
    return ret;
}

static void pop_def(char * text) {
    struct def_t * p = ctx;
    
    if(*text == '"') {
        printf("%s", text);
        return;
    }
    
    while(p->next && p->find && p->replace) {
        if(!strcmp(text, p->find)) {
            printf("%s", p->replace);
            return;
        }
        
        p = p->next;
    }

    printf("%s", text);
}

static void push_def(char * text) {
    /* Skip the whitespace. */
    while(isspace((unsigned char)*text))
        text++;
    
    /* Skip the questionmark */
    text++;
    
    char * find = text, * replace = strchr(text, '=') + 1;
    unsigned find_len = replace - find - 1, replace_len = strlen(replace);
    
    replace[-1] = 0;

    ctx_ptr->find = malloc(find_len + 1);
    memcpy(ctx_ptr->find, find, find_len + 1);
    
    ctx_ptr->replace = malloc(replace_len + 1);
    memcpy(ctx_ptr->replace, replace, replace_len + 1);

    char * ptr = ctx_ptr->replace;
    while(*ptr) {
        if(*ptr == '/')
            *ptr = '\n';
        ptr++;
    }
    
    ctx_ptr->next = new_def();
    ctx_ptr->next->find = ctx_ptr->next->replace = NULL;
    ctx_ptr = ctx_ptr->next;
}

NORETURN static void syntax_error(char * str) {
    fprintf(stderr, "[constpp/asm2bf] line %d: Syntax error: `%s'\n", lineno + 1, str);
    exit(1);
}

%}

%option nounput noinput noyywrap nodefault

%%
^[ \t]*\?([A-Za-z_][A-Za-z0-9_]*)\=([A-Za-z_][A-Za-z0-9_/]*) { push_def(yytext); }
(([A-Za-z_][A-Za-z0-9_]*)|\"[^\"\n]*([A-Za-z_][A-Za-z0-9_/]*)) { pop_def(yytext); }
^[ \t]*\?.* { syntax_error(yytext); }
\n { lineno++; putchar('\n'); }
. { putchar(yytext[0]); }
%%

int main(void) {
    ctx = ctx_ptr = new_def();
    
    yylex();
}
