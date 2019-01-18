cmake_minimum_required(VERSION 3.13)
project(httws VERSION 1.0.0 DESCRIPTION "Simple http and websocket client")

set(CMAKE_C_STANDARD 99)

include_directories(/usr/local/include)
link_directories(/usr/local/opt/openssl/lib)

add_library(httws SHARED
        src/body.c
        src/header.c
        src/request.c
        src/response.c
        src/http.c
        src/websocket.c
        src/httws.c
        )

set_target_properties(httws PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(httws PROPERTIES SOVERSION 1)

set_target_properties(httws PROPERTIES PUBLIC_HEADER "include/http.h;include/websocket.h;include/httws.h;include/body.h;include/header.h;include/request.h;include/response.h")


target_include_directories(httws PRIVATE include)
target_link_libraries(httws ssl crypto)

include(GNUInstallDirs)

install(TARGETS httws
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/httws)




