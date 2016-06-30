cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)

project(kulki)

include_directories(
    ./xeno
    ./model
    ./gui
    ./states
    .
    )

link_directories(
    ./build
    )

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -g -O0 -DDICK_LOG=4")

add_executable(kulki
    ./model/high_score.cpp
    ./model/config.cpp
    ./states/kulki_constants.cpp
    ./states/kulki_variables.cpp
    ./states/high_score_display_state.cpp
    ./states/high_score_input_state.cpp
    ./states/game_over_state.cpp
    ./states/wait_dest_state.cpp
    ./states/move_state.cpp
    ./states/wait_ball_state.cpp
    ./states/kulki_context.cpp
    ./states/score_state.cpp
    ./states/menu_state.cpp
    ./states/options_state.cpp
    ./states/deal_state.cpp
    ./main.cpp
    )

target_link_libraries(kulki m allegro_monolith dick moon)
