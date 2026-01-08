cmake_minimum_required(VERSION 3.30)
project(wouteropengl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(wouteropengl
    src/main.cpp
    src/assetloader.h
    src/assetloader.cpp
    src/windowcontext.h
    src/shader.h
    src/shader.cpp
    src/bsprenderer.cpp
    src/bsprenderer.h
    src/bsprenderbatch.h
    src/bsprenderbatch.cpp
    src/configuration.h
    src/configuration.cpp
    src/texture.h
    src/texture.cpp
    src/pngtexture.h
    src/pngtexture.cpp
    src/wadtexture.cpp
    src/wadtexture.h
    src/lightmaptexture.h
    src/lightmaptexture.cpp
    src/worldgeometry.h
    src/worldgeometry.cpp
    src/camera.cpp
    src/camera.h
    src/bsp.h
    src/bsp.cpp
    src/wad.h
    src/wad.cpp
        )

find_package(SDL3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(simage REQUIRED)
find_package(glm REQUIRED)

find_path(INIPP_INCLUDE_DIRS "inipp.h" REQUIRED)
target_include_directories(wouteropengl PRIVATE ${INIPP_INCLUDE_DIRS})

if (WIN32)
    target_compile_definitions(wouteropengl PRIVATE SDL_MAIN_HANDLED)
endif()

# On macOS, explicitly use the system OpenGL framework
if(APPLE)
    find_library(OPENGL_LIBRARY OpenGL REQUIRED)
    target_link_libraries(wouteropengl
        PRIVATE
        simage::simage
        SDL3::SDL3
        GLEW::GLEW
        ${OPENGL_LIBRARY}
        glm::glm-header-only
    )
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(wouteropengl
        PRIVATE
        simage::simage
        SDL3::SDL3
        GLEW::GLEW
        OpenGL::GL
        glm::glm-header-only
    )
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

