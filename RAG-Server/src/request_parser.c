#include "../include/request_parser.h"

#include <stdio.h>
#include <string.h>

//parses raw http request string and extracts
//method, path and http version
//returns 1 if request is valid, if not valid returns 0
int parse_http_request(const char* raw_request, HttpRequest* request) {
    if (sscanf(raw_request, "%7s %255s %15s",
        request->method,
        request->path,
        request->version) != 3) {
        return 0;
    }

    if (strcmp(request->version, "HTTP/1.1") != 0) {
        return 0;
    }

    if (strcmp(request->method, "GET") != 0 &&
        strcmp(request->method, "POST") != 0) {
        return 0;
    }

    return 1;
}