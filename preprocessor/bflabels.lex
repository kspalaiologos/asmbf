
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
    vector(char *) names;
    unsigned int declared, refs;
};

struct decl_t {
    char type;
    vector(char) data;
};

enum {
    DECL_MISC,
    DECL_REF,
    DECL_AT
};

vector(struct label_t) labels;
vector(struct decl_t) declarations;

void push_decl(unsigned char type, vector(char) data) {
    struct decl_t d = {
        .type = type,
        .data = data
    };
    
    vector_push_back(declarations, d);
}

void push_label(char * name, unsigned int declared) {
    struct label_t l = {
        .names = NULL,
        .declared = declared,
        .refs = 1
    };
    
    vector_push_back(l.names, name);
    vector_push_back(labels, l);
}

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

struct label_t * find_label(char * name) {
    if(labels == NULL)
        return NULL;
    
    for(struct label_t * it = vector_begin(labels); it != vector_end(labels); ++it) {
        for(char ** s = vector_begin(it->names); s != vector_end(it->names); ++s) {
            if(!strcmp(name, *s)) {
                return it;
            }
        }
    }
    
    return NULL;
}

int compare_labels(const void * v1, const void * v2) {
    const struct label_t * l1 = v1, * l2 = v2;
    return l2->refs - l1->refs;
}

void sort_labels(void) {
    qsort(labels, vector_size(labels), sizeof(struct label_t), compare_labels);
}

struct decl_t * prec_label(void) {
    struct decl_t * top = vector_end(declarations) - 1;
    
    while(1) {
        if(top->type == DECL_AT)
            return top;
        else if(top->type == DECL_REF)
            return NULL;
        else if(top->type == DECL_MISC) {
            char * s = top->data;
            int c = 0;
            
            while(s[c] != ';' && c != vector_size(top->data) - 1)
                if(!isspace(s[c]))
                    return NULL;
                else
                    c++;
            
            if(top == vector_begin(declarations))
                return NULL;
            
            --top;
        }
    }
}

void getlabel(char * text) {
    // Extract the label name by skipping the leading `%'.
    text++;
    
    // if the vector has it, enqueue.
    struct label_t * lab = find_label(text);
    
    if(!lab)
        push_label(strdup(text), 0);
    else
        lab->refs++;
    
    push_decl(DECL_REF, strdup(text));
}

void addlabel(char * text) {
    //Trim the leading whitespace including @.
    text = strchr(text, '@') + 1;
    
    struct label_t * lab = find_label(text);
    if(lab) {
        if(lab->declared) {
            fprintf(stderr, "asm2bf: label redeclared: %s\n", text);
            exit(1);
        }
        
        // TODO: More label fusing optimization?
        struct decl_t * pr;
        if(declarations && (pr = prec_label())) {
            char * prev_name = pr->data;
            struct label_t * l = find_label(prev_name);
            l->refs = lab->refs + 1;
            l->declared = 1;
            vector_push_back(l->names, strdup(text));
        } else { lab->declared = 1; lab->refs++; push_decl(DECL_AT, strdup(text)); }
    } else {
        char * copy = strdup(text);
        struct decl_t * pr;
        
        if(declarations && (pr = prec_label())) {
            char * prev_name = pr->data;
            struct label_t * l = find_label(prev_name);
            l->declared = 1;
            vector_push_back(l->names, copy);
        } else {
            push_label(copy, 1);
            push_decl(DECL_AT, copy);
        }
    }
}
        
void flush_decls(void) {
    sort_labels();
    
    for(struct decl_t * it = vector_begin(declarations); it != vector_end(declarations); ++it) {
        if(it->type == DECL_MISC) {
            fwrite(it->data, 1, vector_size(it->data), stdout);
        } else {
            struct label_t * l = find_label(it->data);

            if(it->type == DECL_AT && l->refs == 1) {
                // ignore this label.
                continue;
            }

            printf((it->type == DECL_AT ? "lbl %d\n" : "%d"), (l - vector_begin(labels)) + 1);
            
            if(it->type == DECL_REF && !l->declared) {
                fprintf(stderr, "asm2bf: unresolved `%s'\n", it->data);
                exit(1);
            }
        }
    }
}

void append_code_s(char * s) {
    while(*s) {
        append_code(*s);
        s++;
    }
}

%}

%option nounput noinput noyywrap nodefault

%%
^[ \t]*\@([A-Za-z_][A-Za-z0-9_]*) { addlabel(yytext); }
\"[^\"\n]*\" { append_code_s(yytext); }
\.. { append_code_s(yytext); }
;.*\n { append_code_s(yytext); }
%([A-Za-z_][A-Za-z0-9_]*) { getlabel(yytext); }
.|\n { append_code(yytext[0]); }
%%

int main(int argc, char * argv[]) {
    yylex();
    flush_decls();
    
    if(argc == 2 && !strcmp(argv[1], "-m") && labels) {
        int idx = 1;
        fprintf(stderr, "Mappings:\n");
        for(struct label_t * it = vector_begin(labels); it != vector_end(labels); ++it) {
            fprintf(stderr, "%d: ", idx++);
            
            for(char ** s = vector_begin(it->names); s != vector_end(it->names); ++s) {
                fprintf(stderr, "`%s' ", *s);
            }
            
            fprintf(stderr, "\n");
        }
    }
}
