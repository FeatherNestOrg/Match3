if (WIN32)
    # 如果 CMake 已经指定了工具链文件（CI 环境），则跳过
    if (NOT CMAKE_TOOLCHAIN_FILE)
        if (NOT DEFINED ENV{VCPKG_ROOT})
            message(FATAL_ERROR "Environment variable VCPKG_ROOT not set; please set it to your vcpkg root path.")
        endif ()
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "Vcpkg toolchain file")
    endif ()
endif ()