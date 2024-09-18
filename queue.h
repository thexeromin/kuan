#ifndef QUEUE_H
#define QUEUE_H

struct node {
    struct node *next;
    int *sock_fd;
};

typedef struct node node_t;

void enqueue(int *sock_fd);
int* dequeue();
 
#endif // QUEUE_H