
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    DEFMACRO_SYM, DEFUN_SYM, EXPORTS_SYM, INVOKE_SYM, LOOP_SYM, WHILE_SYM, EQ_SYM, NEQ_SYM, OUTB_SYM, INB_SYM, /* Keywords */
    LPAR, RPAR, LBRA, RBRA, LDER, RDER, /* Symbols */
    PLUS, MINUS, LESS, MUL, DIV, EQUAL, /* Operators */
    INT, ASM, ID, STRING, EOI /* Other */
};

char * words[] = {"defmacro", "defun", "exports", "invoke", "do", "while", "eq", "neq", "outb", "inb", NULL};

int ch = ' ', sym, int_val, line;
char id_name[128], * str_val;

void syntax_error() {
    fprintf(stderr, "[stdin:%d] syntax error - token #%d\n", line, sym); 
}

void next_ch() {
    ch = getchar();
}

void next_sym() {
    int i = 0;
again:
    switch (ch) {
        case '\n': line++;
        case '\r':
        case '\t':
        case ' ': next_ch(); goto again;
        case EOF: sym = EOI; break;
        case '@':
            sym = ASM;
            next_ch();

            if(ch != '"') {
                syntax_error();
            }

            next_ch();

            while(ch != '"') {
                id_name[i++] = ch;
                next_ch();
            }
			
			id_name[i++] = 0;

            next_ch();
            break;
        case '(': next_ch(); sym = LPAR; break;
        case ')': next_ch(); sym = RPAR; break;
        case '{': next_ch(); sym = LBRA; break;
        case '}': next_ch(); sym = RBRA; break;
        case '[': next_ch(); sym = LDER; break;
        case ']': next_ch(); sym = RDER; break;
        case '+': next_ch(); sym = PLUS; break;
        case '*': next_ch(); sym = MUL; break;
        case '/':
            next_ch();
            if(ch == '*') {
                while(1) {
                    next_ch();
                    
                    if(ch == EOF) {
                        fprintf(stderr, "Unterminated comment.");
                        abort();
                    }
                    
                    if(ch == '*') {
                        next_ch();
                        if(ch == '/')
                            break;
                    }
                }
                
                next_ch();
                
                goto again;
            } else
                sym = DIV;
            break;
        case '-': next_ch(); sym = MINUS; break;
        case '<': next_ch(); sym = LESS; break;
        case '=': next_ch(); sym = EQUAL; break;
        default:
            if (ch >= '0' && ch <= '9') {
                int_val = 0; /* missing overflow check */
                while (ch >= '0' && ch <= '9')
                { int_val = int_val*10 + (ch - '0'); next_ch(); }
                sym = INT;
            } else if (ch >= 'a' && ch <= 'z') {
                int i = 0; /* missing overflow check */
                while ((ch >= 'a' && ch <= 'z') || ch == '_')
                { id_name[i++] = ch; next_ch(); }
                id_name[i] = '\0';
                sym = 0;
                while (words[sym] != NULL && strcmp(words[sym], id_name) != 0)
                    sym++;
                if (words[sym] == NULL)
                    sym = ID;
            } else
                syntax_error();
    }
}

/* Stage 1 Parser. */

enum {
    PROGRAM, TREE, P_OPERATOR, P_ID, P_KEY, P_ICONST, P_ASM
};

typedef struct node {
    struct node ** o;
    int type, v, children;
    char * s;
} node;

node * tree();

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
            if(!n->o[idx]->s) abort();
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
            if(!n->o[idx]->s) abort();
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
                fprintf(stderr, "Macro not found: %s\n", globs[c1].name);
				abort();
            }
            break;
        case P_ASM:
            printf("%s\n", x->o[c0]->s);
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
    c(program());
    return 0;
} 
