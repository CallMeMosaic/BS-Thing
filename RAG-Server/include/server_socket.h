/*
 * Server Socket header containing the declarations for socket construction, client accepting, socket closing
 * Basically creates sockets, binds them, listens on them and accepts clients
 * Created by CallMeMosaic
 */

#ifndef BS_THINGY_SERVER_SOCKET_H
#define BS_THINGY_SERVER_SOCKET_H


int create_server_socket(int port);

int accept_client(int server_fd);

void close_socket(int socket_fd);

#endif //BS_THINGY_SERVER_SOCKET_H
