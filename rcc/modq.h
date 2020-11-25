
#ifndef _MODQ_H
#define _MODQ_H

#include "node.h"

struct enqueued_dispatch_t {
    struct node * n;
    char * str;

    struct enqueued_dispatch_t * next;
};

void enqueue(char * name, struct node * n);
void finalize(void (callback(struct node * n)));

#endif
