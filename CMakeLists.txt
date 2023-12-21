cmake_minimum_required(VERSION 3.16)
project(code)

set(CMAKE_CXX_STANDARD 17)

add_executable(code
        src/blockchain.cpp
        src/blockchain.h
        src/main.cpp
        src/usersystem.h
        src/usersystem.cpp
        src/log.cpp
        src/log.h
        src/booksystem.h
        src/booksystem.cpp
)
