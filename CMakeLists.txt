cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)

project(kulki)

include_directories(
    ./xeno
    ./states
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
    ./config.cpp
    ./states/menu_state.cpp
    ./states/deal_state.cpp
    ./states/wait_ball_state.cpp
    ./states/wait_dest_state.cpp
    ./states/move_state.cpp
    ./states/score_state.cpp
    ./states/high_score_state.cpp
    ./states/game_over_state.cpp
)

target_link_libraries(kulki m allegro_monolith moon)
