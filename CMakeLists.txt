cmake_minimum_required(VERSION 3.31)
include(cmake/CheckToolchain.cmake)

project(Match3 VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 26)

# Logging options
option(ENABLE_CONSOLE_LOG "Enable console log output" ON)
set(LOG_LEVEL "DEBUG" CACHE STRING "Log level (TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL)")
set_property(CACHE LOG_LEVEL PROPERTY STRINGS "TRACE" "DEBUG" "INFO" "WARN" "ERROR" "CRITICAL")

include(CheckModules)

add_subdirectory(src)

# ============================================================================
# CPack Configuration - for creating distributable packages
# ============================================================================

# Set the install destination for system libraries to root directory
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "Match3")
set(CPACK_PACKAGE_VENDOR "FeatherNestOrg")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Match3")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/packages")

# ZIP generator configuration
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}")

# Installation directory structure
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")

include(CPack)

