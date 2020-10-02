
#include "modq.h"
#include "emitter.h"

#include <stdlib.h>

static struct enqueued_dispatch_t * dispatch_queue = NULL, * dispatch_queue_head = NULL;

void enqueue(char * name, struct node * n) {
    if(dispatch_queue == NULL) {
        dispatch_queue = dispatch_queue_head = malloc(sizeof(struct enqueued_dispatch_t));
        dispatch_queue_head->next = NULL;
        dispatch_queue_head->n = n;
        dispatch_queue_head->str = name;
    } else {
        dispatch_queue_head->next = malloc(sizeof(struct enqueued_dispatch_t));
        dispatch_queue_head = dispatch_queue_head->next;
        dispatch_queue_head->n = n;
        dispatch_queue_head->str = name;
    }
}

void finalize(void (callback(struct node * n))) {
    while(dispatch_queue) {
        if(dispatch_queue->str)
            emit(dispatch_queue->str, NULL, NULL);

        while(dispatch_queue->n) {
            callback(dispatch_queue->n);
            dispatch_queue->n = dispatch_queue->n->brother;
        }

        dispatch_queue = dispatch_queue->next;
    }
}
