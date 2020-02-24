cmake_minimum_required(VERSION 3.10)

project(blifdot VERSION 0.01)

configure_file(src/c_make_header.h.in src/c_make_header.h)

add_subdirectory(third_party)

#find_package(Boost 1.56 REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})

add_executable(blifdot
                src/main.cc
                src/blif_ingest.cc)

target_include_directories(blifdot PUBLIC
                           "${PROJECT_BINARY_DIR}/src"
                           )

target_link_libraries(blifdot 
                        libblifparse
                        ${Boost_LIBRARIES})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)
