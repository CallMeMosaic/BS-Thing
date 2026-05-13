//
// Basic script for TCP based server
// Done by CallMeMosaic
//

// Include necessary header files
#include <stdio.h>      // For input/output functions (printf, perror)
#include <stdlib.h>     // For exit() and EXIT_FAILURE
#include <string.h>     // For string functions (strlen, memset)
#include <unistd.h>     // For POSIX API functions (read, write, close)
#include <sys/types.h>  // For data types like socklen_t
#include <sys/socket.h> // For socket functions (socket, bind, listen, accept)
#include <netinet/in.h> // For IPv4 address structure (sockaddr_in) and htons()

// Define the port number to listen on
#define PORT 8080
// Define the buffer size for reading/writing data
#define BUFFER_SIZE 1024

int main() {
    // --- Step 1: Create a TCP socket ---
    // socket() creates a new socket for communication.
    // AF_INET = IPv4 protocol family
    // SOCK_STREAM = TCP (reliable, connection-oriented)
    // 0 = Default protocol (TCP for SOCK_STREAM)
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Failed to create socket"); // Print error message to stderr
        exit(EXIT_FAILURE);                 // Exit the program with a failure status
    }
    printf("Socket created successfully.\n");

    // --- Step 2: Define the server address ---
    // sockaddr_in is a structure that defines an IPv4 address.
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;          // Use IPv4
    server_address.sin_addr.s_addr = INADDR_ANY;  // Listen on all network interfaces (0.0.0.0)
    server_address.sin_port = htons(PORT);         // Convert port to network byte order

    // --- Step 3: Bind the socket to the address ---
    // bind() assigns the address (IP + port) to the socket.
    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket bound to port %d.\n", PORT);

    // --- Step 4: Listen for incoming connections ---
    // listen() marks the socket as passive (ready to accept connections).
    // The second argument (3) is the maximum number of pending connections in the queue.
    if (listen(server_fd, 3) < 0) {
        perror("Failed to listen on socket");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", PORT);

    // --- Step 5: Accept a client connection ---
    // accept() waits for a client to connect and returns a new socket for communication.
    struct sockaddr_in client_address;
    socklen_t client_addr_len = sizeof(client_address);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_addr_len);
    if (client_fd < 0) {
        perror("Failed to accept client connection");
        exit(EXIT_FAILURE);
    }
    printf("Client connected.\n");

    // --- Step 6: Read data from the client ---
    // Buffer to store the received message
    char buffer[BUFFER_SIZE] = {0}; // Initialize with zeros
    // read() reads data from the client socket into the buffer.
    ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read < 0) {
        perror("Failed to read from client");
        exit(EXIT_FAILURE);
    }
    printf("Received message from client: %s\n", buffer);

    // --- Step 7: Send a response back to the client ---
    char *response = "Server received your message!";
    // write() sends the response to the client.
    ssize_t bytes_sent = write(client_fd, response, strlen(response));
    if (bytes_sent < 0) {
        perror("Failed to send response to client");
        exit(EXIT_FAILURE);
    }
    printf("Response sent to client.\n");

    // --- Step 8: Clean up ---
    // Close the client and server sockets to free resources.
    close(client_fd);
    close(server_fd);
    printf("Server shutting down.\n");

    return 0;
}