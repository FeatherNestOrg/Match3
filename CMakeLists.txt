cmake_minimum_required(VERSION 4.0)
include(cmake/CheckToolchain.cmake)

project(match-3)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 26)

# Logging options
option(ENABLE_CONSOLE_LOG "Enable console log output" ON)

include(CheckModules)

add_subdirectory(src)
