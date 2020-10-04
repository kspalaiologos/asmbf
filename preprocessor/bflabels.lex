
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
#include "../config.h"

struct label_t {
    struct label_t * next;
    char * name;
    int id;
};

struct label_t * main_node;

struct label_t * alloc_node(void) {
    struct label_t * x = malloc(sizeof(struct label_t));
    x->next = NULL;
    x->name = NULL;
    x->id = 0;
    return x;
}

void addlabel(char * text);

void getlabel(char * text) {
    struct label_t * head;
    
    if(*text == '"') {
        printf("%s", text);
        return;
    }
    
    if(main_node == NULL)
        main_node = alloc_node();
    
    text = strchr(text, '%') + 1;
    head = main_node;
    while(head->next != NULL)
        if(head->name && strcmp(text, head->name) == 0) {
            printf("%d", head->id);
            return;
        } else
            head = head->next;
    
    addlabel(text);
}

void addlabel(char * text) {
    static int curid = 1;
    struct label_t * head;
    char * cpy, surpress_label = 0;
    
    cpy = strchr(text, '@') + 1;
    if(cpy != (void *)1)
        text = cpy;
    else surpress_label = 1;
    
    if(main_node == NULL)
        main_node = alloc_node();
    
    head = main_node;
    while(head->next != NULL) {
        if(head->name && strcmp(text, head->name) == 0) {
            printf("lbl %d\n", head->id);
            return;
        } else head = head->next;
    }
    
    head->next = alloc_node();
    head->name = malloc(strlen(text) + 1);
    strcpy(head->name, text);
    printf(!surpress_label ? "lbl %d\n" : "%d", curid);
    head->id = curid++;
}

%}

%option nounput noinput noyywrap nodefault

%%
^[ \t]*\@([A-Za-z_][A-Za-z0-9_]*) { addlabel(yytext); }
(%([A-Za-z_][A-Za-z0-9_]*)|\"[^\"\n]*%([A-Za-z_][A-Za-z0-9_]*)) { getlabel(yytext); }
.|\n { putchar(yytext[0]); }
%%

int main(void) {
    yylex();
}
