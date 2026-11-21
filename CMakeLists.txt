cmake_minimum_required(VERSION 3.25)

project(RedisDiy VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(RedisDiy_core STATIC
    src/core.cpp
)

target_include_directories(RedisDiy_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(RedisDiy
    src/main.cpp
)

target_link_libraries(RedisDiy PRIVATE RedisDiy_core)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    foreach(target RedisDiy_core RedisDiy)
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        )
    endforeach()
endif()

include(CTest)

if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(googletest)

    add_executable(RedisDiy_tests
        tests/smoke_test.cpp
    )

    target_link_libraries(RedisDiy_tests PRIVATE
        RedisDiy_core
        GTest::gtest_main
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(RedisDiy_tests PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        )
    endif()

    include(GoogleTest)
    gtest_discover_tests(RedisDiy_tests)
endif()