find_package(spdlog QUIET)
if (NOT spdlog_FOUND)
    FetchContent_Declare(spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG "v1.x"
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(spdlog)
endif ()