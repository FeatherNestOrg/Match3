find_package(SDL3_ttf CONFIG QUIET)

if (NOT SDL3_ttf_FOUND)
    message(STATUS "SDL3_ttf can't be found locally, try fetching from remote...")
    FetchContent_Declare(
            SDL3_ttf_fetched
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
            GIT_TAG "main"
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SDL3_ttf_fetched)
else ()
    message(STATUS "Found SDL3_ttf v${SDL3_ttf_VERSION}")
endif ()
