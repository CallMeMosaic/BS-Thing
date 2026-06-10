/*
 * server.c
 *
 * Main entry point of the TCP server.
 *
 * Responsibilities:
 * - create the listening socket
 * - initialize the Pub/Sub system
 * - accept incoming clients
 * - enforce the maximum active connection limit
 * - start one worker thread per connected client
 *
 * @author CallMeMosaic
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "../include/server_socket.h"
#include "../include/client_handler.h"
#include "../include/config.h"
#include "../include/pubsub.h"

static int active_connections = 0;
static pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;

void *client_thread(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    int should_shutdown = handle_client(client_fd);

    close_socket(client_fd);

    pthread_mutex_lock(&connection_mutex);
    active_connections--;
    printf("Client disconnected. Current active connections: %d\n", active_connections);
    pthread_mutex_unlock(&connection_mutex);

    if (should_shutdown) {
        printf("Server shutting down.\n");
        exit(0);
    }

    return NULL;
}

int main(void) {
    int server_fd = create_server_socket(PORT);

    pubsub_init();

    while (1) {
        int client_fd = accept_client(server_fd);

        pthread_mutex_lock(&connection_mutex);

        if (active_connections >= MAX_PENDING_CONNECTIONS) {
            pthread_mutex_unlock(&connection_mutex);

            const char *response = "Max connections reached. Please wait.\n";
            write(client_fd, response, strlen(response));

            close_socket(client_fd);
            printf("Client declined: max connections reached.\n");

            continue;
        }

        active_connections++;
        printf("New client accepted. Current active connections: %d\n", active_connections);

        pthread_mutex_unlock(&connection_mutex);

        int *client_fd_ptr = malloc(sizeof(int));

        if (client_fd_ptr == NULL) {
            perror("ERROR: Failed to allocate memory for client socket");

            pthread_mutex_lock(&connection_mutex);
            active_connections--;
            pthread_mutex_unlock(&connection_mutex);

            close_socket(client_fd);
            continue;
        }

        *client_fd_ptr = client_fd;

        pthread_t thread_id;

        if (pthread_create(&thread_id, NULL, client_thread, client_fd_ptr) != 0) {
            perror("ERROR: Failed to create client thread");

            pthread_mutex_lock(&connection_mutex);
            active_connections--;
            pthread_mutex_unlock(&connection_mutex);

            close_socket(client_fd);
            free(client_fd_ptr);
            continue;
        }

        pthread_detach(thread_id);
    }
}