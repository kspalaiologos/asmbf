
#include "symtab.h"
#include <stdlib.h>
#include <string.h>

static struct symbol_t * symbol_table = NULL, * symbol_table_head = NULL;

void add_symbol(unsigned char type, char * name, unsigned char arity, unsigned char linkage) {
    struct symbol_t * instance = malloc(sizeof(struct symbol_t));

    if(!symbol_table) {
        symbol_table = symbol_table_head = instance;
        symbol_table->type = type;
        symbol_table->arity = arity;
        symbol_table->string = name;
        symbol_table->linkage = linkage;
        symbol_table->next = NULL;
    } else {
        symbol_table_head->next = instance;
        symbol_table_head->next->type = type;
        symbol_table_head->next->string = name;
        symbol_table_head->next->linkage = linkage;
        symbol_table_head->next->arity = arity;
        symbol_table_head->next->next = NULL;
        symbol_table_head = symbol_table_head->next;
    }
}

struct symbol_t * fetch_symbol(unsigned char type, char * name, struct symbol_t * begin) {
    struct symbol_t * ptr = symbol_table;
    
    if(begin)
        ptr = begin;

    while(ptr) {
        if(!strcmp(name, ptr->string) && type == ptr->type)
            return ptr;
        
        ptr = ptr->next;
    }

    return NULL;
}
