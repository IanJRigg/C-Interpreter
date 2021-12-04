cmake_minimum_required(VERSION 3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Application Configuration
###############################################################################
set(MAIN_EXECTUABLE_NAME interpreter)

set(SOURCE_FILES
    main.cpp
    interpreter.cpp
)

set(HEADER_FILES
    interpreter.h
)

add_executable(
    ${MAIN_EXECTUABLE_NAME}
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

set_target_properties(
    ${MAIN_EXECTUABLE_NAME}
    PROPERTIES
        # C++ version information
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO

        # Public Header file location
        PUBLIC_HEADER "${HEADER_FILES}"
)

target_compile_options(
    ${MAIN_EXECTUABLE_NAME}
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_include_directories(
    ${MAIN_EXECTUABLE_NAME}
    PRIVATE
        ${SRC_DIR}
)

target_link_libraries(
    ${MAIN_EXECTUABLE_NAME}
    PUBLIC
)