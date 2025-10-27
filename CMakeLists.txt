cmake_minimum_required(VERSION 3.31)
include(cmake/CheckToolchain.cmake)

project(match-3)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 26)

# Logging options
option(ENABLE_CONSOLE_LOG "Enable console log output" ON)
set(LOG_LEVEL "DEBUG" CACHE STRING "Log level (TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL)")
set_property(CACHE LOG_LEVEL PROPERTY STRINGS "TRACE" "DEBUG" "INFO" "WARN" "ERROR" "CRITICAL")

include(CheckModules)

add_subdirectory(src)
