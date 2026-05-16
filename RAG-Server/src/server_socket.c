/*
 * Server Socket Source File. Used to create and manage sockets, also used for client accepting
 * Created by CallMeMosaic
 */


// Import function declarations to later pass off to server.c
#include "../include/server_socket.h"

#include <stdio.h>      // For input/output functions (printf, perror)
#include <stdlib.h>     // For string functions (strlen, memset)
#include <unistd.h>     // For POSIX/Linux API functions (read, write, close)
#include <sys/types.h>  // For data types like socklen_t
#include <sys/socket.h> // For socket functions (socket, bind, listen, accept)
#include <netinet/in.h> // For IPv4 address structure (sockaddr_in) and htons()

#define MAX_PENDING_CONNECTIONS 3


int create_server_socket(int port) {

    // Creates a TCP Socket with AF_INET (IPv4 Address), SOCK_STREAM (TCP), 0 (Default TCP Protocol) and returns a positive integer = socket descriptor or -1 if creation failed
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Checks if the socket creation worked, if the return value is -1 it failed
    if (server_fd == -1) {
        // Prints a system error and exits, if socket creation failed
        perror("Failed to create socket");
        // Exits the program in case of failure
        exit(EXIT_FAILURE);
    }

    // Print to validate
    printf("Socket created successfully.\n");


    // Create a Struct sockaddr_in holding IP Address, Port, protocol family
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;            // Use IPv4
    server_address.sin_addr.s_addr = INADDR_ANY;    // Accept all interfaces so localhost, LAN IP, Docker Interace and so on are accepted
    server_address.sin_port = htons(port);          // Port assignment. htons converts CPU byte order to network byte order

    // Attaches the socket to the IP Adress and the Port unsing bind by passing the socket (server_fd), converts sockaddr to a generic socket address abd the sizeof(server_address)
    // Checks if the bind() functio returns bigger than 0 if so binding was successful
    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        // Prints a system error
        perror("Failed to bind socket");
        // Closes the socket
        close(server_fd);
        // Exits the program in case of failure
        exit(EXIT_FAILURE);
    }

    // Print for validation
    printf("Socket bound to port %d.\n", port);

    // Turns the socket into a passive/listening socket. Before this the socket was just a communication endpoint, now it's actually able to receive clients
    // Checks if the function call for listen() with parameters socket and Max_Pending_Connections returns bigger than 0, which indicates the listening works
    if (listen(server_fd, MAX_PENDING_CONNECTIONS) < 0) {
        // If not prints system error
        perror("Failed to listen on socket");
        // Closes socket
        close(server_fd);
        // Exits the program in case of failure
        exit(EXIT_FAILURE);
    }

    // Print for validation
    printf("Server is listening on port %d...\n", port);

    // return the socket (int)
    return server_fd;
}

// Function to accept clients duh
int accept_client(int server_fd) {
    // Struct of type sockaddr_in that stores info about the connection clients (IP, port, etc)
    struct sockaddr_in client_address;

    // Stores tge size of the address struct, needed cuz accept() modifies it
    socklen_t client_addr_len = sizeof(client_address);

    // Passes the socket, a cast and the client address to the accept function.
    // server_fd is the listening socket
    // client_fd is the communication socket
    int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_addr_len);

    // Checks if the communication socket is smaller 0, which would indicate that an invalid or failed socket was passed
    if (client_fd < 0) {
        // Prints error message
        perror("Failed to accept client connection");
        // Exits programm safely
        exit(EXIT_FAILURE);
    }

    // Print for validation
    printf("Client connected.\n");

    // returns the communication socket
    return client_fd;
}

// Just closes the socket
void close_socket(int socket_fd) {
    // closes the socket
    close(socket_fd);
}