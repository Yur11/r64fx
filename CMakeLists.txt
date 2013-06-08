#r64fx
cmake_minimum_required(VERSION 2.8)
project(r64fx)

set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -g")

include_directories(/usr/include/freetype2)

add_definitions(
    -DDEBUG
    -DUSE_SDL2
)

add_subdirectory(base)
add_subdirectory(gui)
add_subdirectory(main)


# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)