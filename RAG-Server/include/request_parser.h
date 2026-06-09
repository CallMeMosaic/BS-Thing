#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H

typedef struct {
    char method[8];
    char path[256];
    char version[16];

    char content_type[128];
    int content_length;
    char body[4096];
} HttpRequest;

int parse_http_request(const char* raw_request, HttpRequest* request);

typedef enum {
    CMD_PING,
    CMD_ECHO,
    CMD_HELP,
    CMD_QUIT,
    CMD_SHUTDOWN,

    REQ_GET,
    REQ_POST,
    REQ_PUT,
    
    CMD_MESSAGE,
    CMD_UNKNOWN,
} CommandType;

typedef struct {
    CommandType type;
    char *path;
    char *body;
    char *argument;
} Request;

Request parse_request(char *input);

#endif
