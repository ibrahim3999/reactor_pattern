/***
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "st_reactor.h"

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 6810
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDRESS, &(server_address.sin_addr)) <= 0) {
        perror("Invalid server address");
        exit(1);
    }

    if (connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("Error connecting to server");
        exit(1);
    }

    printf("Connected to the server. Start typing messages:\n");

    while (1) {
        // Read user input
        fgets(buffer, BUFFER_SIZE, stdin);

        // Send the message to the server
        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
            perror("Error sending message to server");
            exit(1);
        }
    }
    close(client_socket);

    return 0;
}
*/