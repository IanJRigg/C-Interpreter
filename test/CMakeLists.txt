cmake_minimum_required(VERSION 3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(TEST_RUNNER_NAME runner)

set(TEST_SOURCE_FILES
    runner.cpp
    interpreter-tests.cpp
    ../src/interpreter.cpp
)

set(TEST_HEADER_FILES
    constants.h
    ../src/interpreter.h
)

add_executable(
    ${TEST_RUNNER_NAME}
    ${TEST_SOURCE_FILES}
    ${TEST_HEADER_FILES}
)

set_target_properties(
    ${TEST_RUNNER_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(
    ${TEST_RUNNER_NAME}
    PRIVATE
        ../include/
)

target_link_libraries(
    ${TEST_RUNNER_NAME}
    PUBLIC
)
