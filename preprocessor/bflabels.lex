
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


struct code_label_t {
    vector(char *) names;
    unsigned int declared, refs;
};

struct decl_t {
    unsigned char type, is_code;
    unsigned long seg, far;
    vector(char) data;
};

enum {
    DECL_MISC,
    DECL_REF,
    DECL_AT
};

vector(struct code_label_t) code_labels;
vector(struct decl_t) declarations;

void push_code_decl(unsigned char type, vector(char) data) {
    struct decl_t d = {
        .type = type,
        .data = data,
        .is_code = 1
    };
    
    vector_push_back(declarations, d);
}

void push_code_label(char * name, unsigned int declared) {
    struct code_label_t l = {
        .names = NULL,
        .declared = declared,
        .refs = 1
    };
    
    vector_push_back(l.names, name);
    vector_push_back(code_labels, l);
}

void append_code(char c) {
    if(declarations && (vector_end(declarations) - 1)->type == DECL_MISC) {
        struct decl_t * d = vector_end(declarations) - 1;
        
        vector_push_back(d->data, c);
    } else {
        struct decl_t decl = {
            .type = DECL_MISC,
            .data = NULL,
            .is_code = 1
        };
        
        vector_push_back(decl.data, c);
        vector_push_back(declarations, decl);
    }
}

struct code_label_t * find_label(char * name) {
    if(code_labels == NULL)
        return NULL;
    
    for(struct code_label_t * it = vector_begin(code_labels); it != vector_end(code_labels); ++it) {
        for(char ** s = vector_begin(it->names); s != vector_end(it->names); ++s) {
            if(!strcmp(name, *s)) {
                return it;
            }
        }
    }
    
    return NULL;
}

int compare_labels(const void * v1, const void * v2) {
    const struct code_label_t * l1 = v1, * l2 = v2;
    return l2->refs - l1->refs;
}

void sort_labels(void) {
    qsort(code_labels, vector_size(code_labels), sizeof(struct code_label_t), compare_labels);
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

            while(s[c] == ';')
                while(s[c] != '\n' && c != vector_size(top->data))
                    c++;
            
            while(isspace(s[c]) && c != vector_size(top->data))
                c++;
            
            if(c != vector_size(top->data) || top == vector_begin(declarations))
                return NULL;
            
            --top;
        }
    }
}

void getlabel_c(char * text) {
    // Extract the label name by skipping the leading `%'.
    text++;
    
    // if the vector has it, enqueue.
    struct code_label_t * lab = find_label(text);
    
    if(!lab)
        push_code_label(strdup(text), 0);
    else
        lab->refs++;
    
    push_code_decl(DECL_REF, strdup(text));
}

void addlabel_c(char * text) {
    //Trim the leading whitespace including @.
    text = strchr(text, '@') + 1;
    
    struct code_label_t * lab = find_label(text);
    if(lab) {
        if(lab->declared) {
            fprintf(stderr, "asm2bf: label redeclared: %s\n", text);
            exit(1);
        }
        
        // TODO: More label fusing optimization?
        struct decl_t * pr;
        if(declarations && (pr = prec_label())) {
            char * prev_name = pr->data;
            struct code_label_t * l = find_label(prev_name);
            l->refs = lab->refs + 1;
            l->declared = 1;
            vector_push_back(l->names, strdup(text));
        } else { lab->declared = 1; lab->refs++; push_code_decl(DECL_AT, strdup(text)); }
    } else {
        char * copy = strdup(text);
        struct decl_t * pr;
        
        if(declarations && (pr = prec_label())) {
            char * prev_name = pr->data;
            struct code_label_t * l = find_label(prev_name);
            l->declared = 1;
            vector_push_back(l->names, copy);
        } else {
            push_code_label(copy, 1);
            push_code_decl(DECL_AT, copy);
        }
    }
}

void append_code_s(char * s) {
    while(*s) {
        append_code(*s);
        s++;
    }
}

/* data part */

struct label_t {
    char * name;
    unsigned long seg, off;
};

static unsigned long long seg, off, bitwidth = 16;

vector(struct label_t) labels;

struct label_t * find_data_label(char * name) {
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
    struct label_t * l = find_data_label(text);
    
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

void getlabel(unsigned long far, char * name) {
    if(*name == '"') {
        append_code_s(name);
        return;
    }
    
    //Extract the label name.
    name = far ? chomp(name + 4) : name + 1;
    
    struct decl_t ref = {
        .type = DECL_REF, .is_code = 0,
        .data = strdup(name),
        .seg = seg, .far = far  // away in a galaxy long long ago
    };
    
    vector_push_back(declarations, ref);
};

void emit_addr(char * labname, unsigned long address) {
    if(address >> 1 > (2 << (bitwidth - 2))) {
        fprintf(stderr, "asm2bf: reference to &%s (%lX) overflows the current bitwidth, can address %lX maximum.", labname, address, (2 << (bitwidth - 1)));
        exit(1);
    }

    printf("%u", address);
}

// TODO: Bitwidth bound checking.
void flush_code(void) {
    sort_labels();

    for(struct decl_t * it = vector_begin(declarations); it != vector_end(declarations); ++it) {
        if(!it->is_code) {
            if(it->type == DECL_MISC) {
                fwrite(it->data, 1, vector_size(it->data), stdout);
            } else {
                struct label_t * l = find_data_label(it->data);
                
                if(!l) {
                    fprintf(stderr, "asm2bf: unresolved `*%s'\n", it->data);
                    exit(1);
                }
                
                unsigned long change = l->seg + l->off;
                
                if(!it->far) {
                    if(change < it->seg) {
                        fprintf(stderr, "asm2bf: can't reference `&%s' from %Xh - negative pointer value.\n", it->data, it->seg);
                        exit(1);
                    } else emit_addr(it->data, change - it->seg);
                } else emit_addr(it->data, change);
            }
        } else {
            if(it->type == DECL_MISC) {
                fwrite(it->data, 1, vector_size(it->data), stdout);
            } else {
                struct code_label_t * l = find_label(it->data);

                if(it->type == DECL_AT && l->refs == 1) {
                    // ignore this label.
                    continue;
                }

                printf((it->type == DECL_AT ? "lbl %d\n" : "%d"), (l - vector_begin(code_labels)) + 1);
                
                if(it->type == DECL_REF && !l->declared) {
                    fprintf(stderr, "asm2bf: unresolved `%s'\n", it->data);
                    exit(1);
                }
            }
        }
    }
}

void bits(char * text) {
    // Skip the `[bits ' part.
    text = chomp(text + 6);

    // atoi skips the `]' automagically.
    bitwidth = atoi(text);

    printf("bts %u\n", bitwidth);
}
%}

%option nounput noinput noyywrap nodefault

%%
^[ \t]*\@([A-Za-z_][A-Za-z0-9_]*) { addlabel_c(yytext); }
\"[^\"\n]*\" { append_code_s(yytext); }
\.. { append_code_s(yytext); }
;.*\n { append_code_s(yytext); }
%([A-Za-z_][A-Za-z0-9_]*) { getlabel_c(yytext); }

^[\ \t]*\[[bB][iI][tT][sS]\ [0-9]+\]   { bits(yytext); }
^[ \t]*\&([A-Za-z_][A-Za-z0-9_]*) { addlabel(yytext); }
(\*[Ff][Aa][Rr][ \t]+([A-Za-z_][A-Za-z0-9_]*)) { getlabel(1, yytext); }
(\*([A-Za-z_][A-Za-z0-9_]*)) { getlabel(0, yytext); }
^[ \t]*[Dd][Bb]_ { off++; append_code_s(yytext); }
^[ \t]*[Dd][Bb] { off++; append_code_s(yytext); }
^[ \t]*[Tt][Xx][Tt][ \t]*\".*\" { off += strlen(strchr(yytext, '"') + 1) - 1; append_code_s(yytext); }
^[ \t]*[Ss][Ee][Gg][ \t]*([0-9]+) { seg = atoi(strpbrk(yytext, "0123456789")); append_code_s(yytext); }
^[ \t]*[Oo][Rr][Gg][ \t]*([0-9]+) { off = atoi(strpbrk(yytext, "0123456789")); append_code_s(yytext); }

.|\n { append_code(yytext[0]); }
%%

int main(int argc, char * argv[]) {
    yylex();
    flush_code();

    if(argc == 2 && !strcmp(argv[1], "-m") && code_labels) {
        int idx = 1;
        fprintf(stderr, "Mappings:\n");
        for(struct code_label_t * it = vector_begin(code_labels); it != vector_end(code_labels); ++it) {
            fprintf(stderr, "%d: ", idx++);
            
            for(char ** s = vector_begin(it->names); s != vector_end(it->names); ++s) {
                fprintf(stderr, "`%s' ", *s);
            }
            
            fprintf(stderr, "\n");
        }
    }

    if(argc == 2 && !strcmp(argv[1], "-m") && labels != NULL) {
        fprintf(stderr, "Data mappings:\n");
        for(struct label_t * it = vector_begin(labels); it != vector_end(labels); ++it)
            fprintf(stderr, "`%s': 0%Xh:0%Xh (far: 0%Xh)\n", it->name, it->seg, it->off, it->seg + it->off);
    }
}
