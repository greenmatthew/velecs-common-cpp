cmake_minimum_required(VERSION 3.14)
project(velecs-common VERSION 0.1.0)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories to bin
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add external dependencies
add_subdirectory(libs)

# Source files for the library
set(LIB_SOURCES
    src/Uuid.cpp
)

# Header files for the library (for IDE organization)
set(LIB_HEADERS
    include/velecs/common/Uuid.hpp
)

# Always build the library
add_library(velecs-common ${LIB_SOURCES} ${LIB_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_SOURCES} ${LIB_HEADERS})

target_include_directories(velecs-common
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(velecs-common
    PUBLIC stduuid
)

if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # We're being included as a submodule
    set(VELECS_COMMON_LIBRARIES velecs-common PARENT_SCOPE)
endif()