set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

cmake_minimum_required(VERSION 3.20)
project(opentitus)

find_package(SDL2 2.0.7)
find_package(SDL2_mixer 2.0.2)

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

add_executable(openmoktar ${TITUS_SOURCES})
target_include_directories(openmoktar PRIVATE "src")
target_link_libraries(openmoktar m opl SDL2::SDL2main SDL2::SDL2 SDL2::mixer)
if(MSVC)
    target_compile_options(openmoktar PRIVATE "/W4")
else()
    target_compile_options(openmoktar PRIVATE "-Wall")
endif()

install(TARGETS opentitus DESTINATION "titus")
install(TARGETS openmoktar DESTINATION "moktar")
