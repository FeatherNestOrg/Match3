# Android Toolchain Configuration for Match-3 Game
# This file provides simplified Android build configuration

if(NOT ANDROID_NDK)
    set(ANDROID_NDK $ENV{ANDROID_NDK_HOME})
endif()

if(NOT ANDROID_NDK)
    message(FATAL_ERROR "ANDROID_NDK not set. Please set ANDROID_NDK_HOME environment variable.")
endif()

# Default Android settings
if(NOT ANDROID_PLATFORM)
    set(ANDROID_PLATFORM android-24)  # Android 7.0, minimum for SDL3
endif()

if(NOT ANDROID_ABI)
    set(ANDROID_ABI arm64-v8a)  # Default to ARM64
endif()

if(NOT ANDROID_STL)
    set(ANDROID_STL c++_shared)
endif()

# Include the official Android toolchain
include(${ANDROID_NDK}/build/cmake/android.toolchain.cmake)

message(STATUS "Android Toolchain Configuration:")
message(STATUS "  NDK: ${ANDROID_NDK}")
message(STATUS "  Platform: ${ANDROID_PLATFORM}")
message(STATUS "  ABI: ${ANDROID_ABI}")
message(STATUS "  STL: ${ANDROID_STL}")
