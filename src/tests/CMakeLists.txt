# Define the test executable
add_executable(print_id_test print_id_test.c)

# Link the test executable against gtest and the library
target_link_libraries(print_id_test pthread thread_pool)

# Add test to CTest
add_test(NAME print_id_test COMMAND print_id_test)
