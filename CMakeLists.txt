cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0016 NEW)

project(protobuf_parser)

add_subdirectory(${CMAKE_SOURCE_DIR}/src/protobuf)

enable_testing()
add_subdirectory(test)
