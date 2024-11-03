cmake_minimum_required(VERSION 3.26)
project(tictactoe)

set(CMAKE_C_STANDARD 17)

set(RAYLIB_VERSION 5.0)

set(SOURCES client.c
        tictactoe.c
        tictactoe.h
        net.c
        net.h
        util.c
        util.h)

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

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(BUNDLEINFO_PATH "${CMAKE_SOURCE_DIR}/macos/BundleInfo.plist.in")
    set(MACOSX_BUNDLE_ICON_FILE tictactoe_icon.icns)
    set(MACOSX_BUNDLE_INFO_STRING "TicTacToe 0.1 Copyright (c) 2024 mik3se")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2024 mik3se")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "0.1")
    set(tictactoe_ICON ${CMAKE_SOURCE_DIR}/macos/tictactoe_icon.icns)
    set_source_files_properties(${tictactoe_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${tictactoe_ICON} ${SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES
            BUNDLE True
            MACOSX_BUNDLE_GUI_IDENTIFIER net.xumoye.${PROJECT_NAME}
            MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
            MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
            MACOSX_BUNDLE_INFO_PLIST ${BUNDLEINFO_PATH})
else()
    add_executable(${PROJECT_NAME} ${SOURCES})
endif()

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