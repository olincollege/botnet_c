project(server)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(server_functions server_functions.c server_functions.h)

add_executable(server server.c)
target_include_directories(server PRIVATE ${INCLUDE_DIRS})

target_link_libraries(server PRIVATE util)
target_link_libraries(server PRIVATE server_functions)

