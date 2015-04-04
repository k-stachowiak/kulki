cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)

project(kulki)

include_directories(
    ./xeno
    .
    )

link_directories(
    ./build
    )

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -g -O0")

add_executable(kulki
    ./main.cpp
    ./high_score.cpp
    ./kulki_states.cpp
    ./config.cpp
)

target_link_libraries(kulki m allegro_monolith moon lang)
