cmake_minimum_required(VERSION 3.9.0)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message( "====== Install Prefix ==========" )
message("${CMAKE_INSTALL_PREFIX}")
message( "================================" )

find_package(g3logger REQUIRED 
             PATHS /usr/local/lib64 /usr/local/lib )
# TODO: improve cmake interoperability
# find_package(g3logrotate REQUIRED
#              NAMES g3logrotate g3sinks )
find_library(ROTATE g3logrotate)
# find_package(g3log_syslog REQUIRED )
find_library(SYSLOG g3log_syslog)

project(g3logBindings)

add_library(g3logBindings 
    SHARED 
    g3logBindings/ColorTermSink.cpp 
    g3logBindings/log.cpp
    g3logBindings/sinks.cpp
    g3logBindings/store.cpp
    g3logBindings/fd_listener.cpp
    g3logBindings/worker.cpp
    )
    
# https://github.com/codecov/example-cpp11-cmake/blob/master/CMakeLists.txt
option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(CODE_COVERAGE)
  message( "Coverage enabled in CMake" )
  # Add required flags (GCC & LLVM/Clang)
  target_compile_options(g3logBindings PUBLIC
    -O0        # no optimization
    -g         # generate debug info
    --coverage # sets all required flags
  )
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(g3logBindings PUBLIC --coverage)
  else()
    target_link_libraries(g3logBindings PUBLIC --coverage)
  endif()
endif(CODE_COVERAGE)
    
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
