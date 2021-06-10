set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

cmake_minimum_required(VERSION 3.20)
project(opentitus LANGUAGES C)

find_package(SDL2 2.0.7)
find_package(SDL2_mixer 2.0.2)

add_subdirectory(opl)

set(TITUS_SOURCES
    opentitus.c
    src/audio.c
    src/backbuffer.c
    src/common.c
    src/draw.c
    src/enemies.c
    src/engine.c
    src/elevators.c
    src/fonts.c
    src/gates.c
    src/globals.c
    src/keyboard.c
    src/level.c
    src/menu.c
    src/objects.c
    src/original.c
    src/player.c
    src/reset.c
    src/scroll.c
    src/settings.c
    src/sprites.c
    src/sqz.c
    src/tituserror.c
    src/tile_animation.c
    src/viewimage.c
)

add_executable(opentitus ${TITUS_SOURCES})
set_property(TARGET opentitus PROPERTY POSITION_INDEPENDENT_CODE FALSE)
target_include_directories(opentitus PRIVATE "src")
target_link_libraries(opentitus m opl SDL2::SDL2main SDL2::SDL2 SDL2::mixer)
install(TARGETS opentitus DESTINATION ".")
