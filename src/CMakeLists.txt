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
        unordered_dense::unordered_dense
        spdlog
        fmt
)

if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif ()