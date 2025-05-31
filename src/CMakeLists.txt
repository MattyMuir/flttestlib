# LIBRARY PROJECT - flttestlib

# === Includes ===
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# === Create Library ===
add_library(flttestlib INTERFACE)

set_property(TARGET flttestlib PROPERTY CXX_STANDARD 20)

# === Install ===
# Install binaries and headers
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Config
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # static library files
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # includes
)

# Export includes
install(DIRECTORY "../include"
        DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY "../src"
        DESTINATION ${CMAKE_INSTALL_PREFIX}
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp")

# Configure the main config.cmake file
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Install the export set and configuration files
install(EXPORT ${PROJECT_NAME}Config
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)