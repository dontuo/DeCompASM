# Download and link GoogleTest
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Add test files
include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB TEST_SOURCES *.cpp)

add_executable(UnitTests ${TEST_SOURCES} ../src/assembler.cpp )
target_link_libraries(UnitTests gtest gtest_main)

# Register tests
include(GoogleTest)
gtest_discover_tests(UnitTests)
