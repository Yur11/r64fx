#r64fx serialize
add_library(r64fx_serialize SHARED serialize.cpp)
target_link_libraries(r64fx_serialize r64fx_json)