#r64fx_main
cmake_minimum_required(VERSION 2.8)
project(r64fx_main)

include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/gui)

add_executable(r64fx
    main.cpp
)

link_directories(${CMAKE_BINARY_DIR}/base)
link_directories(${CMAKE_BINARY_DIR}/gui)

target_link_libraries(r64fx
    r64fx_base
    r64fx_gui
)

install(TARGETS r64fx DESTINATION bin)
