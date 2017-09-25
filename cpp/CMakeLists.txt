if (DEFINED ENV{CMAKE_BUILD_TYPE})
	set(CMAKE_BUILD_TYPE $ENV{CMAKE_BUILD_TYPE})
elseif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE "Release" CACHE
		STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

project (SK3)

cmake_minimum_required (VERSION 3.0)

find_path (YAML_INCLUDE yaml-cpp)
include_directories (${YAML_INCLUDE})

find_library (YAML_LIB yaml-cpp)
link_libraries (${YAML_LIB})

file(GLOB_RECURSE SIM_SOURCES src/*.cpp)

add_executable (simulate ${SIM_SOURCES})

set_source_files_properties(cmake PROPERTIES GENERATED true)
add_custom_target(cmake
        COMMAND rm -rf CMakeFiles CMakeCache.txt Makefile cmake_install.cmake \; cmake .
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

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
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

