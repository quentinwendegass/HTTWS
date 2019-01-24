cmake_minimum_required(VERSION 3.13)
project(httws_example C)

set(CMAKE_C_STANDARD 99)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_custom_target(http)
add_custom_target(websocket)

add_executable(http_example http_test.c)
add_executable(websocket_example websocket_test.c)

target_link_libraries(http_example httws)
target_link_libraries(websocket_example httws)

add_dependencies(http http_example)
add_dependencies(websocket websocket_example)


