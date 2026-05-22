/*
 * Client Handler Source File
 * Handles commmunication with one connected client.
 * Created by CallMeMosaic
 *
 */

#include "../include/client_handler.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

// Function to handle communication with a connected client
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE] = {0};

    //Read message from the connected client
    ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE-1);

    // Checks if bytes read returns negative int, meaning read failed
    if (bytes_read < 0) {
        perror("Failed to read from client :c");
        return;
    }

    if(bytes_read == 0) {
        printf("Client disconnected without sendind data -_- \n");
        return;
    }

    //


}