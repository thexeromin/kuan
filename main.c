#include <stdio.h>
#include <stdlib.h>

#include "./socket.h"

#define PORT "8181"

int main(void) {
    int listener;   // Listening socket descriptor
    
    // Set up and get a listening socket
    listener = get_listener_socket(PORT);

    if(listener == -1) {
        fprintf(stderr, "error gettting listening socket\n");
        exit(1);
    }

    printf("Listening on PORT: " PORT "\n");

    return 0;
}