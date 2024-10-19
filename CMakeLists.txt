cmake_minimum_required(VERSION 3.26)
project(tictactoe)

set(CMAKE_C_STANDARD 17)

set(RAYLIB_VERSION 5.0)

# STATIC stuff (Windows)
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(BUILD_FOR_WIN TRUE)
endif()
option(STATIC_BUILD "Build a static binary." ${BUILD_FOR_WIN})

if (STATIC_BUILD)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" CONFIG)
    set(BUILD_SHARED_LIBS OFF)
endif()

include (FindPkgConfig)
include (FindSDL_net)

find_package(raylib ${RAYLIB_VERSION} REQUIRED)
pkg_search_module (SDL_NET REQUIRED sdl2_net)

include_directories (${SDL_NET_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} client.c
        tictactoe.c
        tictactoe.h
        net.c
        net.h
        util.c
        util.h)
add_executable(ttt-server server.c
        net.c
        net.h
        util.c
        util.h
        serverutils.c
        serverutils.h)

target_link_libraries(${PROJECT_NAME} raylib ${SDL_NET_LIBRARIES})
target_link_libraries(ttt-server ${SDL_NET_LIBRARIES})

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()