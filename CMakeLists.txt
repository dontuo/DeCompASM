cmake_minimum_required(VERSION 3.10)
project(DeCompAsm)

set(CMAKE_CXX_STANDARD 17)

include_directories(src/)
file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS src/*.hpp)

# Main program
add_executable(DeCompAsm ${SOURCES} ${HEADERS})

# GoogleTest setup
enable_testing()
add_subdirectory(tests)
