cmake_minimum_required(VERSION 3.10)
project(ThreadPool VERSION 1.0.0)

# Option to enable/disable tests
option(BUILD_TESTS "Build tests" OFF)

# Add the library and its source files
add_library(thread_pool
    src/task_queue.c
    src/thread_pool.c
)

# Specify the include directories for the library
target_include_directories(thread_pool PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  # For the build tree
    $<INSTALL_INTERFACE:include>                            # For the install tree
)

# Create export target for the library
install(TARGETS thread_pool
    EXPORT thread_poolTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install the headers
install(DIRECTORY include/ DESTINATION include)

# Generate and install the package configuration files
install(EXPORT thread_poolTargets
    FILE thread_poolTargets.cmake
    NAMESPACE ThreadPool::
    DESTINATION lib/cmake/thread_pool
)

# Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/thread_poolConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Create a Config.cmake file
configure_file(ThreadPoolConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/thread_poolConfig.cmake"
    @ONLY
)

# Install the Config and ConfigVersion files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/thread_poolConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/thread_poolConfigVersion.cmake"
    DESTINATION lib/cmake/thread_pool
)

# Add tests if enabled
if(BUILD_TESTS)
    enable_testing()
    # Set output directory for test executables
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

    add_subdirectory(src/tests)
endif()
