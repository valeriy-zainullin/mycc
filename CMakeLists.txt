project(mycc)

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

include_directories(ast/include)
include_directories(parser/include)

add_subdirectory(ast)
add_subdirectory(parser)
