#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define PORT 6803

typedef struct {
    int client_socket;
    struct sockaddr_in client_address;
} Client;

void initialize_clients(Client* clients);

typedef struct {
    Client clients[MAX_CLIENTS];
    struct pollfd fds[MAX_CLIENTS + 1];
    nfds_t num_fds;
    int active;
    pthread_t reactor_thread;
} Reactor;

typedef void (*handler_t)(int fd);  // typedef for handler function pointer

Reactor* createReactor(int server_socket) {
    Reactor* reactor = (Reactor*)malloc(sizeof(Reactor));
    if (reactor == NULL) {
        perror("Error creating reactor");
        exit(1);
    }

    initialize_clients(reactor->clients);

    reactor->num_fds = 1;
    reactor->fds[0].fd = server_socket;
    reactor->fds[0].events = POLLIN;
    reactor->active = 1;

    return reactor;
}

void handle_client_message(Reactor* reactor, int current_client, char* message) {
    printf("Received message from client %d: %s\n", current_client, message);

    // Broadcast the message to all connected clients except the sender
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (i != current_client && reactor->clients[i].client_socket != -1) {
            if (send(reactor->clients[i].client_socket, message, strlen(message), 0) == -1) {
                perror("Error sending message to client");
                exit(1);
            }
        }
    }
}

void handle_new_connection(Reactor* reactor, int server_socket) {
    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);
    int new_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length);

    // Find an empty slot to store the new client information
    int i;
    for (i = 0; i < MAX_CLIENTS; ++i) {
        if (reactor->clients[i].client_socket == -1) {
            reactor->clients[i].client_socket = new_socket;
            reactor->clients[i].client_address = client_address;
            break;
        }
    }

    if (i == MAX_CLIENTS) {
        fprintf(stderr, "Max clients reached. Connection rejected.\n");
        close(new_socket);
    } else {
        printf("New connection established: client %d\n", i);
        reactor->fds[reactor->num_fds].fd = new_socket;
        reactor->fds[reactor->num_fds].events = POLLIN;
        reactor->num_fds++;
    }
}

void handle_client_disconnection(Reactor* reactor, int current_client) {
    printf("Client %d disconnected\n", current_client);
    close(reactor->clients[current_client].client_socket);
    reactor->clients[current_client].client_socket = -1;
}

void initialize_clients(Client* clients) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i].client_socket = -1;
    }
}

void stopReactor(void* this) {
    Reactor* reactor = (Reactor*)this;
    reactor->active = 0;
}

void startReactor(void* this) {
    Reactor* reactor = (Reactor*)this;
    char buffer[BUFFER_SIZE];
    while (reactor->active) {
        int result = poll(reactor->fds, reactor->num_fds, -1);
        if (result == -1) {
            perror("Error in poll");
            exit(1);
        }

        if (reactor->fds[0].revents & POLLIN) {
            handle_new_connection(reactor, reactor->fds[0].fd);
        }

        for (int i = 1; i < reactor->num_fds; ++i) {
            if (reactor->fds[i].revents & POLLIN) {
                int current_client = -1;
                for (int j = 0; j < MAX_CLIENTS; ++j) {
                    if (reactor->clients[j].client_socket == reactor->fds[i].fd) {
                        current_client = j;
                        break;
                    }
                }

                if (current_client == -1) {
                    fprintf(stderr, "Unknown client socket\n");
                    continue;
                }

                int bytes_received = recv(reactor->fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0) {
                    handle_client_disconnection(reactor, current_client);
                    reactor->fds[i] = reactor->fds[reactor->num_fds - 1];
                    reactor->num_fds--;
                } else {
                    buffer[bytes_received] = '\0';
                    handle_client_message(reactor, current_client, buffer);
                }
            }
        }
    }
}

void addFd(void* this, int fd, handler_t handler) {
    Reactor* reactor = (Reactor*)this;
    reactor->fds[reactor->num_fds].fd = fd;
    reactor->fds[reactor->num_fds].events = POLLIN;
    reactor->num_fds++;
}

void* reactorThread(void* arg) {
    Reactor* reactor = (Reactor*)arg;
    startReactor(reactor);
    return NULL;
}

void startReactorThread(Reactor* reactor) {
    pthread_create(&(reactor->reactor_thread), NULL, reactorThread, (void*)reactor);
}

void WaitFor(void* this) {
    Reactor* reactor = (Reactor*)this;
    pthread_join(reactor->reactor_thread, NULL);
}

void handler1(int fd) {
    printf("Handler 1 fired for fd: %d\n", fd);
}

void handler2(int fd) {
    printf("Handler 2 fired for fd: %d\n", fd);
}

int main() {
    int server_socket, current_client;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        exit(1);
    }

    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error listening on socket");
        exit(1);
    }

    printf("Server started. Waiting for connections...\n");

    Reactor* reactor = createReactor(server_socket);

    // Create additional file descriptors and their handlers
    int fd1 = 123;  // Replace with your own file descriptor
    int fd2 = 456;  // Replace with your own file descriptor
    int fd3 = 789;  // Replace with your own file descriptor
    // Create corresponding handler functions for each fd
    handler_t ft7i = handler1;
    handler_t foaad = handler2;
    // Add the file descriptors and handlers to the reactor
    addFd(reactor, fd1, handler1);
    addFd(reactor, fd2, handler2);

    startReactorThread(reactor);

    // Wait for the reactor thread to finish
    WaitFor(reactor);

    // Free memory
    free(reactor);

    return 0;
}
