
/* bfasmapi.c
 *
 * Copyright (C) Krzysztof Palaiologos Szewczyk, Maciej Maviek Stanka, 2020.
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

int __bfasm(char* code);
int bf_set_input_func(int (*ptr)());
int bf_set_output_func(int (*ptr)(int));
int bf_get_reg(int reg);
int bf_set_reg(int reg, int val);

#ifndef BF_STK
#define BF_STK "8"
#endif
#ifndef BF_ORG
#define BF_ORG "0"
#endif
#ifndef BF_SEG
#define BF_SEG "0"
#endif
#define bfasm(code) \
  __bfasm("stk " BF_STK "\norg " BF_ORG "\nseg " BF_SEG "\n" code)
