cmake_minimum_required(VERSION 2.8)
project(r64fx_base)

add_library(r64fx_base
    SHARED
    AudioEngine.cpp
    JackAudioEngine.cpp
    dsp.cpp
)

target_link_libraries(r64fx_base jack)

