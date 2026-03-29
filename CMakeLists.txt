cmake_minimum_required(VERSION 3.16)
project(ts-mini LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tslib
    src/ts_packet.cpp
    src/packetizer.cpp
    src/parser.cpp
)
target_include_directories(tslib PUBLIC include)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable(ts-mini src/main.cpp)
target_link_libraries(ts-mini PRIVATE tslib)

enable_testing()
add_executable(ts_tests test/test_ts.cpp)
target_link_libraries(ts_tests PRIVATE tslib GTest::gtest_main)