find_package(spdlog CONFIG QUIET)

if (NOT spdlog_FOUND)
    message(STATUS "spdlog not found, trying to fetch from remote...")
    FetchContent_Declare(spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG "v1.x"
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(spdlog)
else ()
    message(STATUS "Found spdlog")
endif ()

