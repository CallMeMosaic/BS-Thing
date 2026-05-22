/*
 * Basic Server architecture for the server
 * Written my CallMeMosaic
 * Current Pipeline: Create socket → bind to port → listen → accept client → read message → send response → close
 *
 */

// Include necessary header files
#include <stdio.h>      // For input/output functions (printf, perror)
#include <stdlib.h>     // For exit() and EXIT_FAILURE
#include <string.h>     // For string functions (strlen, memset)
#include <unistd.h>     // For POSIX API functions (read, write, close)

#include "../include/server_socket.h"

// Define the port number to listen on
#define PORT 8080
// Define the buffer size for reading/writing data
#define BUFFER_SIZE 1024

int main() {

    // Creates the server socket at the specified Port returns the listening socket ID
    int server_fd = create_server_socket(PORT);

    // Accepts clients and returns the client's communication socket
    int client_fd = accept_client(server_fd);

    // Empty char array of size BUFFER_SIZE that can later be writte into when messages come in
    char buffer[BUFFER_SIZE] = {0};

    // Reads the client message and assigns it to the bytes_read variable
    ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);

    if (bytes_read < 0) {
        // If message is empty, sockets are closed and error message is printed with safe exit
        perror("Failed to read from client");
        close_socket(client_fd);
        close_socket(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Received message from client: %s\n", buffer);

    char *response = "Server received your message!";

    // Assigns the bytes_sent variable to the value at which *response is pointing
    ssize_t bytes_sent = write(client_fd, response, strlen(response));

    if (bytes_sent < 0) {
        // Same thing as for received, won't explain again
        perror("Failed to send response to client");
        close_socket(client_fd);
        close_socket(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Response sent to client.\n");

    close_socket(client_fd);
    close_socket(server_fd);

    printf("Server shutting down.\n");

    return 0;
}