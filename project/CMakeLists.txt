cmake_minimum_required(VERSION 3.28)
project(RedHotHouseFinder LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Fetch SFML
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Add stricter compiler flags to catch errors quicker.
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(
        -Wall        # Enable all common warnings
        -Wextra      # Enable extra warnings
        -Wpedantic   # Issue warnings for strict ISO C/C++
        -Wunused     # Warn about unused variables/functions
    )
endif()

# Create library from src (excluding main.cc)
file(GLOB_RECURSE SRC_FILES "src/*.cc")
list(REMOVE_ITEM SRC_FILES "${CMAKE_SOURCE_DIR}/src/main.cc")

add_library(project_lib STATIC ${SRC_FILES})

target_include_directories(project_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${SFML_INCLUDE_DIRS}  # Propagate SFML includes
)
target_link_libraries(project_lib PUBLIC SFML::Graphics)  # Changed to PUBLIC

# Main executable
add_executable(main src/main.cc)
target_link_libraries(main PRIVATE project_lib)

# Hold test executables
set(TEST_EXECUTABLES "")

# Add tests subdirectory
add_subdirectory(tests)

# Add test runner subdirectory
add_subdirectory(test_runner)

# Add data generation subdirectory
add_subdirectory(data_gen)

# Custom target to run tests
add_custom_target(run-tests
    COMMAND test_runner
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    DEPENDS test_runner ${TEST_EXECUTABLES}
    USES_TERMINAL
)
