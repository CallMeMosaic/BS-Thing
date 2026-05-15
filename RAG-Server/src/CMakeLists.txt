cmake_minimum_required(VERSION 3.12)
project(BS_Thingy C)

set(CMAKE_C_STANDARD 99)



add_executable(BS_Thingy.exe server.c
        api.c
        ../include/server.h
        ../include/server_socket.h
        ../include/config.h
        ../include/client_handler.h
        server_socket.c
        client_handler.c
)
