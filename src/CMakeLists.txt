add_executable(${PROJECT_NAME}
        main.cpp
        Core/Game.cpp
        Core/Logger.cpp
        Render/Renderer.cpp
        Render/Texture.cpp
        Render/ResourceManager.cpp
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
        unordered_dense::unordered_dense
        spdlog
)