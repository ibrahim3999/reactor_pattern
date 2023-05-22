#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define PORT 3490

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

void initialize_clients(Client* clients, int start_index, int count) {
    for (int i = start_index; i < start_index + count; ++i) {
        clients[i].client_socket = -1;
    }
}


Reactor* createReactor(int server_socket, size_t max_clients) {
    Reactor* reactor = (Reactor*)malloc(sizeof(Reactor));
    if (reactor == NULL) {
        perror("Error creating reactor");
        exit(1);
    }

    reactor->clients = (Client*)malloc(max_clients * sizeof(Client));
    if (reactor->clients == NULL) {
        perror("Error creating clients array");
        exit(1);
    }

    reactor->fds = (struct pollfd*)malloc((max_clients + 1) * sizeof(struct pollfd));
    if (reactor->fds == NULL) {
        perror("Error creating pollfds array");
        exit(1);
    }

    reactor->max_clients = max_clients;
    reactor->num_fds = 1;
    reactor->fds[0].fd = server_socket;
    reactor->fds[0].events = POLLIN;
    reactor->active = 1;

    return reactor;
}

void handle_client_message(Reactor* reactor, int current_client, char* message) {
    printf("Received message from client %d: %s\n", current_client, message);

}

void handle_new_connection(Reactor* reactor, int server_socket) {
    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);
    int new_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length);

    // Check if there is an empty slot in the clients array
    int empty_slot = -1;
    for (int i = 0; i < reactor->max_clients; ++i) {
        if (reactor->clients[i].client_socket == -1) {
            empty_slot = i;
            break;
        }
    }

    if (empty_slot == -1) {
        // No empty slots available, allocate new memory for additional clients
        int new_max_clients = reactor->max_clients * 2;  // Double the current capacity
        Client* new_clients = realloc(reactor->clients, new_max_clients * sizeof(Client));
        if (new_clients == NULL) {
            fprintf(stderr, "Failed to allocate memory for additional clients\n");
            close(new_socket);
            return;
        }

        initialize_clients(new_clients, reactor->max_clients, new_max_clients - reactor->max_clients);  // Initialize new memory
        reactor->clients = new_clients;  // Update clients array pointer
        empty_slot = reactor->max_clients;  // Set the index of the new empty slot
        reactor->max_clients = new_max_clients;  // Update the max_clients value
    }

    // Store the new client information in the empty slot
    reactor->clients[empty_slot].client_socket = new_socket;
    reactor->clients[empty_slot].client_address = client_address;

    printf("New connection established: client %d\n", empty_slot);

    reactor->fds[reactor->num_fds].fd = new_socket;
    reactor->fds[reactor->num_fds].events = POLLIN;
    reactor->num_fds++;
}



void handle_client_disconnection(Reactor* reactor, int current_client) {
    printf("Client %d disconnected\n", current_client);
    close(reactor->clients[current_client].client_socket);
    reactor->clients[current_client].client_socket = -1;
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

        for (size_t i = 1; i < reactor->num_fds; ++i) {
            if (reactor->fds[i].revents & POLLIN) {
                int current_client = -1;
                for (size_t j = 0; j < reactor->max_clients; ++j) {
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
