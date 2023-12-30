cmake_minimum_required(VERSION 3.14)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

project(sequence_view VERSION 0.1 LANGUAGES CXX)
option(DEVELOPER_BUILD "Enable developer mode" OFF)
option(SEQUENCE_VIEW_ENABLE_TESTS "Enable test build" ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
    _project_options
    URL https://github.com/aminya/project_options/archive/refs/tags/v0.33.0.zip
)
FetchContent_MakeAvailable(_project_options)

add_library(sequence_view INTERFACE)
target_sources(sequence_view INTERFACE
    include/sequence_view/common.hpp
)
target_include_directories(sequence_view INTERFACE include)

if(DEVELOPER_BUILD)
    set(ENABLE_CPPCHECK "ENABLE_CPPCHECK")
    if(NOT CXX_CPPCHECK)
        set(ENABLE_CPPCHECK "")
    endif()

    check_sanitizers_support(
        ENABLE_SANITIZER_ADDRESS
        ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
        ENABLE_SANITIZER_LEAK
        ENABLE_SANITIZER_THREAD
        ENABLE_SANITIZER_MEMORY
    )

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(ENABLE_GCC_ANALYZER "ENABLE_GCC_ANALYZER")
        set(GCC_WARNINGS "GCC_WARNINGS")
        set(ENABLE_COVERAGE "ENABLE_COVERAGE")
        set(ENABLE_CONTROL_FLOW_PROTECTION "ENABLE_CONTROL_FLOW_PROTECTION")
        set(ENABLE_STACK_PROTECTION "ENABLE_STACK_PROTECTION")
        set(ENABLE_OVERFLOW_PROTECTION "ENABLE_OVERFLOW_PROTECTION")
        set(WARNINGS_AS_ERRORS "WARNINGS_AS_ERRORS")
    else()
        set(ENABLE_GCC_ANALYZER "")
        set(GCC_WARNINGS "")
        set(ENABLE_SANITIZER_ADDRESS "")
        set(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "")
        set(ENABLE_SANITIZER_LEAK "")
        set(ENABLE_SANITIZER_THREAD "")
        set(ENABLE_SANITIZER_MEMORY "")
    endif()

    include(${_project_options_SOURCE_DIR}/Index.cmake)
    include(${_project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)
    project_options(
        PREFIX ${PROJECT_NAME}
        ENABLE_CACHE
        ${ENABLE_CPPCHECK}
        ${ENABLE_CLANG_TIDY}
        ${ENABLE_SANITIZER_ADDRESS}
        ${ENABLE_SANITIZER_LEAK}
        ${ENABLE_SANITIZER_UNDEFINED_BEHAVIOR}
        ${ENABLE_COVERAGE}
        ${ENABLE_CONTROL_FLOW_PROTECTION}
        ${ENABLE_STACK_PROTECTION}
        ${ENABLE_OVERFLOW_PROTECTION}
        ${WARNINGS_AS_ERRORS}
        ${ENABLE_GCC_ANALYZER}
        ${GCC_WARNINGS}
    )

    target_link_libraries(io_dev INTERFACE 
        ${PROJECT_NAME}_project_options 
        ${PROJECT_NAME}_project_warnings
    )
endif()

if(SEQUENCE_VIEW_ENABLE_TESTS)
    include(FetchContent)
    include(CTest)
    include(GoogleTest)

    enable_testing()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        58d77fa8070e8cec2dc1ed015d66b454c8d78850 # release-1.12.1
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(main 
        common.cc
    )

    find_package(fmt REQUIRED)

    target_link_libraries(main PRIVATE sequence_view gtest_main gmock fmt::fmt)
    gtest_discover_tests(main)
endif()