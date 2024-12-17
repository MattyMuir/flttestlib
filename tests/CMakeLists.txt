# EXECUTABLE PROJECT - tests

# === Create Executable ===
add_executable(tests "tests.cpp")

# === Libraries ===
target_include_directories(tests PRIVATE "../include/")

find_package(PkgConfig)
pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
target_link_libraries(tests PRIVATE PkgConfig::mpfr)

# === Feature Enables ===
set_property(TARGET tests PROPERTY CXX_STANDARD 20)
enable_ipo(tests)
set_arch(tests)