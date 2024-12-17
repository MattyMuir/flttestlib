# GLOBAL - flttestlib

# === Includes ===
cmake_policy(SET CMP0069 NEW)
include(cmake/macros.cmake)

# === Configuration ===
cmake_minimum_required(VERSION 3.21)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable multi-processor build
enable_mp()

# Options
set(WARNINGS OFF CACHE BOOL "Enables clang-tidy and warnings on clang-like compilers")
if (WARNINGS)
	add_compile_options("-Wall")
    add_compile_options("-Wextra")
    add_compile_options("-Wpedantic")
    enable_clangtidy()
endif()

# === Projects ===
project("flttestlib")
add_subdirectory("src")

add_subdirectory("examples")

# === Tests ===
enable_testing()
set(CTEST_OUTPUT_ON_FAILURE ON)

add_subdirectory("tests")

add_test(NAME ULPDistanceFloat COMMAND tests 0)
add_test(NAME ULPDistanceDouble COMMAND tests 1)
add_test(NAME DistFloat COMMAND tests 2)
add_test(NAME DistDouble COMMAND tests 3)
add_test(NAME DistFloatEx COMMAND tests 4)
add_test(NAME DistDoubleEx COMMAND tests 5)