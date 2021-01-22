
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

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "config.h"

#define STYLE_POSTFIX 1
#define STYLE_PREFIX 0

int main(int argc, char * argv[]) {
    int style = STYLE_POSTFIX, c, z;
    unsigned long n = 0;
    
    if(argc == 2) {
        if(!strcmp("-postfix", argv[1]))
            style = STYLE_POSTFIX;
        else if(!strcmp("-prefix", argv[1]))
            style = STYLE_PREFIX;
    }
    
    while((c = getchar()) != EOF) {
        if(style == STYLE_PREFIX) {
            z = getchar();
            if(isdigit(z)) {
                n = 0;
                
                while(isdigit(z)) {
                    n += z - '0';
                    n *= 10;
                    
                    z = getchar();
                }
                
                n /= 10;
                
                ungetc(z, stdin);
                
                while(n --> 0)
                    putchar(c);
            } else {
                putchar(c);
                ungetc(z, stdin);
            }
        } else {
            if(!isdigit(c))
                n = 10;
            else
                n = 0;
            
            while(isdigit(c)) {
                n += c - '0';
                n *= 10;
                
                c = getchar();
            }
            
            n /= 10;
            
            while(n --> 0)
                putchar(c);
        }
    }
}
