add_library(
  utils
  ${CMAKE_CURRENT_LIST_DIR}/datastructs.c
  ${CMAKE_CURRENT_LIST_DIR}/mesh.c
  ${CMAKE_CURRENT_LIST_DIR}/qtree.c
)

add_library(
  logging
  ${CMAKE_CURRENT_LIST_DIR}/logging.c
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set_target_properties(logging PROPERTIES COMPILE_FLAGS "-DVERB_LEVEL=VERB_ERR")
endif()

add_executable(gen_mesh ${CMAKE_CURRENT_LIST_DIR}/main.c)

target_link_libraries(gen_mesh m utils logging)

# SDL2
find_package(SDL2 REQUIRED)
target_include_directories(gen_mesh PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(gen_mesh ${SDL2_LIBRARIES})
