#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <threads.h>

#include "./socket.h"

#define PORT "8181"

int handle_connection(void *arg);

int main(void) {
    int listener;   // Listening socket descriptor
    int new_fd;      // Newly accept()ed socket descriptor

    struct sockaddr_storage client_addr;     // Client address
    socklen_t addrlen;

    char client_ip[INET6_ADDRSTRLEN];

    // Set up and get a listening socket
    listener = get_listener_socket(PORT);

    if(listener == -1) {
        fprintf(stderr, "error gettting listening socket\n");
        exit(1);
    }

    printf("[kuan]: listening on port: " PORT "\n");

    // Main accept() loop
    while(1) {
        addrlen = sizeof client_addr;
        new_fd = accept(
            listener,
            (struct sockaddr *)&client_addr,
            &addrlen
        );
        if(new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(
            client_addr.ss_family,
            get_in_addr((struct sockaddr *)&client_addr),
            client_ip,
            sizeof client_ip
        );
        printf("[kuan]: got connection from %s\n", client_ip);

        // handle request in separate thread
        thrd_t t;
        int *p_fd = malloc(sizeof *p_fd);
        *p_fd = new_fd;

        thrd_create(&t, handle_connection, p_fd);
        thrd_detach(t);
    }

    return 0;
}

int handle_connection(void *arg) {
    int client_fd = *(int*)arg;
    free(arg);

    // TODO: parse request

    // TODO: send response
    if (send(client_fd, "Hello, world!", 13, 0) == -1)
        perror("send");
    close(client_fd);
    return 0;
}