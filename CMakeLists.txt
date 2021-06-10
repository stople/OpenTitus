project(opentitus LANGUAGES C)

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

find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)

add_library(SDL INTERFACE)
target_link_libraries(SDL INTERFACE ${SDL_LIBRARIES} ${SDL_MIXER_LIBRARIES})
target_include_directories(SDL INTERFACE ${SDL_INCLUDE_DIRS} ${SDL_MIXER_INCLUDE_DIRS})

add_library(opl INTERFACE)
target_link_libraries(opl INTERFACE "${CMAKE_SOURCE_DIR}/opl/libopl_linux.a")
target_include_directories(opl INTERFACE "opl")

add_executable(opentitus ${TITUS_SOURCES})
set_property(TARGET opentitus PROPERTY POSITION_INDEPENDENT_CODE FALSE)
target_compile_options(opentitus PRIVATE "-no-pie")
target_link_options(opentitus PRIVATE "-no-pie")
target_include_directories(opentitus PRIVATE "src")
target_link_libraries(opentitus m SDL opl)
install(TARGETS opentitus DESTINATION ".")
