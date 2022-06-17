
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

#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"

/* Optimizer, kinda */

node * opt(node * n) {
	return n;
}

/* Code generator. */

#define CODE_OFFSET 4

enum {
    TYPE_VARIABLE, TYPE_MACRO, TYPE_FUNC
};

typedef struct symbol {
    int type, id, arity;
    char * name;
    node * macro;
} symbol;

int lid = 1;
int doStack[16], doSP = 0, lastGlob = 0;
symbol globs[256];

void c(node * x);

int gentree(node * x, int c0) {
    int c1, c2;
    switch(x->o[c0]->type) {
        case P_ICONST:
            printf("psh %d\n", x->o[c0]->v);
            break;
        case P_OPERATOR:
            // Todo: Binary operator optimization.
            if(x->o[c0]->v == PLUS) // Binary plus
                printf("; Binary addition\npop r2\npop r3\nadd r2, r3\npsh r2\n");
            if(x->o[c0]->v == MUL) // Binary multiplication
                printf("; Binary multiplication\npop r2\npop r3\nmul r2, r3\npsh r2\n");
            if(x->o[c0]->v == LESS) // Binary less-than operator
                printf("; Binary less-than operator\npop r2\npop r3\nlt_ r2, r3\npsh r2\n");
            if(x->o[c0]->v == MINUS && x->children == 2) // Unary -
                printf("; Unary minus\npop r2\nneg r2\npsh r2\n");
            if(x->o[c0]->v == MINUS && x->children > 2) // Binary -
                printf("; Binary minus\npop r2\npop r3\nsub r2, r3\npsh r2\n");
            if(x->o[c0]->v == EQUAL) // =
                printf("; Binary set\npop r2\nadd r2, %d\npop r3\nsto r2, r3\n", CODE_OFFSET);
            break;
        case P_KEY:
            if(!strcmp("outb", x->o[c0]->s))
                printf("; outb\npop r2\nout r2\n");
            else if(!strcmp("inb", x->o[c0]->s))
                printf("; inb\nin_ r2\npsh r2\n");
            else if(!strcmp("eq", x->o[c0]->s))
                printf("; eq\npop r2\npop r3\neq_ r2, r3\npsh r2\n");
            else if(!strcmp("neq", x->o[c0]->s))
                printf("; neq\npop r2\npop r3\nne_ r2, r3\npsh r2\n");
            else if(!strcmp("do", x->o[c0]->s))
                doStack[doSP++] = lid, printf("; do\nlbl %d\n", lid++);
            else if(!strcmp("while", x->o[c0]->s))
                printf("; while\npop r2\njnz r2, %d\n", doStack[--doSP]);
            break;
        case TREE:
            c(x->o[c0]);
            break;
        case P_ID:
            for(c1 = 0, c2 = 0; c1 < lastGlob; c1++) {
                if(globs[c1].name != NULL && !strcmp(globs[c1].name, x->o[c0]->s)) {
                    if(globs[c1].macro == NULL) {
                        fprintf(stderr, "%s is not a macro", globs[c1].name);
                        abort();
                    } else
                        c2++, c(globs[c1].macro);
                }
            }
            
            if(!c2) {
                fprintf(stderr, "Macro not found: %s\n", x->o[c0]->s);
                abort();
            }
            break;
        case P_ASM:
            printf("%s\n", x->o[c0]->s);
            break;
        case STRING:
            printf("txt \"%s\"\ndb_ 0\n", x->o[c0]->s);
            break;
        default:
            fprintf(stderr, "Unexpected %d.\n", x->o[c0]->type);
            abort();
    }
    
    return 0;
}

void c(node * x) {
    int c0, c1, c2, c3;
    
    if(x->type == PROGRAM)
        dump_ast(x, 0);
    
    switch(x->type) {
        case PROGRAM:
            printf("org 0\nstk 16\n");
            c(x->o[0]);
            break;
        
        case TREE:
            c3 = 0;
            
            if(x->children >= 1) {
                node * n = x->o[x->v != 0 ? (x->children - 1) : 0];
                if(n->type == P_KEY) {
                    if(!strcmp("defmacro", n->s)) {
                        if(x->children != 3) {
                            fprintf(stderr, "Three macro children expected.");
                            abort();
                        }

                        if(x->o[x->v != 0 ? (x->children - 2) : 1]->type != P_ID) {
                            fprintf(stderr, "Second macro child is not an identifier");
                            abort();
                        }

                        if(x->o[x->v != 0 ? (x->children - 3) : 2]->type != TREE) {
                            fprintf(stderr, "Third macro child is not a tree");
                            abort();
                        }

                        globs[lastGlob].type = TYPE_MACRO;
                        globs[lastGlob].macro = x->o[x->v != 0 ? (x->children - 3) : 1];
                        globs[lastGlob++].name = x->o[x->v != 0 ? (x->children - 2) : 2]->s;
                        break;
                    } else if(!strcmp("defun", n->s)) {
                        node * chnode;
                        int nparam;
                        
                        if(x->children != 3) {
                            fprintf(stderr, "Three defun children expected.");
                            abort();
                        }
                        
                        if(x->o[x->v != 0 ? 2 : 1]->type != P_ID) {
                            fprintf(stderr, "Second defun child is not an identifier");
                            abort();
                        }
                        
                        if(x->o[x->v != 0 ? 1 : 2]->type != TREE) {
                            fprintf(stderr, "Third macro child is not a tree");
                            abort();
                        }
                        
                        globs[lastGlob].type = TYPE_FUNC;
                        globs[lastGlob].macro = NULL;
                        globs[lastGlob].id = lid++;
                        globs[lastGlob++].name = x->o[x->v != 0 ? 2 : 1]->s;
                        
                        printf("; function decl\nlbl %d\n", globs[lastGlob-1].id);
                        c(x->o[x->v != 0 ? 1 : 2]);
                        printf("pop r1\nret\n");
                        
                        break;
                    } else if(!strcmp("invoke", n->s)) {
                        int rad = lid++;
                        
                        if(x->children != 3) {
                            fprintf(stderr, "Three invoke children expected.");
                            abort();
                        }
                        
                        if(x->o[x->v != 0 ? 1 : 1]->type != P_ID) {
                            fprintf(stderr, "Second invoke child is not a P_ID.");
                            abort();
                        }
                        
                        if(x->o[x->v != 0 ? 0 : 1]->type != TREE) {
                            fprintf(stderr, "Third invoke child is not a tree.");
                            abort();
                        }
                        
                        printf("; save return address\npsh %d\n", rad);
                        
                        printf("; evaluate parameter tree\n");
                        c(x->o[x->v != 0 ? 0 : 1]);
                        
                        for(c1 = 0, c2 = 0; c1 < lastGlob; c1++) {
                            if(globs[c1].name != NULL && !strcmp(globs[c1].name, x->o[x->v != 0 ? 1 : 1]->s)) {
                                if(globs[c1].macro != NULL) {
                                    fprintf(stderr, "%s is a macro", globs[c1].name);
                                    abort();
                                } else
                                    c2++, c0 = globs[c1].id;
                            }
                        }
                        
                        if(!c2) {
                            fprintf(stderr, "Function not found: %s\n", x->o[x->v != 0 ? 1 : 1]->s);
                            abort();
                        }
                        
                        printf("; invoke\njmp %d\nlbl %d\npsh r1\n", c0, rad);
                        
                        break;
                    } else if(!strcmp("exports", n->s)) {
                        if(x->children < 2) {
                            fprintf(stderr, "At least two `exports` children expected.");
                            abort();
                        }
                        
                        c3 = lid++;
                        
                        
                        printf("; exports\njmp %d\n", c3);
                    }
                }
            }

            if(x->v == 1) {
                c0 = x->children;
                while(--c0 >= 0)
                    if(gentree(x, c0))
                        break;
            } else if(x->v == 0) {
                c0 = 0;
                while(c0 < x->children)
                    if(gentree(x, c0++))
                        break;
            } else {
                c0 = x->children;
                while(--c0 >= 0)
                    if(gentree(x, c0))
                        break;
                printf("; deref\npop r2\nadd r2, %d\nrcl r3, r2\npsh r3\n", CODE_OFFSET);
            }
            
            if(c3) {
                printf("; export end block\nlbl %d\n", c3);
            }
            
            break;
    }
}

/* Stub */

int main() {
    c(opt(program()));
    return 0;
} 

