#ifndef ST_REACTOR_H
#define ST_REACTOR_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

typedef struct {
    int client_socket;
    struct sockaddr_in client_address;
} Client;

typedef struct {
    Client* clients;
    size_t max_clients;
    struct pollfd* fds;
    nfds_t num_fds;
    int active;
    pthread_t reactor_thread;
} Reactor;

typedef void (*handler_t)(int fd);  // typedef for handler function pointer

Reactor* createReactor(int server_socket, size_t max_clients);
void handle_client_message(Reactor* reactor, int current_client, char* message);
void handle_new_connection(Reactor* reactor, int server_socket);
void handle_client_disconnection(Reactor* reactor, int current_client);
void startReactor(void* );
void stopReactor(void* );
void addFd(void* , int fd, handler_t handler);
void* reactorThread(void* arg);
void startReactorThread(Reactor* reactor);
void WaitFor(void* );
void handler1(int fd);
void handler2(int fd);

#endif  ST_REACTOR_H 
