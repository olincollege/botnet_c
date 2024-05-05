project(test_project)

# Some compilers warn that the return value of cr_assert gets converted from an
# int to a unsigned char:1. To avoid this error in those compilers, turn off
# conversion warnings for test code.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-conversion -Wno-int-conversion")

list(APPEND CMAKE_CTEST_ARGUMENTS --verbose)

# Add test executables
add_executable(test_util test_util.c)
add_executable(test_server test_server.c)
add_executable(test_client test_client.c)

# Link libraries to test executables
target_link_libraries(test_util PRIVATE util ${CRITERION})
target_link_libraries(test_server PRIVATE ${CRITERION})
target_link_libraries(test_client PRIVATE ${CRITERION})

# Define tests
add_test(NAME test_util COMMAND test_util ${CRITERION_FLAGS})
add_test(NAME test_server COMMAND test_server ${CRITERION_FLAGS})
add_test(NAME test_client COMMAND test_client ${CRITERION_FLAGS})
