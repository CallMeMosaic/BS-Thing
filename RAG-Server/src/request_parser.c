/*
 * request_parser.c
 *
 * Parses incoming client messages.
 *
 * This module supports two request styles:
 *
 * 1. HTTP-style requests:
 *    Example:
 *    GET /health HTTP/1.1
 *
 * 2. Plain text command-style requests:
 *    Example:
 *    ping
 *    SUB news
 *    PUB news hello everyone
 *
 * The HTTP parser fills an HttpRequest struct.
 * The plain text parser fills a Request struct.
 */

#include "../include/request_parser.h"

#include <stdio.h>
#include <string.h>

/*
 * Parses a raw HTTP request.
 *
 * The function extracts:
 * - HTTP method
 * - path
 * - HTTP version
 * - Content-Type
 * - Content-Length
 * - body
 *
 * Returns:
 * 1 = valid supported HTTP request
 * 0 = invalid or unsupported HTTP request
 */
int parse_http_request(const char *raw_request, HttpRequest *request) {
    request->content_type[0] = '\0';
    request->content_length = 0;
    request->body[0] = '\0';

    if (sscanf(
            raw_request,
            "%7s %255s %15s",
            request->method,
            request->path,
            request->version
        ) != 3) {
        return 0;
    }

    if (strcmp(request->version, "HTTP/1.1") != 0 &&
        strcmp(request->version, "HTTP/1.0") != 0) {
        return 0;
    }

    if (strcmp(request->method, "GET") != 0 &&
        strcmp(request->method, "POST") != 0) {
        return 0;
    }

    const char *content_length_header = strstr(raw_request, "Content-Length:");

    if (content_length_header != NULL) {
        sscanf(content_length_header, "Content-Length: %d", &request->content_length);
    }

    const char *content_type_header = strstr(raw_request, "Content-Type:");

    if (content_type_header != NULL) {
        sscanf(content_type_header, "Content-Type: %127s", request->content_type);
    }

    const char *body_start = strstr(raw_request, "\r\n\r\n");

    if (body_start != NULL) {
        body_start += 4;

        if (request->content_length > 0) {
            int length = request->content_length;

            if (length > 4095) {
                length = 4095;
            }

            strncpy(request->body, body_start, length);
            request->body[length] = '\0';
        } else {
            strncpy(request->body, body_start, 4095);
            request->body[4095] = '\0';
        }
    }

    return 1;
}

/*
 * Removes trailing line endings from plain text input.
 *
 * ncat usually sends:
 * command\n
 *
 * Some clients may send:
 * command\r\n
 *
 * This helper normalizes both to:
 * command
 */
static void trim_line_endings(char *input) {
    size_t len = strlen(input);

    while (len > 0 &&
           (input[len - 1] == '\n' || input[len - 1] == '\r')) {
        input[len - 1] = '\0';
        len--;
    }
}

/*
 * Parses plain text commands.
 *
 * Supported examples:
 * - ping
 * - echo hello
 * - help
 * - quit
 * - shutdown
 * - GET /path
 * - POST /path body
 * - PUT /path body
 * - SUB topic
 * - UNSUB topic
 * - PUB topic message
 *
 * If the input does not match any command, it becomes CMD_MESSAGE.
 */
Request parse_request(char *input) {
    Request request;

    request.type = CMD_UNKNOWN;
    request.path = NULL;
    request.topic = NULL;
    request.body = NULL;
    request.argument = NULL;

    trim_line_endings(input);

    if (strcmp(input, "ping") == 0) {
        request.type = CMD_PING;
        return request;
    }

    if (strncmp(input, "echo ", 5) == 0) {
        request.type = CMD_ECHO;
        request.argument = input + 5;
        return request;
    }

    if (strcmp(input, "help") == 0) {
        request.type = CMD_HELP;
        return request;
    }

    if (strcmp(input, "quit") == 0) {
        request.type = CMD_QUIT;
        return request;
    }

    if (strcmp(input, "shutdown") == 0 ||
        strcmp(input, "exit") == 0) {
        request.type = CMD_SHUTDOWN;
        return request;
    }

    if (strncmp(input, "GET ", 4) == 0) {
        request.type = REQ_GET;
        request.path = input + 4;
        return request;
    }

    if (strncmp(input, "POST ", 5) == 0) {
        request.type = REQ_POST;

        char *space_after_path = strchr(input + 5, ' ');

        if (space_after_path != NULL) {
            *space_after_path = '\0';
            request.path = input + 5;
            request.body = space_after_path + 1;
        } else {
            request.path = input + 5;
            request.body = NULL;
        }

        return request;
    }

    if (strncmp(input, "PUT ", 4) == 0) {
        request.type = REQ_PUT;

        char *space_after_path = strchr(input + 4, ' ');

        if (space_after_path != NULL) {
            *space_after_path = '\0';
            request.path = input + 4;
            request.body = space_after_path + 1;
        } else {
            request.path = input + 4;
            request.body = NULL;
        }

        return request;
    }

    if (strncmp(input, "SUB ", 4) == 0) {
        request.type = REQ_SUB;
        request.topic = input + 4;
        return request;
    }

    if (strncmp(input, "UNSUB ", 6) == 0) {
        request.type = REQ_UNSUB;
        request.topic = input + 6;
        return request;
    }

    if (strncmp(input, "PUB ", 4) == 0) {
        request.type = REQ_PUB;

        char *space_after_topic = strchr(input + 4, ' ');

        if (space_after_topic != NULL) {
            *space_after_topic = '\0';
            request.topic = input + 4;
            request.body = space_after_topic + 1;
        } else {
            request.topic = input + 4;
            request.body = "";
        }

        return request;
    }

    request.type = CMD_MESSAGE;
    request.argument = input;

    return request;
}