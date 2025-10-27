add_executable(${PROJECT_NAME}
        main.cpp
        Core/Game.cpp
        Core/Logger.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")

# Configure logging
if (ENABLE_CONSOLE_LOG)
    target_compile_definitions(${PROJECT_NAME} PRIVATE M3_ENABLE_CONSOLE_LOG)
endif ()

# Set log level
target_compile_definitions(${PROJECT_NAME} PRIVATE M3_LOG_LEVEL="${LOG_LEVEL}")

target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        unordered_dense::unordered_dense
        spdlog
)