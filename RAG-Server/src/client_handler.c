/*
 * client_handler.c
 *
 * Handles the communication with one connected client.
 *
 * This module is responsible for:
 * - reading data from the client socket
 * - deciding whether the received data is HTTP or plain text
 * - forwarding HTTP requests to the HTTP handler
 * - forwarding plain text commands to the command handler
 * - handling Pub/Sub commands
 * - cleaning up Pub/Sub subscriptions when a client disconnects
 *
 * Return values of handle_client():
 * 0 = client disconnected normally
 * 1 = client requested full server shutdown
 */

#include "../include/client_handler.h"
#include "../include/config.h"
#include "../include/request_parser.h"
#include "../include/response.h"
#include "../include/pubsub.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Checks whether the received message looks like an HTTP request.
 *
 * A real HTTP request usually contains an HTTP version string such as:
 * - HTTP/1.1
 * - HTTP/1.0
 *
 * If this is not found, we treat the message as plain text.
 */
static int looks_like_http_request(const char *buffer) {
    return strstr(buffer, "HTTP/1.1") != NULL ||
           strstr(buffer, "HTTP/1.0") != NULL;
}

/*
 * Handles plain text commands sent through tools like ncat.
 *
 * Example commands:
 * - ping
 * - echo hello
 * - SUB news
 * - PUB news hello everyone
 * - UNSUB news
 * - quit
 * - shutdown
 *
 * Return values:
 * 0 = keep client connected
 * 1 = request full server shutdown
 * 2 = close only this client connection
 */
static int handle_plain_text_request(int client_fd, char *buffer) {
    Request request = parse_request(buffer);

    switch (request.type) {
        case CMD_PING: {
            const char *response = "pong\n";
            write(client_fd, response, strlen(response));
            return 0;
        }

        case CMD_ECHO: {
            write(client_fd, request.argument, strlen(request.argument));
            write(client_fd, "\n", 1);
            return 0;
        }

        case CMD_HELP: {
            const char *response =
                "HELP:\n"
                "Commands:\n"
                "ping\n"
                "echo <message>\n"
                "SUB <topic>\n"
                "UNSUB <topic>\n"
                "PUB <topic> <message>\n"
                "quit\n"
                "shutdown\n";
            write(client_fd, response, strlen(response));
            return 0;
        }

        case REQ_SUB: {
            int success = subscribe_client(client_fd, request.topic);

            const char *response = success
                ? "Subscribed.\n"
                : "Could not subscribe.\n";

            write(client_fd, response, strlen(response));
            return 0;
        }

        case REQ_UNSUB: {
            int success = unsubscribe_client(client_fd, request.topic);

            const char *response = success
                ? "Unsubscribed.\n"
                : "Could not unsubscribe.\n";

            write(client_fd, response, strlen(response));
            return 0;
        }

        case REQ_PUB: {
            int delivered = publish_message(client_fd, request.topic, request.body);

            char response[128];
            snprintf(
                response,
                sizeof(response),
                "Published to %d subscriber(s).\n",
                delivered
            );

            write(client_fd, response, strlen(response));
            return 0;
        }

        case CMD_QUIT: {
            const char *response = "Quitting...\n";
            write(client_fd, response, strlen(response));
            return 2;
        }

        case CMD_SHUTDOWN: {
            const char *response = "Shutting down...\n";
            write(client_fd, response, strlen(response));
            return 1;
        }

        case CMD_MESSAGE: {
            const char *prefix = "Message received: ";
            write(client_fd, prefix, strlen(prefix));
            write(client_fd, request.argument, strlen(request.argument));
            write(client_fd, "\n", 1);
            return 0;
        }

        default: {
            const char *response = "Unknown command. Type help.\n";
            write(client_fd, response, strlen(response));
            return 0;
        }
    }
}

/*
 * Handles HTTP-style requests.
 *
 * Supported routes:
 * - GET  /health
 * - POST /submit
 * - POST /subscribe/<topic>
 * - POST /unsubscribe/<topic>
 * - POST /publish/<topic>
 * - POST /shutdown
 */
static int handle_http_request(int client_fd, char *buffer) {
    HttpRequest request;

    if (!parse_http_request(buffer, &request)) {
        char *response = create_http_response(400, "text/plain", "Bad Request");

        if (response != NULL) {
            write(client_fd, response, strlen(response));
            free(response);
        }

        return 0;
    }

    char *response = NULL;

    if (strcmp(request.method, "POST") == 0 &&
        strncmp(request.path, "/subscribe/", 11) == 0) {

        const char *topic = request.path + 11;
        int success = subscribe_client(client_fd, topic);

        response = create_http_response(
            success ? 200 : 500,
            "text/plain",
            success ? "Subscribed" : "Could not subscribe"
        );
    }
    else if (strcmp(request.method, "POST") == 0 &&
             strncmp(request.path, "/unsubscribe/", 13) == 0) {

        const char *topic = request.path + 13;
        int success = unsubscribe_client(client_fd, topic);

        response = create_http_response(
            success ? 200 : 404,
            "text/plain",
            success ? "Unsubscribed" : "Subscription not found"
        );
    }
    else if (strcmp(request.method, "POST") == 0 &&
             strncmp(request.path, "/publish/", 9) == 0) {

        const char *topic = request.path + 9;
        int delivered = publish_message(client_fd, topic, request.body);

        char body[128];
        snprintf(body, sizeof(body), "Published to %d subscriber(s)", delivered);

        response = create_http_response(200, "text/plain", body);
    }
    else if (strcmp(request.method, "POST") == 0 &&
             strcmp(request.path, "/shutdown") == 0) {

        response = create_http_response(200, "text/plain", "Server shutting down");

        if (response != NULL) {
            write(client_fd, response, strlen(response));
            free(response);
        }

        return 1;
    }
    else if (strcmp(request.method, "GET") == 0 &&
             strcmp(request.path, "/health") == 0) {

        response = create_http_response(200, "text/plain", "OK");
    }
    else if (strcmp(request.method, "POST") == 0 &&
             strcmp(request.path, "/submit") == 0) {

        printf("Received POST body: %s\n", request.body);
        response = create_http_response(201, "text/plain", "Created");
    }
    else if (strcmp(request.method, "GET") == 0) {
        response = create_http_response(200, "text/plain", "OK");
    }
    else {
        response = create_http_response(404, "text/plain", "Not Found");
    }

    if (response != NULL) {
        write(client_fd, response, strlen(response));
        free(response);
    }

    return 0;
}

/*
 * Main client loop.
 *
 * This function runs inside one client thread.
 * It repeatedly reads from the client socket until:
 * - the client disconnects
 * - the client sends quit
 * - the client sends shutdown
 * - a socket read error occurs
 */
int handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, '\0', BUFFER_SIZE);

        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);

        if (bytes_read < 0) {
            perror("Failed to read from client :c");
            remove_client_from_all_topics(client_fd);
            return 0;
        }

        if (bytes_read == 0) {
            printf("Client disconnected.\n");
            remove_client_from_all_topics(client_fd);
            return 0;
        }

        buffer[bytes_read] = '\0';

        printf("Received message from client:\n%s\n", buffer);

        int result;

        if (looks_like_http_request(buffer)) {
            result = handle_http_request(client_fd, buffer);
        } else {
            result = handle_plain_text_request(client_fd, buffer);
        }

        if (result == 1) {
            remove_client_from_all_topics(client_fd);
            return 1;
        }

        if (result == 2) {
            remove_client_from_all_topics(client_fd);
            return 0;
        }
    }
}