cmake_minimum_required(VERSION 3.20)
project(ClienteSistema C)

set(CMAKE_C_STANDARD 99)
link_libraries(crypto)
link_libraries(pthread)
add_executable(ClienteSistema main.c)
