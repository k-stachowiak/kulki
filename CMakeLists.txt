cmake_minimum_required(VERSION 2.8)

project(kulki)

include_directories(
    .
    )

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -g -O0")

add_executable(kulki
    ./main.cpp
)

target_link_libraries(kulki m allegro_monolith)
