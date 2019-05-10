
/* asm2bf
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


#define BLK_SIZE 32

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

struct element {
    char * name;
    int id, udecl;
};

struct element * lb;
int lb_capacity, lb_size;

void xerror(char * format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    exit(1);
}

void res(void) {
    if(lb_capacity - 2 > lb_size)
        return;
    
    if(!(lb = realloc(lb, sizeof(struct element) * (lb_capacity += BLK_SIZE))))
        xerror("Error: Out of memory.\n");
}

struct element * byid(char * i) {
    int x;
    for(x = 0; x < lb_size; x++)
        if(lb[x].name != NULL && !strcmp(i, lb[x].name))
            return &lb[x];
    return NULL;
}

void ins(char * s) {
    struct element * e;
    
    s[strlen(s)-1] = 0;
    e = byid(s);
    
    if(!e) {
        lb[lb_size].name = malloc(strlen(s));
        strcpy(lb[lb_size].name, s);
        
        lb[lb_size].id = lb_size, lb[lb_size].udecl = 1;
        
        printf("lbl %d\n", ++lb_size);
    } else
        if((*e).udecl == 1)
            xerror("Error: %s redefined.", s);
        else
            printf("lbl %d\n", (*e).id + 1), (*e).udecl = 1;
}

void inc(char * s) {
    FILE * f;
    
    s[strlen(s)-1] = 0;
    
    f = fopen(s, "r");
    if(!f) {
        xerror("Error: Couldn't open %s!", s);
    }
    
    start(f);
}

void get(char * s, int l) {
    struct element * e;
    
    s[l] = 0;
    e = byid(s);
    
    if(!e) {
        lb[lb_size].name = malloc(strlen(s));
        strcpy(lb[lb_size].name, s);
        
        printf("%d", (lb[lb_size].id = lb_size++));
    } else
        printf("%d", (*e).id + 1);
}

void start(FILE * f) {
    char * s = NULL, * c, e, v, n2;
    
    while((getline(&s, &n2, f)) != -1)
        if(!strncmp(s, "txt \"", 5)) {
            c = s + 4, e = 0;
            
            printf("txt \"");
            
            while(*++c) {
                if(e) {
                    switch(*c) {
                        case '0':  v = 0x00; break;
                        case 'n':  v = 0x0A; break;
                        case 'r':  v = 0x0D; break;
                        case 'a':  v = 0x07; break;
                        case 'b':  v = 0x08; break;
                        case 'e':  v = 0x1B; break;
                        case 'f':  v = 0x0C; break;
                        case 't':  v = 0x09; break;
                        case '\\': v = 0x5C; break;
                        case '"':  v = 0x22; break;
                        default: xerror("Error: Invalid escape sequence: \\%c\n", c);
                    }
                    
                    printf("\"\ndb_ %d\ntxt \"", v);
                    
                    e = 0;
                    continue;
                } else if(*c == '\\')
                    e = 1;
                else
                    putchar(*c);
            }
        } else {
            char * c = s, * g, * ss, * g2, wf = 0;
            
            if(*c == '#') {
                c++;
                inc(c);
                wf = 1;
            }
            
            if(*c != ':') {
                g = strchr(c, '[');
                
                if(g && g[-1] != '.') {
                    fwrite(c, 1, g - c, stdout);
                    
                    ss = g++, g2 = strchr(g, ']');
                    
                    if(g2 && g2[-1] != '.')
                        res(), get(g, g2-g), fwrite(g2 + 1, 1, strlen(g2 + 1), stdout);
                } else if(!wf)
                    printf("%s", s);
            } else
                res(), ins(++c);
        }
}

int main(void) {
    start(stdin);
}
