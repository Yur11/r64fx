cmake_minimum_required(VERSION 2.8)
project(r64fx_base)

add_library(r64fx_base
    SHARED
    AudioEngine.cpp
    JackAudioEngine.cpp
    dsp.cpp
)
# add_executable(r64fx_base_example example.cpp)

target_link_libraries(r64fx_base jack)

# target_link_libraries(r64fx_base_example r64fx)

install(TARGETS r64fx_base DESTINATION lib)
# install(TARGETS r64fx_base_example DESTINATION bin)
