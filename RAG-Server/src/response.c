//eigene header-datei mit funktionsdefinition einbinden
#include "../include/response.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* create_http_response(
    int status_code,
    const char* content_type,
    const char* body
    ) {

    // enthält http status text
    const char* status_text;

    // text für jeweiligen statuscode ausgewählt
    switch (status_code) {
        case 200:
            status_text = "OK";
            break;

        case 404:
            status_text = "Not Found";
            break;

        case 500:
            status_text = "Internal Server Error";
            break;

    case 201:
        status_text = "Created";
        break;

        default:
            status_text = "Unknown";
            break;
    }

    // länge von http-body berechnen
    int body_length = strlen(body);

    // speicher für http-antwort reservieren
    char* response = malloc(1024 + body_length);

    // prüfen ob speicher reserviert wurde
    if (response == NULL) {
        return NULL;
    }

    // http response
    sprintf(
        response,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s",
        status_code,
        status_text,
        content_type,
        body_length,
        body
    );

    // fertige http antwort rückgabe
    return response;
}
