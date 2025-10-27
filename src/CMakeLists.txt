add_executable(${PROJECT_NAME}
        main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")

# Configure logging
if (ENABLE_CONSOLE_LOG)
    target_compile_definitions(${PROJECT_NAME} PRIVATE TEH_ENABLE_CONSOLE_LOG)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        unordered_dense::unordered_dense
        spdlog
)

if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif ()