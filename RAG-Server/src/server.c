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
#include <pthread.h>    // Used for Multithreading


// Include custom headers
#include "../include/server_socket.h"
#include "../include/client_handler.h"
#include "../include/config.h"

// Define the client counter
int active_connections = 0;

pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;

void *client_thread(void *arg) {
    int client_fd = *(int*)arg;
    free(arg);

    int should_shutdown = handle_client(client_fd);

    close_socket(client_fd);

    if (should_shutdown) {
        printf("Server shutting down.\n");
        exit(0);
    }
    return NULL;
}



int main() {
    // Creates the server socket at the specified Port returns the listening socket ID
    int server_fd = create_server_socket(PORT);

    // Define a termination variable
    int run = 1;

    // Accept loop
    while (run) {
        // Accepts clients and returns the client's communication socket
        int client_fd = accept_client(server_fd);

        //
        int *client_fd_ptr = malloc(sizeof(int));

        // Check if the max client number is reached or not
        if (active_connections >= MAX_PENDING_CONNECTIONS) {}

        if (client_fd_ptr == NULL) {
            perror("ERROR: FAILED TO ALLOCATE MEMORY FOR CLIENT SOCKET!");
            close_socket(client_fd);
            continue;
        }

        *client_fd_ptr = client_fd;

        pthread_t thread_id;

        if (pthread_create(&thread_id, NULL, client_thread, client_fd_ptr) != 0) {
            perror("ERROR: FAILED TO CREATE CLIENT THREAD!");
            close_socket(client_fd);
            free(client_fd_ptr);
            continue;
        }

        pthread_detach(thread_id);
    }
}