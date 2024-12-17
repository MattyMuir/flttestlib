# EXECUTABLE PROJECT - example

# === Create Executable ===
add_executable(example "example.cpp")

# === Libraries ===
target_include_directories(example PRIVATE "../include/")

find_package(PkgConfig)
pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
target_link_libraries(example PRIVATE PkgConfig::mpfr)

# === Feature Enables ===
set_property(TARGET example PROPERTY CXX_STANDARD 20)
enable_ipo(example)
set_arch(example)