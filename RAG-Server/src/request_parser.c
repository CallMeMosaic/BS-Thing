//including own .h file
#include "../include/request_parser.h"

#include <stdio.h>
#include <string.h>

// parses raw http request and stores method, path and http version in struct
int parse_http_request(const char* raw_request, HttpRequest* request) {
    if (sscanf(raw_request, "%7s %255s %15s",
        request->method,
        request->path,
        request->version) != 3) {

        // invalid request
        return 0;
    }
    //only supports http/1.1 requests
    if (strcmp(request->version, "HTTP/1.1") != 0) {
        return 0;
    }
    //allows supported http methods
    if (strcmp(request->method, "GET") != 0 &&
        strcmp(request->method, "POST") != 0) {
        return 0;
    }
    // parsed successfully
    return 1;
}