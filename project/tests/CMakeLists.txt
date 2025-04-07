file(GLOB TEST_SOURCES "*.cc")

# Add compilation for each test as an executable
foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)

    # Create an executable for each test
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE project_lib)
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
    )

    # Add the test executable to the list
    list(APPEND TEST_EXECUTABLES ${test_name})
endforeach()

# Propagate the list of test executables to the parent scope (for test runner to rebuild them)
set(TEST_EXECUTABLES ${TEST_EXECUTABLES} PARENT_SCOPE)

# Generate test list for test runner
set(TEST_LIST "")
foreach(test ${TEST_EXECUTABLES})
    string(APPEND TEST_LIST "    \"tests/${test}\",\n")
endforeach()

# Create a fake TestList.hh header with the list of tests.
configure_file(
    ${CMAKE_SOURCE_DIR}/test_runner/TestList.hh.in
    ${CMAKE_BINARY_DIR}/test_runner/TestList.hh
    @ONLY
)
