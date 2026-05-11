#ifndef LL_H
#define LL_H

#include <stddef.h>

/* A Node :D */
typedef struct Node {
    void *data;
    struct Node *next;
} Node;

/* Funtion ptr for Destructor */
typedef void (*Destructor)(void *);

typedef struct {
    Node *head;
    size_t length;
    Destructor destroy;
} LinkedList;

/* Initialise the linked list */
void ll_init(LinkedList *list, Destructor destroy);

/* Append a node */
void ll_append(LinkedList *list, void *data);

/* Get length */
size_t ll_length(LinkedList *list);

/* Get head */
Node *ll_begin(LinkedList *list);

/* Delete a node */
void ll_delete(LinkedList *list, void *data);

/* Destroy :D */
void ll_destroy(LinkedList *list);

#endif