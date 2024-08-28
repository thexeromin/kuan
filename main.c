#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "./socket.h"

#define PORT "8181"

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

        // launch a separate thread & handle request
    }

    return 0;
}