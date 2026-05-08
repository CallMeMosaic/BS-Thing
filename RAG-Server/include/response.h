#ifndef RESPONSE_H
#define RESPONSE_H

char* create_http_response(
    int status_code,
    const char* content_type,
    const char* body
);

#endif
