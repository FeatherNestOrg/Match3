file(GLOB_RECURSE M3_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Determine if we're building for Android
if (ANDROID)
    # Build as shared library for Android
    add_library(${PROJECT_NAME} SHARED ${M3_SOURCES})
else ()
    # Build as executable for desktop platforms
    add_executable(${PROJECT_NAME} ${M3_SOURCES})
endif ()

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
        nlohmann_json::nlohmann_json
)

# ============================================================================
# Copy resources to build directory for development
# ============================================================================
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
        COMMENT "Copying resources to build directory..."
)

# ============================================================================
# Installation configuration
# ============================================================================

if (ANDROID)
    # For Android, install the shared library
    install(TARGETS ${PROJECT_NAME}
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
    )
else ()
    # For desktop platforms, install everything to root directory
    # This creates a flat structure: Match3-x.x.x/Match3.exe + DLLs + resources/
    install(TARGETS ${PROJECT_NAME}
            RUNTIME DESTINATION .
            BUNDLE DESTINATION .
    )

    # Install runtime DLLs to root directory (same as executable)
    # This includes DLLs from target dependencies
    install(FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            DESTINATION .
            OPTIONAL
    )

    # For Windows with vcpkg, also install DLLs from the build directory
    # This catches dependencies like freetyped.dll, brotli, etc.
    if (WIN32)
        # Install DLLs dynamically at install time
        install(CODE "
            file(GLOB DLL_FILES \"${CMAKE_CURRENT_BINARY_DIR}/*.dll\")
            if(DLL_FILES)
                file(INSTALL \${DLL_FILES} DESTINATION \"\${CMAKE_INSTALL_PREFIX}\")
            endif()
        ")
    endif ()
endif ()

# Install resources (only if they exist and contain files)
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../resources")
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../resources/"
            DESTINATION resources
            FILES_MATCHING
            PATTERN "*.ttf"
            PATTERN "*.png"
            PATTERN "*.jpg"
            PATTERN "*.wav"
            PATTERN "*.ogg"
            PATTERN "*.mp3"
            PATTERN "*.json"
            PATTERN "*.xml"
            PATTERN ".gitkeep" EXCLUDE
    )
endif ()
