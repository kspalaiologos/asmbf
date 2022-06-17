
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

#include "parser.h"
#include "lexer.h"

#include <stdlib.h>
#include <stdio.h>

node * new_node(int size, int type) {
    node * x = malloc(sizeof(node));
    
    if(!x)
        abort();
    
    x->s = NULL;
    x->children = size;
    x->type = type;
    
    if(size != 0) {
        x->o = malloc(sizeof(node) * size);
        if(!x->o)
            abort();
    }
    
    return x;
}

node * tree_contents(int type) {
    node * n = new_node(8, TREE);
    int idx = 0, max = 8;
    
    n->v = type;
    
    while((type == 1 && sym != RPAR) ||
          (type == 2 && sym != RDER) ||
          (type == 0 && sym != RBRA)) {
        if(sym == LPAR) {
            n->o[idx++] = tree(1);
            continue;
        }

        else if(sym == ASM) {
            n->o[idx] = new_node(0, P_ASM);
			
            n->o[idx]->s = malloc(strlen(id_name) + 1);
			
            if(!n->o[idx]->s)
				abort();
			
            strcpy(n->o[idx++]->s, id_name);
        }
        
        else if(sym == STRING) {
            n->o[idx] = new_node(0, P_ASM);
			
            n->o[idx]->s = malloc(strlen(id_name) + 1);
			
            if(!n->o[idx]->s)
				abort();
			
            strcpy(n->o[idx++]->s, id_name);
        }
        
        else if(sym == LBRA) {
            n->o[idx++] = tree(0);
            continue;
        }
        
        else if(sym == LDER) {
            n->o[idx++] = tree(2);
            continue;
        }
        
        else if(sym == ID || sym <= INB_SYM) {
            n->o[idx] = new_node(0, sym == ID ? P_ID : P_KEY);
			
            n->o[idx]->s = malloc(strlen(id_name) + 1);
			
            if(!n->o[idx]->s)
				abort();
			
            strcpy(n->o[idx++]->s, id_name);
        }
        
        else if(sym == INT) {
            n->o[idx] = new_node(0, P_ICONST);
			
            n->o[idx++]->v = int_val;
        }
        
        else if(sym >= PLUS && sym <= EQUAL) {
            n->o[idx] = new_node(0, P_OPERATOR);
			
            n->o[idx++]->v = sym;
        }
        
        else
            syntax_error();
        
        if(idx == max) {
            max += 8;
            n->o = realloc(n->o, sizeof(node) * max);
            if(!n->o)
                abort();
        }
        
        next_sym();
    }
    
    n->children = idx;
    
    return n;
}

node * tree(int type) {
    node * t;
    
    if ((type == 1 && sym != LPAR) ||
        (type == 2 && sym != LDER) ||
        (type == 0 && sym != LBRA))
        syntax_error();
	
        
    next_sym();
    t = tree_contents(type);
    
    if ((type == 1 && sym != RPAR) ||
        (type == 2 && sym != RDER) ||
        (type == 0 && sym != RBRA))
        syntax_error();
	
    next_sym();
    return t;
}

node * program() {
    node * x = new_node(1, PROGRAM);
	
    next_sym();
    
    x->o[0] = tree(sym == LPAR ? 1 : (sym == LBRA ? 0 : 2));
    
    if (sym != EOI)
        syntax_error();
	
    return x;
}

void dump_ast(node * n, int indent) {
    int a, b;
	
    for(a = 0; a < indent; a++)
        fprintf(stderr, "  ");
	
    if (n != NULL) {
        fprintf(stderr, "t%d / c%d: s%s i%d\n", n->type, n->children, n->s, n->v);
		
        for(b = 0; b < n->children; b++)
            dump_ast(n->o[b], indent + 1);
    } else
        fprintf(stderr, "[NULL node]\n");
}
