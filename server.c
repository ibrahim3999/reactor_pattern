
#include "st_reactor.h"

#define BUFFER_SIZE 1024
#define PORT 6810

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

    if (listen(server_socket, SOMAXCONN) == -1) {
        perror("Error listening on socket");
        exit(1);
    }

    printf("Server started. Waiting for connections...\n");

    size_t max_clients = 10;  // Maximum number of clients
    Reactor* reactor = createReactor(server_socket, max_clients);

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
    free(reactor->clients);
    free(reactor->fds);
    free(reactor);

    return 0;
}
