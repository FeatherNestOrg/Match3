find_package(SDL3 REQUIRED)

if (NOT SDL3_FOUND)
    message(STATUS "SDL3 can't be found locally, try fetching from remote...")
    FetchContent_Declare(
            SDL3_fetched
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG "main"
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SDL3_fetched)
else ()
    message(STATUS "Found SDL3 v${SDL3_VERSION}")
endif ()

find_package(SDL3_image REQUIRED)
if (NOT SDL3_image_FOUND)
    message(STATUS "SDL3_image can't be found locally, try fetching from remote...")
    FetchContent_Declare(
            SDL3_image_fetched
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
            GIT_TAG "main"
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SDL3_image_fetched)
else ()
    message(STATUS "Found SDL3_image v${SDL3_image_VERSION}")
endif ()