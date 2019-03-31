
/* asm2bfpp - preprocessor for asm2bf
 * currently this application is creating simple readable label system.
 * Example:
 *
 * :label
 *   jmp :label
 *
 * At the moment of writing it's a bit bugged, going to improve soon.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct label {
    char name[16];
    int id;
    
    struct label * next;
};

int find(char * name, struct label * list) {
    if(!strcmp(list->name, name))
        return list->id;
    else if(list->next == NULL)
        return 0;
    else
        find(name, list->next);
}

void preprocess(FILE * stream) {
    int k = ':', c, i;
    char isInQuote = 0;
    char identifier[16];
    struct label * l = malloc(sizeof(struct label)), * base;
    
    base = l;
    
    l->next = NULL;
    l->id = 0;
    strcpy(l->name, "_bfasm_reserved");
    
    while(!feof(stream)) {
        if(k != ':')
            putchar(k);
        c = k;
        k = getc(stream);
        
        if(k == '\"' && c != '.')
            isInQuote = !isInQuote;
        if(isInQuote)
            continue;
        
        if(k == ':') {
            fscanf(stream, "%s", identifier);
            
            if(strlen(identifier) == 0) {
                fprintf(stderr, "Error: Identifier expected.\n");
                exit(1);
            }
            
            i = find(identifier, base);
            
            if(i != 0) {
                printf("%d", i);
            } else {
                l->next = malloc(sizeof(struct label));
                l->next->id = l->id + 1;
                strcpy(l->next->name, identifier);
                l = l->next;
                printf("lbl %d", l->id);
            }
        }
    }
}

int main(void) {
    preprocess(stdin);
}
