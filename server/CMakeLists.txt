project(server)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(util util.c util.h)

add_executable(server server.c)
target_link_libraries(server PRIVATE util)

