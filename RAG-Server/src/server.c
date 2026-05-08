//
// Basic script for TCP based server
// Done by CallMeMosaic
//


// Necessary imports for input/output, strings and networking in C
#include "server.h"

#include <stdio.h>          // For printf() and perror()
#include <stdlib.h>         // For exit(), EXIT_FAILURE
#include <string.h>         // For strlen(), memset()
#include <unistd.h>         // For data types like socklen_t
#include <sys/socket.h>     // For socket(), bind(), listen(), accept()
#include <netinet/in.h>     // For struct sockaddr_in, htons(), INADDR_ANY
#include <net


// Defining constants for the port number to listen to and the Size of the buffer for reading/writing data
#define PORT 8080
#define BUFFER_SIZE 1024



int main() {
    // Defines variables of type int for server_fd and new_socket
    int server_fd, new_socket;
    // creates a new struct of scokaddr_in type with the name address
    struct sockaddr_in address;
    // initializes the int variable address_len with the size of the address struct
    int address_len = sizeof(address);

    // Create Socket

    // First checks if requirements are met, if not prints an error message and exits
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, address_len) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for incoming connections...\n Server listening on port %d...\n",PORT);

    // Infinite While loop to test
    int check = 1;
    while (1) {
        // Accept a client connection
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }

        // Read client message
        read(new_socket, buffer, BUFFER_SIZE);
        printf("Received: %s\n", buffer);
        if (buffer[0] != 'Z') {
            check = 0;
        }


        // Send a response (echo for now)
        char *response = "Hello from the C server!";
        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }

    return 0;
}