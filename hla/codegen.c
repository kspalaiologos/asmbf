
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "modq.h"
#include "node.h"
#include "emitter.h"
#include "symtab.h"

struct {
    unsigned int stack_size;

    unsigned int origin;
    unsigned int segment;

    unsigned char in_preserve;

    unsigned char moduleDeclared;
    unsigned char programDeclared;

    unsigned char functionsDeclared;
    unsigned char startDeclared;
} context;

#define INDENT for(i = 0; i < indent; i++) putchar(' ')
static void dump_tree(struct node * n) {
    static int indent = 0;
    int i;
    
    if(!n)
        return;
    
    INDENT; printf("[%d @ %d:%d]\n", n->type, n->line, n->col);
    if(n->value)    { INDENT; printf("value: %s\n", n->value); }
    if(n->child)    { INDENT; indent += 1; printf("->\n"); dump_tree(n->child); indent -= 1; }
    if(n->brother)  { INDENT; printf("----\n"); dump_tree(n->brother); }
}
#undef INDENT

static void dispatch_program(struct node * n) {
    switch(n->type) {
        case Address: case ProgramOrigin: {
            struct node * tmp = n->child;
            
            while(tmp) {
                dispatch_program(tmp);
                tmp = tmp->brother;
            }

            break;
        }

        case StackSize: {
            context.stack_size = atoi(n->value);
            emit_data(InternalLinkage, NULL, Stk, context.stack_size);
            break;
        }

        case Segment: {
            context.segment = atoi(n->value);
            emit_data(InternalLinkage, NULL, Seg, context.segment);
            break;
        }
        
        case Offset: {
            context.origin = atoi(n->value);
            emit_data(InternalLinkage, NULL, Off, context.origin);
            break;
        }

        default: {
            fprintf(stderr, "[%d:%d] ICE: unsupported declaration inside the program scope.\n", n->line, n->col);
            exit(1);
        }
    }
}

static void validate_register(int reg, struct node * n) {
    if(reg <= 0 && reg >= 7) {
        fprintf(stderr, "[%d:%d] Incorrect register specification: `$%d'", n->line, n->col, reg);
        exit(1);
    }
}

static char * immediatify(struct node * n) {
    if(n->type == ConstantWrapper)
        return n->value;
    else if(n->type == RegisterReference) {
        char * reg_lookup[] = {"r1", "r2", "r3", "r4", "r5", "r6"};
        int reg = atoi(n->value);
        validate_register(reg, n);
        return reg_lookup[reg - 1];
    } else if(n->type == RefNode) {
        char * name = n->value;
        struct symbol_t * sym = fetch_symbol(SymbolData, name, NULL);
        
        if(!sym) {
            fprintf(stderr, "[%d:%d] Unknown data symbol: `%s'\n", n->line, n->col, n->value);
            exit(1);
        }

        char * buf;

        asprintf(&buf, sym->linkage == InternalLinkage ? "*_internal_%s" : "*%s", name);

        return buf;
    } else {
        fprintf(stderr, "[%d:%d] Unknown immediate type.\n", n->line, n->col);
        exit(1);
    }
}

static void emit_expr(char * insn, struct node * n) {
    char * reg_lookup[] = {"r1", "r2", "r3", "r4", "r5", "r6"};
    int reg = atoi(n->child->value);
    
    validate_register(reg, n);

    if(n->child->brother->type == ConstantWrapper
    || n->child->brother->type == RegisterReference
    || n->child->brother->type == RefNode) {
        emit(insn, reg_lookup[reg - 1], immediatify(n->child->brother));
    } else if(n->child->brother->type == MinusExpression
           || n->child->brother->type == PlusExpression) {
        struct node * expr = n->child->brother;
        emit(insn, reg_lookup[reg - 1], immediatify(expr->child));
        struct node * child_expr = expr->child->brother;
        
        while(child_expr->child) {
            if(expr->type == MinusExpression)
                emit("sub", reg_lookup[reg - 1], immediatify(child_expr->child));
            else if(expr->type == PlusExpression)
                emit("add", reg_lookup[reg - 1], immediatify(child_expr->child));
            child_expr = child_expr->child->brother;
            expr = expr->child->brother;
        }
        
        if(child_expr) {
            if(expr->type == MinusExpression)
                emit("sub", reg_lookup[reg - 1], immediatify(child_expr));
            else if(expr->type == PlusExpression)
                emit("add", reg_lookup[reg - 1], immediatify(child_expr));
        }
    } else {
        fprintf(stderr, "[%d:%d] Unknown expression node.", n->line, n->col);
        exit(1);
    }
}

static void unroll_pop(struct node * n) {
    if(n->brother)
        unroll_pop(n->brother);
    
    emit("pop", immediatify(n), NULL);
}

static void dispatch_imperative(struct node * n) {
    switch(n->type) {
        case ImperativeReturn: {
            if(context.in_preserve) {
                fprintf(stderr, "[%d:%d] Warning: `ret' used inside `preserve'\n", n->line, n->col);
            }

            emit("ret", NULL, NULL);
            break;
        }

        case ImperativeExit: {
            emit("end", NULL, NULL);
            break;
        }

        case ImperativeAssign: {
            emit_expr("mov", n);

            break;
        }

        case ImperativeAssignAdd: {
            emit_expr("add", n);

            break;
        }

        case ImperativeOut: {
            emit("out", immediatify(n->child), NULL);

            break;
        }

        case ImperativeIn: {
            emit("in", immediatify(n->child), NULL);
            
            break;
        }

        case Preserve: {
            struct node * regs = n->child->child;
            struct node * unrolled_regs = regs;
            struct node * code = n->child->brother;

            while(regs) {
                emit("push", immediatify(regs), NULL);
                regs = regs->brother;
            }

            context.in_preserve = 1;

            while(code) {
                dispatch_imperative(code);
                code = code->brother;
            }

            context.in_preserve = 0;

            // Unroll a list.
            // Captain recursion.

            unroll_pop(unrolled_regs);

            break;
        }

        case Call: {
            // Has special arguments?
            if(n->child->brother) {
                struct node * code = n->child->brother;
                
                while(code) {
                    dispatch_imperative(code);
                    code = code->brother;
                }
            }

            struct node * name = n->child;
            struct node * arity = n->child->child;

            struct symbol_t * s = NULL;

            unsigned char status = 0;

            while((s = fetch_symbol(SymbolCode, name->value, s))) {
                char * buf = NULL;

                if(s->arity != atoi(arity->value))
                    continue;

                if(s->linkage == ExternalLinkage) {
                    asprintf(&buf, "#call(\"%s%s\")", name->value, arity->value);
                } else {
                    asprintf(&buf, "#call(\"_internal_%s%s\")", name->value, arity->value);
                }

                emit(buf, NULL, NULL);
                
                status = 1;

                break;
            }

            if(!status) {
                fprintf(stderr, "[%d:%d] Unresolved `%s/%s'\n", n->line, n->col, name->value, arity->value);
                exit(1);
            }

            break;
        }

        case ImperativeDeref: {
            struct node * from = n->child;
            struct node * to = n->child->brother;
            
            if(from->type != RegisterReference) {
                fprintf(stderr, "[%d:%d] Bogus deref.\n", n->line, n->col);
                exit(1);
            }

            emit("movf", immediatify(from), immediatify(to));
            break;
        }

        case While: {
            if(n->child->brother->brother) {
                // while { blocc1 } (cond) { blocc2 }
            } else {
                // while (cond) { blocc }
            }
        }
    }
}

static void dispatch_module(struct node * n) {
    switch(n->type) {
        case DataDeclaration: {
            // Discarded declaration.
            if(n->child == NULL) {
                // A string
                if(*n->value == '"') {
                    emit_txt(InternalLinkage, NULL, n->value);
                    context.origin += strlen(n->value) - 2;
                } else {
                    emit_data(InternalLinkage, NULL, Db, atoi(n->value));
                    context.origin++;
                }
            } else {
                if(*n->value == '"') {
                    // Strip the quotes
                    emit_txt(InternalLinkage, n->child->value, n->value);
                    context.origin += strlen(n->value) - 2;
                } else {
                    emit_data(InternalLinkage, n->child->value, Db, atoi(n->value));
                    context.origin++;
                }

                if(fetch_symbol(SymbolData, n->child->value, NULL)) {
                    fprintf(stderr, "[%d:%d] Data symbol `%s' redeclared.\n", n->line, n->col, n->child->value);
                    exit(1);
                }

                add_symbol(SymbolData, n->child->value, 0, InternalLinkage);
            }

            break;
        }

        case ExternalDataDeclaration: {
            if(*n->value == '"') {
                emit_txt(ExternalLinkage, n->child->value, n->value);
                context.origin += strlen(n->value) - 2;
            } else {
                emit_data(ExternalLinkage, n->child->value, Db, atoi(n->value));
                context.origin++;
            }

            if(fetch_symbol(SymbolData, n->child->value, NULL)) {
                fprintf(stderr, "[%d:%d] External data symbol `%s' redeclared.\n", n->line, n->col, n->child->value);
                exit(1);
            }

            add_symbol(SymbolData, n->child->value, 0, ExternalLinkage);

            break;
        }

        case InternalFunctionDeclaration: {
            char * name = n->value;
            int arity = atoi(n->child->value);
            struct node * body = n->child->brother;
            char * name_buf = NULL;
            unsigned char isStart = !strcmp(name, "_start") && arity == 0;

            if(context.functionsDeclared && isStart) {
                fprintf(stderr, "[%d:%d] _start/0 should be the first code symbol of a module.", n->line, n->col);
                exit(1);
            }
            
            context.functionsDeclared = 1;

            if(isStart && !context.startDeclared) {
                context.startDeclared = 1;
            } else if(isStart) {
                fprintf(stderr, "[%d:%d] _start/0 redeclared.", n->line, n->col);
                exit(1);
            }

            asprintf(&name_buf, "@_internal_%s%d", name, arity);

            if(fetch_symbol(SymbolCode, name, NULL)) {
                fprintf(stderr, "[%d:%d] `%s/%d' redeclared.", n->line, n->col, name, arity);
                exit(1);
            }

            add_symbol(SymbolCode, name, arity, InternalLinkage);

            enqueue(NULL, body);

            break;
        }

        case ExternalFunctionDeclaration: {
            char * name = n->value;
            int arity = atoi(n->child->value);
            struct node * body = n->child->brother;
            char * name_buf = NULL;

            context.functionsDeclared = 1;

            asprintf(&name_buf, "@%s%d", name, arity);

            if(fetch_symbol(SymbolCode, name, NULL)) {
                fprintf(stderr, "[%d:%d] `%s/%d' redeclared.", n->line, n->col, name, arity);
                exit(1);
            }

            add_symbol(SymbolCode, name, arity, ExternalLinkage);

            enqueue(name_buf, body);
            break;
        }

        default: {
            fprintf(stderr, "[%d:%d] ICE: unsupported declaration inside the module scope.\n", n->line, n->col);
            exit(1);
        }
    }
}

void dispatch(struct node * n) {
	switch(n->type) {
        case ProgramDeclaration: {
            struct node * tmp = n->child;

            if(context.programDeclared) {
                fprintf(stderr, "[%d:%d] `program' used twice.\n", n->line, n->col);
                exit(1);
            } else context.programDeclared = 1;

            while(tmp) {
                dispatch_program(tmp);
                tmp = tmp->brother;
            }

            break;
        }

        case ModuleDeclaration: {
            struct node * tmp = n->child;

            if(context.moduleDeclared) {
                fprintf(stderr, "[%d:%d] multiple modules declared in a single compilation unit.\n", n->line, n->col);
                exit(1);
            } else context.moduleDeclared = 1;

            while(tmp) {
                dispatch_module(tmp);
                tmp = tmp->brother;
            }

            finalize(dispatch_imperative);

            break;
        }

        default: {
            fprintf(stderr, "[%d:%d] ICE: unsupported declaration inside the toplevel scope.\n", n->line, n->col);
            exit(1);
        }
    }
}
