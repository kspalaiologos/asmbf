
#ifndef _SYMTAB_H
#define _SYMTAB_H

struct symbol_t {
	unsigned char type, arity, linkage;
	char * string;

    struct symbol_t * next;
};
enum {
    ExternalLinkage, InternalLinkage
};

enum {
    SymbolData, SymbolCode
};

void add_symbol(unsigned char type, char * name, unsigned char arity, unsigned char linkage);
struct symbol_t * fetch_symbol(unsigned char type, char * name, struct symbol_t * begin);

#endif
