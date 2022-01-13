cmake_minimum_required(VERSION 3.10)
project(gen_mesh)

add_executable(gen_mesh main.c datastructs.c)

find_package(SDL2 REQUIRED)
target_include_directories(gen_mesh PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(gen_mesh ${SDL2_LIBRARIES})
