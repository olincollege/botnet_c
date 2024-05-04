project(auto_client)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(util util.c util.h)

add_executable(auto_client auto_client.c)
target_link_libraries(auto_client PRIVATE util)

