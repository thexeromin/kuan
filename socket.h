#include <sys/socket.h>

#ifndef SOCKET_H
#define SOCKET_H

void *get_in_addr(struct sockaddr *sa);
int get_listener_socket(char *port);

#endif // SOCKET_H
