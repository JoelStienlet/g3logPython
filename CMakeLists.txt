cmake_minimum_required(VERSION 3.9.0)

enable_language(CXX)

set(g3logger_DIR "/usr/local/lib64/cmake/g3logger")

message( "====== Install Prefix ==========" )
message("${CMAKE_INSTALL_PREFIX}")
message( "================================" )

find_package(g3logger REQUIRED)
find_library(ROTATE g3logrotate)
find_library(SYSLOG g3log_syslog)

project(g3logBindings)

add_library(g3logBindings 
    SHARED 
    g3logBindings/ColorTermSink.cpp 
    g3logBindings/log.cpp
    g3logBindings/sinks.cpp
    g3logBindings/store.cpp
    g3logBindings/worker.cpp
    )
    
set(PUB_HEADER_FILES "g3logBindings/g3logBindings.h" "g3logBindings/ColorTermSink.h" )
set_target_properties(g3logBindings PROPERTIES PUBLIC_HEADER "${PUB_HEADER_FILES}" )

target_include_directories(g3logBindings 
    PRIVATE g3logBindings
    INTERFACE 
        $<BUILD_INTERFACE:{CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}>
    )

target_link_libraries(g3logBindings PRIVATE ${ROTATE})
target_link_libraries(g3logBindings PRIVATE ${SYSLOG})

install(TARGETS g3logBindings
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include/g3logBindings"
    )
