cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0016 NEW)

set(TEST_PROJECT_NAME test_proj)
set(CMAKE_CXX_STANDARD 14)

find_package(GTest REQUIRED)
find_package(Protobuf REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src ${GTEST_INCLUDE_DIRS})

set(TEST_ALL_SOURCES
    Test_DelimitedMessagesStreamParser.cpp
    Test_Helpers.cpp
)

add_executable(${TEST_PROJECT_NAME} ${TEST_ALL_SOURCES})

target_link_libraries(${TEST_PROJECT_NAME} message_proto)
target_link_libraries(${TEST_PROJECT_NAME} protobuf::libprotobuf)
target_link_libraries(${TEST_PROJECT_NAME} GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_proj)