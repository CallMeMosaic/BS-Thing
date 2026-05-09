cmake_minimum_required(VERSION 3.7.2)
project(BS_Thingy C)

set(CMAKE_C_STANDARD 99)

add_executable(BS_Thingy main.c
        RAG-Server/src/main.c
        RAG-Server/src/api.c
        RAG-Server/src/request_parser.c
)
