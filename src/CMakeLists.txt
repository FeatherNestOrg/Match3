file(GLOB_RECURSE M3_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME}
        ${M3_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")

# Configure logging
if (ENABLE_CONSOLE_LOG)
    target_compile_definitions(${PROJECT_NAME} PRIVATE M3_ENABLE_CONSOLE_LOG)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
        unordered_dense::unordered_dense
        spdlog::spdlog
        EnTT::EnTT
)

# ============================================================================
# Installation configuration
# ============================================================================

# Install the executable
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION .
)

install(FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
        DESTINATION bin
        OPTIONAL
)

install(DIRECTORY DESTINATION resources/textures)
install(DIRECTORY DESTINATION resources/sounds)
install(DIRECTORY DESTINATION resources/data)
install(DIRECTORY DESTINATION resources/fonts)

# Install any existing resources (if present)
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../resources")
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../resources/"
            DESTINATION resources
            OPTIONAL
    )
endif ()
