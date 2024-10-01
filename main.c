#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <threads.h>

#include "./socket.h"
#include "./queue.h"

#define PORT "8181"
#define MAX_DATA_SIZE 1000
#define THREAD_POOL_SIZE 5

typedef struct {
    char method[5];
    char path[10];
} Req;

thrd_t thread_pool[THREAD_POOL_SIZE];
mtx_t mutex;
cnd_t cond_var;

int handle_connection(void *arg);
int thread_function(void *arg);
void parse_request(char *str, Req *req);
int handle_send(int sock_fd, char *fname);

int main(void) {
    int listener;   // Listening socket descriptor
    int new_fd;      // Newly accept()ed socket descriptor

    struct sockaddr_storage client_addr;     // Client address
    socklen_t addrlen;

    char client_ip[INET6_ADDRSTRLEN];

    // init mutex & conditional variables
    mtx_init(&mutex, mtx_plain);
    cnd_init(&cond_var);

    // create thread pool
    for(int i = 0; i < THREAD_POOL_SIZE; i++) {
        thrd_create(thread_pool + i, thread_function, NULL);
    }

    // Set up and get a listening socket
    listener = get_listener_socket(PORT);

    if(listener == -1) {
        fprintf(stderr, "error gettting listening socket\n");
        exit(1);
    }

    printf("[kuan]: serving at http://localhost:" PORT "\n");

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
        int *p_fd = malloc(sizeof *p_fd);
        *p_fd = new_fd;
        mtx_lock(&mutex);
        enqueue(p_fd);
        cnd_signal(&cond_var);
        mtx_unlock(&mutex);
    }

    return 0;
}

int thread_function(void *arg) {
    (void)arg;

    while(1) {
        int *psock_fd;
        mtx_lock(&mutex);
        if((psock_fd = dequeue()) == NULL) {
            cnd_wait(&cond_var, &mutex);
            psock_fd = dequeue();
        }
        mtx_unlock(&mutex);

        if(psock_fd != NULL) {
            // we have a connection
            handle_connection(psock_fd);
        }
    }
}

int handle_connection(void *arg) {
    int client_fd = *(int*)arg;
    free(arg);
    char buf[MAX_DATA_SIZE];
    int numbytes;
    Req req;

    // parse request
    if((numbytes = recv(client_fd, buf, MAX_DATA_SIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';

    parse_request(buf, &req);

    // handle routes
    if(strcmp(req.method, "GET") != 0) {
        handle_send(client_fd, "public/404.html");
    }

    if(strcmp(req.path, "/") == 0) {
        handle_send(client_fd, "public/index.html");
    } else if(strcmp(req.path, "/about") == 0) {
        handle_send(client_fd, "public/about.html");
    } else if(strcmp(req.path, "/contact") == 0) {
        handle_send(client_fd, "public/contact.html");
    }

    close(client_fd);
    return 0;
}

void parse_request(char *str, Req *req) {
    int parse_idx = 0;
    int m, n;
    m = n = 0;

    for(int i = 0; str[i]; i++) {
        if(str[i] == ' ') parse_idx++;

        if(parse_idx > 1) {
            break;
        }

        if(str[i] != ' ') {
            switch(parse_idx) {
            case 0:
                // method parse
                req->method[m++] = str[i];
                break;
            case 1:
                // url path parse
                req->path[n++] = str[i];
            }
        }
    }

    req->method[m] = '\0';
    req->path[n] = '\0';
}

int handle_send(int sock_fd, char *fname) {
    char buf[2000];
    char res[2500];
    FILE *fp = fopen(fname, "r");

    if(!fp) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }

    // copy file to string
    int c, i;
    for(i = 0; (c = fgetc(fp)) != EOF; i++)
        buf[i] = c;
    buf[i] = '\0';

    // build response
    sprintf(
        res,
        "HTTP/1.1 200 OK\r\nconten-type: text/html\r\n\r\n%s",
        buf
    );

    // send it
    if (send(sock_fd, res, strlen(res), 0) == -1)
        perror("send");

    return 0;
}
