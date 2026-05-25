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

// Mutex is used here to prevent two clients accessing the active connections variable at once, so the incrementation always works properly
pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;

void *client_thread(void *arg) {
    int client_fd = *(int*)arg;
    free(arg);

    // variable to check if the client ended their session
    int should_shutdown = handle_client(client_fd);

    close_socket(client_fd);

    pthread_mutex_lock(&connection_mutex);

    active_connections--;

    printf("Client disconnected.");
    printf("Current active connections: %d \n", active_connections);

    pthread_mutex_unlock(&connection_mutex);

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

        //
        pthread_mutex_lock(&connection_mutex);

        // Check if the max client number is reached or not
        if (active_connections >= MAX_PENDING_CONNECTIONS) {
            pthread_mutex_unlock(&connection_mutex);

            const char *response =
                "Max connections reached. Please wait. \n";

            write(client_fd, response, strlen(response));

            close(client_fd);

            printf("Client declined!");

            continue;
        }

        // Print the accepting message after checking if it works
        printf("New client accepted! \n");

        // Increases the active connections, if the limit hasn't been reached yet
        active_connections++;

        printf("Current active connections: %d \n", active_connections);

        pthread_mutex_unlock(&connection_mutex);

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