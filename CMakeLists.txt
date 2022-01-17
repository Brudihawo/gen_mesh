cmake_minimum_required(VERSION 3.10)
project(gen_mesh)

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
add_subdirectory(${SRC_DIR})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests ${CMAKE_CURRENT_LIST_DIR}/tests/exes)

