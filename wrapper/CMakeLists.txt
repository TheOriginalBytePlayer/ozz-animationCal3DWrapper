cmake_minimum_required(VERSION 3.24)

# Cal3D wrapper library with ozz-animation backend
project(cal3d_wrapper)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/include/ozz
)

# Source files for the wrapper
set(WRAPPER_SOURCES
    src/cal3d_wrapper_impl.cpp
    src/core_model.cpp
    src/all_stubs.cpp
)

# Header files
set(WRAPPER_HEADERS
    include/cal3d_wrapper_internal.h
    ../cal3d_wrapper.h
)

# Create shared library
add_library(cal3d_wrapper SHARED ${WRAPPER_SOURCES} ${WRAPPER_HEADERS})

# Define CAL3D_WRAPPER_EXPORTS for the library build only
target_compile_definitions(cal3d_wrapper PRIVATE CAL3D_WRAPPER_EXPORTS)

# Link with ozz libraries
target_link_libraries(cal3d_wrapper
    ozz_animation
    ozz_base
    ozz_geometry
)

# Set library output name
set_target_properties(cal3d_wrapper PROPERTIES
    OUTPUT_NAME "cal3d"
    VERSION 0.1.0
    SOVERSION 0
)

# Installation
install(TARGETS cal3d_wrapper
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ../cal3d_wrapper.h
    DESTINATION include
)

# Optional: Build example program
option(BUILD_CAL3D_EXAMPLE "Build Cal3D wrapper example" OFF)  # Disabled by default due to C compatibility issues in header

if(BUILD_CAL3D_EXAMPLE)
    add_executable(cal3d_example example.c)
    target_link_libraries(cal3d_example cal3d_wrapper)
    target_include_directories(cal3d_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
endif()
