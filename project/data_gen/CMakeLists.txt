add_executable(datagen main.cc)
target_link_libraries(datagen PRIVATE project_lib)
target_include_directories(datagen PRIVATE ${CMAKE_BINARY_DIR}/datagen)
