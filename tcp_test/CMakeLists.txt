cmake_minimum_required(VERSION 3.20)
project(tcp_test)
set (CMAKE_C_COMPILER "D:/Program Files/Mingw/bin/gcc.exe")
set (MAKE_CXX_COMPILER "D:/Program Files/Mingw/bin/g++.exe")

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

set(SOURCES
    src/main.cc
    src/client.cc
    src/server.cc
)

add_executable(tcp_test ${SOURCES})
target_link_libraries(tcp_test ws2_32)