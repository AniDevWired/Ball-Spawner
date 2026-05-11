#include "ll.h"
#include <stdlib.h>

void ll_init(LinkedList *list, Destructor destroy) {
    list->head = NULL;
    list->length = 0;
    list->destroy = destroy;
}

void ll_append(LinkedList *list, void *data) {
    Node *newNode = malloc(sizeof(Node));

    if(!newNode) return;

    newNode->data = data;
    newNode ->next = NULL;

    if(list->head == NULL) {
        list->head = newNode;
    } else {
        Node *currentNode = list->head;
        while (currentNode->next != NULL) {
            currentNode = currentNode->next;
        }
        currentNode->next = newNode;
    }
    list->length++;
}

size_t ll_length(LinkedList *list) {
    return list->length;
}

Node *ll_begin(LinkedList *list) {
    return list->head;
}

void ll_destroy(LinkedList *list) {

    Node *currentNode = list->head;

    if(currentNode == NULL) return;

    while (currentNode != NULL) {
        Node *next = currentNode->next;

        // destroy actual obj
        if(list->destroy) {
            list->destroy(currentNode->data);
        }

        // free da node
        free(currentNode);

        currentNode = next;
    }

    list->head = NULL;
    list->length = 0;
}
