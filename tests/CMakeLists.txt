# EXECUTABLE PROJECT - tests

# === Create Executable ===
add_executable(tests "tests.cpp")

# === Libraries ===
target_include_directories(tests PRIVATE "../include/")

# === Feature Enables ===
set_property(TARGET tests PROPERTY CXX_STANDARD 20)
enable_ipo(tests)
set_arch(tests)