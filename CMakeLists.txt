cmake_minimum_required(VERSION 3.25)

project(Rediy VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Rediy
    src/main.cpp
)

set_target_properties(Rediy PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_compile_options(Rediy PRIVATE
    -Wall -Wextra -Wpedantic -Wconversion -Wshadow
)

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

    add_executable(Rediy_tests
        tests/smoke_test.cpp
    )

    set_target_properties(Rediy_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    target_link_libraries(Rediy_tests PRIVATE
        GTest::gtest_main
    )

    target_compile_options(Rediy_tests PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow
    )

    include(GoogleTest)
    gtest_discover_tests(Rediy_tests)
endif()
