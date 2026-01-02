cmake_minimum_required(VERSION 3.30)
project(wouteropengl)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(wouteropengl
    src/main.cpp
    src/assetloader.h
    src/windowcontext.h
    src/shader.h
    src/shader.cpp
    src/renderer.h
    src/renderer.cpp
        )

find_package(SDL3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

# On macOS, explicitly use the system OpenGL framework
if(APPLE)
    find_library(OPENGL_LIBRARY OpenGL REQUIRED)
    target_link_libraries(wouteropengl PRIVATE SDL3::SDL3 GLEW::GLEW ${OPENGL_LIBRARY})
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(wouteropengl PRIVATE SDL3::SDL3 GLEW::GLEW OpenGL::GL)
endif()

# Copy assets directory to build directory
add_custom_command(TARGET wouteropengl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:wouteropengl>/assets
    COMMENT "Copying assets directory to build directory"
)

# macOS bundle configuration
if(APPLE)
    set_target_properties(wouteropengl PROPERTIES
			MACOSX_BUNDLE_BUNDLE_NAME "wouteropengl"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.wouter.wouteropengl"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    )

    # Set RPATH for bundled frameworks
    set_target_properties(wouteropengl PROPERTIES
        INSTALL_RPATH "@executable_path/../Frameworks"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

