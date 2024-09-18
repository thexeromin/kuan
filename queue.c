#include <stdlib.h>

#include "queue.h"

node_t *head = NULL;
node_t *tail = NULL;

void enqueue(int *sock_fd) {
    node_t *newnode = malloc(sizeof(node_t));
    newnode->sock_fd = sock_fd;
    newnode->next = NULL;
    if(tail == NULL) {
        head = newnode;
    } else {
        tail->next = newnode;
    }

    tail = newnode;
}

int* dequeue() {
    if(head == NULL) {
        return NULL;
    } else {
        int *result = head->sock_fd;
        node_t *temp = head;
        head = head->next;
        if(head == NULL) { tail = NULL; }
        free(temp);
        return result;
    }
}