
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
#include "vector.h"

struct label_t {
    char * name;
    unsigned int seg, off;
};

struct decl_t {
    unsigned char type;
    unsigned int seg, far;
    vector(char) data;
};

enum { DECL_REF, DECL_MISC };

static unsigned int seg, off;

vector(struct label_t) labels;
vector(struct decl_t) declarations;

void append_code(char c) {
    if(declarations && (vector_end(declarations) - 1)->type == DECL_MISC) {
        struct decl_t * d = vector_end(declarations) - 1;
        
        vector_push_back(d->data, c);
    } else {
        struct decl_t decl = {
            .type = DECL_MISC,
            .data = NULL
        };
        
        vector_push_back(decl.data, c);
        vector_push_back(declarations, decl);
    }
}

void append_str(char * s) {
    while(*s)
        append_code(*(s++));
}


struct label_t * find_label(char * name) {
    if(labels == NULL)
        return NULL;
        
    for(struct label_t * it = vector_begin(labels); it != vector_end(labels); ++it)
        if(!strcmp(name, it->name))
            return it;
    
    return NULL;
}

static char * chomp(char * s) {
    while(isspace(*(s++)));
    return --s;
}

void addlabel(char * text) {
    // find the label name.
    text = strchr(text, '&') + 1;
    struct label_t * l = find_label(text);
    
    if(l != NULL) {
        fprintf(stderr, "asm2bf: label redefined: `&%s'\n", text);
        exit(1);
    }
    
    struct label_t q = {
        .name = strdup(text),
        .seg = seg, .off = off
    };
    
    vector_push_back(labels, q);
}

void getlabel(unsigned int far, char * name) {
    if(*name == '"') {
        append_str(name);
        return;
    }
    
    //Extract the label name.
    name = far ? chomp(name + 4) : name + 1;
    
    struct decl_t ref = {
        .type = DECL_REF,
        .data = strdup(name),
        .seg = seg, .far = far  // away in a galaxy long long ago
    };
    
    vector_push_back(declarations, ref);
};

// TODO: Bitwidth bound checking.
void flush_code(void) {
    for(struct decl_t * it = vector_begin(declarations); it != vector_end(declarations); ++it) {
        if(it->type == DECL_MISC) {
            fwrite(it->data, 1, vector_size(it->data), stdout);
        } else {
            struct label_t * l = find_label(it->data);
            
            
            if(!l) {
                fprintf(stderr, "asm2bf: unresolved `*%s'\n", it->data);
                exit(1);
            }
            
            int change = l->seg + l->off;
            
            if(!it->far) {
                if(change < it->seg) {
                    // TODO: Automatic relocations
                    
                    fprintf(stderr, "asm2bf: can't reference `&%s' from %Xh - negative pointer value.\n", it->data, it->seg);
                    exit(1);
                } else printf("%d", change - it->seg);
            } else printf("%d", change);
        }
    }
}

%}

%option nounput noinput noyywrap nodefault

%%
^[ \t]*\&([A-Za-z_][A-Za-z0-9_]*) { addlabel(yytext); }
(\*far[ \t]+([A-Za-z_][A-Za-z0-9_]*))|(\"[^\"\n]*\*far[ \t]+([A-Za-z_][A-Za-z0-9_]*)) { getlabel(1, yytext); }
(\*([A-Za-z_][A-Za-z0-9_]*))|(\"[^\"\n]*\*([A-Za-z_][A-Za-z0-9_]*)) { getlabel(0, yytext); }
^[ \t]*db_ { off++; append_str(yytext); }
^[ \t]*db { off++; append_str(yytext); }
^[ \t]*txt[ \t]*\".*\" { off += strlen(strchr(yytext, '"') + 1) - 1; append_str(yytext); }
^[ \t]*seg[ \t]*([0-9]+) { seg = atoi(strpbrk(yytext, "0123456789")); append_str(yytext); }
^[ \t]*org[ \t]*([0-9]+) { off = atoi(strpbrk(yytext, "0123456789")); append_str(yytext); }
.|\n { append_code(yytext[0]); }
%%

int main(int argc, char * argv[]) {
    yylex();
    flush_code();
    
    if(argc == 2 && !strcmp(argv[1], "-m") && labels != NULL) {
        fprintf(stderr, "Data mappings:\n");
        for(struct label_t * it = vector_begin(labels); it != vector_end(labels); ++it)
            fprintf(stderr, "`%s': 0%Xh:0%Xh (far: 0%Xh)\n", it->name, it->seg, it->off, it->seg + it->off);
    }
}
