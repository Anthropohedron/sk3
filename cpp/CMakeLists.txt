project (SK3)

cmake_minimum_required (VERSION 3.0)

find_path (YAML_INCLUDE yaml-cpp)
include_directories (${YAML_INCLUDE})

find_library (YAML_LIB yaml-cpp)
link_libraries (${YAML_LIB})

file(GLOB_RECURSE SIM_SOURCES src/*.cpp)

add_executable (simulate ${SIM_SOURCES})
target_compile_options (simulate PUBLIC -std=c++11 -Wall)

