#r64fx
cmake_minimum_required(VERSION 2.8)
project(r64fx)

set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -g")


add_definitions(
    -DDEBUG
    -DUSE_SDL2
)

include_directories(
    /usr/include/freetype2 
    ${CMAKE_SOURCE_DIR}/utf8string
    ${CMAKE_SOURCE_DIR}/json
    ${CMAKE_SOURCE_DIR}/jit
    ${CMAKE_SOURCE_DIR}/base
    ${CMAKE_SOURCE_DIR}/gui
)

link_directories(
    ${CMAKE_BINARY_DIR}/utf8string
    ${CMAKE_BINARY_DIR}/json
    ${CMAKE_BINARY_DIR}/base
    ${CMAKE_BINARY_DIR}/gui
)

add_subdirectory(utf8string)
add_subdirectory(json)
add_subdirectory(base)
add_subdirectory(gui)
add_subdirectory(main)


# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)