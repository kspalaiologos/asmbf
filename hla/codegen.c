
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"

struct symbol_t {
	unsigned char type, arity;
	char * string;

    struct symbol_t * next;
};

struct {
    unsigned int stack_size;

    unsigned int origin;
    unsigned int segment;

    unsigned char moduleDeclared;
    unsigned char programDeclared;

    unsigned char functionsDeclared;
    unsigned char startDeclared;
} context;

static struct symbol_t * symbol_table = NULL, * symbol_table_head = NULL;

static void add_symbol(unsigned char type, char * name) {
    struct symbol_t * instance = malloc(sizeof(struct symbol_t));

    if(!symbol_table) {
        symbol_table = symbol_table_head = instance;
        symbol_table->type = type;
        symbol_table->string = name;
        symbol_table->next = NULL;
    } else {
        symbol_table_head->next = instance;
        symbol_table_head->next->type = type;
        symbol_table_head->next->string = name;
        symbol_table_head->next->next = NULL;
        symbol_table_head = symbol_table_head->next;
    }
}

struct symbol_t * fetch_symbol(unsigned char type, char * name) {
    struct symbol_t * ptr = symbol_table;
    
    while(ptr) {
        if(!strcmp(name, ptr->string) && type == ptr->type)
            return ptr;
        
        ptr = ptr->next;
    }

    return NULL;
}

enum {
    Txt, Db, Seg, Off, Stk
};

enum {
    ExternalLinkage, InternalLinkage
};

enum {
    NONE = -8, F3 = -7, R1 = -6, R2 = -5, R3 = -4, R4 = -3, R5 = -2, R6 = -1
};

enum {
    SymbolData, SymbolCode
};

struct code_chunk {
    char * instruction;
    int param1, param2;

    struct code_chunk * next;
};

struct data_chunk {
    unsigned char linkage;
    int type;
    char * identifier;

    union {
        char * char_sequence;
        int constant;
    } value;

    struct data_chunk * next;
};

static struct code_chunk * code = NULL, * code_head = NULL;
static struct data_chunk * data = NULL, * data_head = NULL;

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

static void emit(char * instruction, int param1, int param2) {
    struct code_chunk * instance = malloc(sizeof(struct code_chunk));

    if(!code) {
        code = code_head = instance;
        code->instruction = instruction;
        code->param1 = param1;
        code->param2 = param2;
        code->next = NULL;
    } else {
        code_head->next = instance;
        code_head->next->instruction = instruction;
        code_head->next->param1 = param1;
        code_head->next->param2 = param2;
        code_head->next->next = NULL;
        code_head = code_head->next;
    }
}

static void emit_txt(unsigned char linkage, char * identifier, char * string) {
    struct data_chunk * instance = malloc(sizeof(struct data_chunk));

    if(!data) {
        data = data_head = instance;
        data->next = NULL;
        data->type = Txt;
        data->linkage = linkage;
        data->identifier = identifier;
        data->value.char_sequence = string;
    } else {
        data_head->next = instance;
        data_head->next->next = NULL;
        data_head->next->type = Txt;
        data_head->next->linkage = linkage;
        data_head->next->identifier = identifier;
        data_head->next->value.char_sequence = string;
        data_head = data_head->next;
    }
}

static void emit_data(unsigned char linkage, char * identifier, int type, int n) {
    struct data_chunk * instance = malloc(sizeof(struct data_chunk));

    if(!data) {
        data = data_head = instance;
        data->next = NULL;
        data->type = type;
        data->identifier = identifier;
        data->linkage = linkage;
        data->value.constant = n;
    } else {
        data_head->next = instance;
        data_head->next->next = NULL;
        data_head->next->type = type;
        data_head->next->linkage = linkage;
        data_head->next->identifier = identifier;
        data_head->next->value.constant = n;
        data_head = data_head->next;
    }
}

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

static int immediatify(struct node * n) {
    if(n->type == ConstantWrapper)
        return atoi(n->value);
    else if(n->type == RegisterReference) {
        const int reg_lookup[] = {R1, R2, R3, R4, R5, R6};
        int reg = atoi(n->value);
        validate_register(reg, n);
        return reg_lookup[reg - 1];
    } else {
        fprintf(stderr, "NOT IMPLEMENTED!!\n");
        return 0;
    }
}

static void emit_expr(char * insn, struct node * n) {
    const int reg_lookup[] = {R1, R2, R3, R4, R5, R6};
    int reg = atoi(n->child->value);
    
    validate_register(reg, n);

    if(n->child->brother->type == ConstantWrapper) {
        emit(insn, reg_lookup[reg - 1], immediatify(n->child->brother));
    } else if(n->child->brother->type == RegisterReference) {
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
        
        if(expr->type == MinusExpression)
            emit("sub", reg_lookup[reg - 1], immediatify(child_expr));
        else if(expr->type == PlusExpression)
            emit("add", reg_lookup[reg - 1], immediatify(child_expr->child));
    }
}

static void unroll_pop(struct node * n) {
    if(n->brother)
        unroll_pop(n->brother);
    
    emit("pop", immediatify(n), NONE);
}

static void dispatch_imperative(struct node * n) {
    switch(n->type) {
        case ImperativeReturn: {
            emit("ret", NONE, NONE);
            break;
        }

        case ImperativeExit: {
            emit("end", NONE, NONE);
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

        case Preserve: {
            struct node * regs = n->child->child;
            struct node * unrolled_regs = regs;
            struct node * code = n->child->brother;

            while(regs) {
                emit("push", immediatify(regs), NONE);
                regs = regs->brother;
            }

            while(code) {
                dispatch_imperative(code);
                code = code->brother;
            }

            // Unroll a list.
            // Captain recursion.

            unroll_pop(unrolled_regs);

            break;
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

                if(fetch_symbol(SymbolData, n->child->value)) {
                    fprintf(stderr, "[%d:%d] Data symbol `%s' redeclared.\n", n->line, n->col, n->child->value);
                    exit(1);
                }

                add_symbol(SymbolData, n->child->value);
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

            if(fetch_symbol(SymbolData, n->child->value)) {
                fprintf(stderr, "[%d:%d] External data symbol `%s' redeclared.\n", n->line, n->col, n->child->value);
                exit(1);
            }

            add_symbol(SymbolData, n->child->value);

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

            if(fetch_symbol(SymbolCode, name_buf + 1)) {
                fprintf(stderr, "[%d:%d] `%s/%d' redeclared.", n->line, n->col, name, arity);
                exit(1);
            }

            if(!isStart)
                emit(name_buf, NONE, NONE);

            while(body) {
                dispatch_imperative(body);
                body = body->brother;
            }

            break;
        }

        case ExternalFunctionDeclaration: {
            char * name = n->value;
            int arity = atoi(n->child->value);
            struct node * body = n->child->brother;
            char * name_buf = NULL;

            context.functionsDeclared = 1;

            asprintf(&name_buf, "@%s%d", name, arity);

            if(fetch_symbol(SymbolCode, name_buf + 1)) {
                fprintf(stderr, "[%d:%d] `%s/%d' redeclared.", n->line, n->col, name, arity);
                exit(1);
            }

            emit(name_buf, NONE, NONE);

            while(body) {
                dispatch_imperative(body);
                body = body->brother;
            }

            break;
        }

        default: {
            fprintf(stderr, "[%d:%d] ICE: unsupported declaration inside the module scope.\n", n->line, n->col);
            exit(1);
        }
    }
}

void flush_code(FILE * f) {
    {
        struct data_chunk * iterator = data;

        while(iterator) {
            switch(iterator->type) {
                case Txt:
                    if(iterator->identifier) {
                        fprintf(f,
                            iterator->linkage == ExternalLinkage ?
                                "&%s\n" : "&_internal_%s\n"
                            , iterator->identifier
                        );
                    }

                    fprintf(f, "txt %s\n", iterator->value.char_sequence);
                    break;
                case Db:
                    if(iterator->identifier) {
                        fprintf(f,
                            iterator->linkage == ExternalLinkage ?
                                "&%s\n" : "&_internal_%s\n"
                            , iterator->identifier
                        );
                    }

                    fprintf(f, "db %d\n", iterator->value.constant);
                    break;
                case Seg:
                    fprintf(f, "seg %d\n", iterator->value.constant);
                    break;
                case Off:
                    fprintf(f, "org %d\n", iterator->value.constant);
                    break;
                case Stk:
                    fprintf(f, "stk %d\n", iterator->value.constant);
                    break;
            }

            iterator = iterator->next;
        }
    }
    {
        struct code_chunk * iterator = code;
        
        const char * regs[] = {
            NULL, "r6", "r5", "r4", "r3", "r2", "r1", "f3"
        };
        
        while(iterator) {
            fprintf(f, "%s", iterator->instruction);
            
            if(iterator->param1 != NONE) {
                if(iterator->param1 >= 0)
                    fprintf(f, " %d", iterator->param1);
                else
                    fprintf(f, " %s", regs[-iterator->param1]);
            }

            if(iterator->param2 != NONE) {
                if(iterator->param2 >= 0)
                    fprintf(f, ", %d\n", iterator->param2);
                else
                    fprintf(f, ", %s\n", regs[-iterator->param2]);
            } else
                fprintf(f, "\n");

            iterator = iterator->next;
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

            break;
        }

        default: {
            fprintf(stderr, "[%d:%d] ICE: unsupported declaration inside the toplevel scope.\n", n->line, n->col);
            exit(1);
        }
    }
}
