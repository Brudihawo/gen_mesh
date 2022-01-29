add_executable(
  test_qtree test_qtree.c
)

target_link_libraries(test_qtree utils logging m)
target_include_directories(test_qtree PUBLIC ${SRC_DIR}) 
