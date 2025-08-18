cmake_minimum_required(VERSION 3.10)

project(caro)

add_executable(caro
    src/main.c
    src/application.h
    src/application.c
    src/std_include.h
    src/pgn.h
    src/pgn.c
    src/common.h
    src/common.c
    src/chess.h
    src/chess.c
)

add_subdirectory(vendor/SDL)
add_subdirectory(vendor/SDL_image)

target_include_directories(caro PRIVATE vendor/SDL/include/)
target_include_directories(caro PRIVATE vendor/SDL_image/include)

target_link_libraries(
    caro
    PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
)

set(CMAKE_BUILD_TYPE Debug)
