project(auto_client)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(client_functions client_functions.c client_functions.h)
add_library(util ../util/util.c ../util/util.h)

add_executable(auto_client auto_client.c)
target_include_directories(auto_client PRIVATE ${INCLUDE_DIRS})

target_link_libraries(auto_client PRIVATE client_functions)
target_link_libraries(auto_client PRIVATE util)

