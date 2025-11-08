cmake_minimum_required(VERSION 3.18)
project(pqxx_ext_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory(thirdparty/libpqxx build-pqxx)
add_subdirectory(thirdparty/googletest build-gtest)
add_subdirectory(pqxx_ext)

enable_testing()

add_executable(pqxx_ext_test)

add_subdirectory(test)

target_link_libraries(pqxx_ext_test
    PRIVATE
    GTest::gtest_main
    pqxx
    pqxx_ext
)

include(GoogleTest)
gtest_discover_tests(pqxx_ext_test)
