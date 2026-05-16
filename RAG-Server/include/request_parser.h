#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H

typedef struct {
    char method[8];
    char path[256];
    char version[16];
} HttpRequest;

int parse_http_request(const char* raw_request, HttpRequest* request);

#endif
