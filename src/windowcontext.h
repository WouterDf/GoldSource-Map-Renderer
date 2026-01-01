#pragma once

#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include "SDL3/SDL_opengl.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include <iostream>

struct WindowContext {
public:
     WindowContext() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
            return;
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                            SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        window = SDL_CreateWindow(
            "OpenGL Window",
            800,
            600,
            SDL_WINDOW_OPENGL
        );

        if (!window) {
            std::cerr << "SDL_CreateWindow failed: "
                    << SDL_GetError() << "\n";
            return;
        }

        SDL_ShowWindow(window);

        context = SDL_GL_CreateContext(window);
        if (!context) {
            std::cerr << "SDL_GL_CreateContext failed: "
                    << SDL_GetError() << "\n";
            return;
        }

        if (!SDL_GL_MakeCurrent(window, context)) {
            std::cerr << "SDL_GL_MakeCurrent failed: "
                    << SDL_GetError() << "\n";
            return;
        }

        // Initialize GLEW
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();
        if (glewError != GLEW_OK) {
            std::cerr << "GLEW initialization failed: "
                    << glewGetErrorString(glewError) << "\n";
            return;
        }

        const GLubyte* version = glGetString(GL_VERSION);
        if (!version) {
            std::cerr << "Failed to get OpenGL version\n";
        } else {
            std::cout << "OpenGL version: " << version << "\n";
        }

        const GLubyte* renderer = glGetString(GL_RENDERER);
        if (renderer) {
            std::cout << "Renderer: " << renderer << "\n";
        }
    }

    ~WindowContext() {
        SDL_DestroyWindow( window );
        SDL_Quit();
     }

public:
     SDL_Window* window;
     SDL_GLContext context;
};

