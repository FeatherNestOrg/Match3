find_package(fmt QUIET)
if (NOT fmt_FOUND)
    message(STATUS "fmt can't be found locally, try fetching from remote...")
    FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG "master"
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(fmt)
else ()
    message(STATUS "Found fmt v${fmt_VERSION}")
endif ()