cmake_minimum_required(VERSION 3.12)
project(BS_Thingy C)

set(CMAKE_C_STANDARD 99)

add_executable(BS_Thingy.exe server.c
        api.c
        request_parser.c
        response.c
        server.h)
