# Some compilers warn that the return value of cr_assert gets converted from an
# int to a unsigned char:1. To avoid this error in those compilers, turn off
# conversion warnings for test code.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-conversion -Wno-int-conversion")

list(APPEND CMAKE_CTEST_ARGUMENTS --verbose)

add_executable(test_util test_util.c)
target_link_libraries(test_util
   PRIVATE util
   PUBLIC ${CRITERION}
)
add_test(
   NAME test_util
   COMMAND test_util ${CRITERION_FLAGS}
)

add_executable(test_server test_server.c)
target_link_libraries(test_server
   PRIVATE server
   PUBLIC ${CRITERION}
)
add_test(
   NAME test_server
   COMMAND test_server ${CRITERION_FLAGS}
)

add_executable(test_client test_client.c)
target_link_libraries(test_client
   PRIVATE client
   PUBLIC ${CRITERION}
)
add_test(
   NAME test_client
   COMMAND test_client ${CRITERION_FLAGS}
)
