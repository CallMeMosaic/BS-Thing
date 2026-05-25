#include "../include/client_handler.h"
#include "../include/request_parser.h"
#include "../include/response.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 8192

//Handles one client request and sends an Http response
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE] = {0};

    ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE -1);

    if (bytes_read < 0) {
        perror("Failed to read from client");
        return;
    }

    buffer[bytes_read] = '\0';

    printf("Received message from client: %s\n", buffer);

    HttpRequest request;

    if (!parse_http_request(buffer, &request)) {
        char* response = create_http_response(400, "text/plain", "Bad Request");

        if (response != NULL) {
            write(client_fd, response, strlen(response));
            free(response);
        }

        return;
    }

    char* response = NULL;

    if (strcmp(request.method, "POST") == 0 && strcmp(request.path, "/submit") == 0)
    {
        printf("Received POST body: %s\n", request.body);

        response = create_http_response(201, "text/plain", "Created");
    } else if (strcmp(request.method, "GET") == 0) {
        response = create_http_response(200, "text/plain", "OK");
    } else {
        response = create_http_response(404, "text/plain", "Not Found");
    }

    if (response != NULL) {
        write(client_fd, response, strlen(response));
        free(response);
    }

    printf("Response sent to client.\n")
}