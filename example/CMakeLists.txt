cmake_minimum_required(VERSION 3.13)
project(httws_example C)

set(CMAKE_C_STANDARD 99)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)


add_executable(httws_example example.c)

target_link_libraries(httws_example httws)
