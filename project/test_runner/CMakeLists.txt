add_executable(test_runner main.cc)
target_link_libraries(test_runner PRIVATE project_lib)
target_include_directories(test_runner PRIVATE ${CMAKE_BINARY_DIR}/test_runner)
