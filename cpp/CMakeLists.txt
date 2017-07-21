project (SK3)

cmake_minimum_required (VERSION 3.0)

find_path (YAML_INCLUDE yaml-cpp)
include_directories (${YAML_INCLUDE})

find_library (YAML_LIB yaml-cpp)
link_libraries (${YAML_LIB})

file(GLOB_RECURSE SIM_SOURCES src/*.cpp)

add_executable (simulate ${SIM_SOURCES})

# Add a top-level "tags" target which includes all files in both
# the build and source versions of src/*.
set_source_files_properties(tags PROPERTIES GENERATED true)
add_custom_target(tags
        DEPENDS ${SIM_SOURCES}
        COMMAND ctags -R --c++-kinds=+p --fields=+iaS --extra=+q
        ${SIM_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# ...but only make it a dependency of the project if the ctags program
# is available, else it will fail to build on Windows.
find_program(CTAGS_PATH ctags)
if(CTAGS_PATH)
        message(STATUS "Found ctags: ${CTAGS_PATH}")
        add_dependencies(simulate tags)
endif(CTAGS_PATH)

find_program(DOXYGEN_PATH doxygen)
if(DOXYGEN_PATH)
        message(STATUS "Found doxygen: ${DOXYGEN_PATH}")
        # Add a top-level "docs" target to run doxygen
        set_source_files_properties(docs PROPERTIES GENERATED true)
        add_custom_target(docs
                DEPENDS ${SIM_SOURCES}
                COMMAND doxygen Doxyfile
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif(DOXYGEN_PATH)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

