set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

cmake_minimum_required(VERSION 3.20)
project(opentitus)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_subdirectory(opl)

set(TITUS_SOURCES
    opentitus.cpp
    src/audio.cpp
    src/backbuffer.cpp
    src/common.cpp
    src/draw.cpp
    src/enemies.cpp
    src/engine.cpp
    src/elevators.cpp
    src/fonts.cpp
    src/gates.cpp
    src/globals.cpp
    src/keyboard.cpp
    src/level.cpp
    src/menu.cpp
    src/objects.cpp
    src/original.cpp
    src/player.cpp
    src/reset.cpp
    src/scroll.cpp
    src/settings.cpp
    src/sprites.cpp
    src/sqz.cpp
    src/tituserror.cpp
    src/tile_animation.cpp
    src/viewimage.cpp
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

add_executable(opentitus ${TITUS_SOURCES})
target_include_directories(opentitus PRIVATE "src")
target_link_libraries(opentitus m opl SDL2::SDL2main SDL2::SDL2 SDL2::mixer)
if(MSVC)
    target_compile_options(opentitus PRIVATE "/W4")
else()
    target_compile_options(opentitus PRIVATE "-Wall")
endif()

install(PROGRAMS $<TARGET_FILE:opentitus> DESTINATION "titus")
install(PROGRAMS $<TARGET_FILE:opentitus> DESTINATION "moktar" RENAME "openmoktar${CMAKE_EXECUTABLE_SUFFIX}")
