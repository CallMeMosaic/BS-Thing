/*
 * Client Handler Source File
 * Handles commmunication with one connected client.
 * Created by CallMeMosaic
 *
 */

#include "../include/client_handler.h"
#include "../include/config.h"
#include "../include/request_parser.h"

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

        Request request = parse_request(buffer);

        switch (request.type) {
            case CMD_PING: {
                const char *response = "pong\n";
                write(client_fd, response, strlen(response));
                break;
            }

            case CMD_ECHO: {
                write(client_fd, request.argument, strlen(request.argument));
                write(client_fd, "\n", 1);
                break;
            }

            case CMD_HELP: {
                const char *response =
                    "HELP:\n"
                    "Available commands: ping, echo <message>, help, quit, shutdown\n";
                write(client_fd, response, strlen(response));
                break;
            }

            case CMD_QUIT: {
                const char *response = "Quitting...\n";
                write(client_fd, response, strlen(response));
                return 0;
            }

            case CMD_SHUTDOWN: {
                const char *response = "Shutting down...\n";
                write(client_fd, response, strlen(response));
                return 1;
            }

            case CMD_UNKNOWN:
            default: {
                const char *response = "Unknown command. Type help.\n";
                write(client_fd, response, strlen(response));
                break;
            }
            case CMD_MESSAGE: {
                const char *prefix = "Message received: ";
                write(client_fd, prefix, strlen(prefix));
                write(client_fd, request.argument, strlen(request.argument));
                write(client_fd, "\n", 1);
                break;
            }

            case REQ_GET: {
                const char *response = "GET request received for path: ";
                write(client_fd, response, strlen(response));
                write(client_fd, request.path, strlen(request.path));
                write(client_fd, "\n", 1);
                break;
            }

            case REQ_POST: {
                const char *response = "POST request received.\n";
                write(client_fd, response, strlen(response));
                break;
            }

            case REQ_PUT: {
                const char *response = "PUT request received.\n";
                write(client_fd, response, strlen(response));
                break;
            }
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
