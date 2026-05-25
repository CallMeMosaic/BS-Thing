/*
 * Client Handler Source File
 * Handles commmunication with one connected client.
 * Created by CallMeMosaic
 *
 */

#include "../include/client_handler.h"
#include "../include/config.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>



// Function to handle communication with a connected client
int handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    int connection_bool = 1;

    // create a client loop which closes if the "quit" command is sent

    while (connection_bool) {

        memset(buffer, '\0', BUFFER_SIZE);


        //Read message from the connected client
    ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);

    // Checks if bytes read returns negative int, meaning read failed
    if (bytes_read < 0) {
        perror("Failed to read from client :c");
        return  0;
    }

    if (bytes_read == 0) {
        printf("Client disconnected without sending data -_- \n");
        return 0;
    }

    // Make sure the message is null-terminated.
    buffer[bytes_read] = '\0';

    printf("Client received data : %s\n", buffer);

    // DEBUG BLOCK
    //printf("DEBUG: BUFFER RECEIVED AS: [%s]\n", buffer);
    //printf("DEBUG: bytes_read: %zd\n", bytes_read);

    if (strncmp(buffer, "shutdown\n",8) == 0 ||  strncmp(buffer, "exit\n",8) == 0 || strncmp(buffer, "shutdown",8) == 0 || strncmp(buffer, "Holmberg-15A",8) == 0) {
        const char *response = "Shutting down...";
        write(client_fd, response, strlen(response));
        //printf("SHUTDOWN RECEIVED! \n");
        return 1;
    }

    // Implementation of simple ping-pong, to check server response
    if (strncmp(buffer, "ping", 4) == 0) {
        const char *response = "pong\n";
        write(client_fd, response, strlen(response));
        continue;
    }

    // Implementation of simple echo command, to check message handling
    if (strncmp(buffer, "echo", 5) == 0) {
        const char *message = buffer + 5;
        printf("DEBUG");
        write(client_fd, message, strlen(message));
        continue;
    }

    // Implementation of a simple help command, to output all current commands
    if (strncmp(buffer, "help", 4) == 0) {
        const char *response =
            "HELP: \n"
            "Available commands are: ping, echo <message>, shutdown and quit \n";
        write(client_fd, response, strlen(response));
        continue;
    }

    if (strncmp(buffer, "quit", 4) == 0) {
        const char *response = "Quitting...";
        write(client_fd, response, strlen(response));
        const int *connection_bool = 0;
        return 0;
    }



    // Send response to client
    const char *response = "Server received your message! \n";

    ssize_t bytes_sent = write(client_fd, response, strlen(response));

    //write(client_fd, response, strlen(response));

    if (bytes_sent < 0) {
        perror("Failed to write to client :c");
        return 0;
    }

    printf("Response sent to client : %s\n", response);
    }


}
