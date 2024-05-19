# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(ThreadPool VERSION 1.0)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Define the source files for the library
set(SOURCES
    src/task_queue.c
    src/thread_pool.c
)


# Add the library
add_library(thread_pool_lib ${SOURCES})

# Include directories for the library
target_include_directories(thread_pool_lib PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)

# Optionally, add compiler warnings
if (MSVC)
    target_compile_options(thread_pool_lib PRIVATE /W4 /WX)
else()
    target_compile_options(thread_pool_lib PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Enable testing
enable_testing()

# Set output directory for test executables
set(TEST_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

# Automatically find and add test executables
file(GLOB TEST_SOURCES "src/tests/*.c")
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE thread_pool_lib)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_OUTPUT_DIRECTORY}/${TEST_NAME})
endforeach()