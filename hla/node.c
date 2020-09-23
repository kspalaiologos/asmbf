
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"

struct node *new_node(
	unsigned int type,
	struct node *child, struct node *brother,
	char *value,
	unsigned int line, unsigned int col)
{
	struct node *instance = malloc(sizeof(struct node));
	assert(instance);

	instance->child = child;
	instance->value = value;
	instance->type = type;
	instance->brother = brother;
	instance->line = line;
	instance->col = col;

	return instance;
}

struct node * append_brother(struct node * parent, struct node * brother) {
    struct node * original_parent = parent;
    
    if(!parent) {
        return brother;
    }
    
    while(parent->brother)
        parent = parent->brother;
    
    parent->brother = brother;
    
    return original_parent;
}
