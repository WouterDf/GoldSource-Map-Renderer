#include "windowcontext.h"
#include "assetloader.h"
#include "SDL3/SDL_events.h"
#include <SDL3/SDL_main.h>
#include <iostream>
#include <string>
#include <sys/types.h>

void mainLoop() {
    bool done = false;
    while( !done )
    {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }
    };
}


int main()
{
    std::cout << "Starting application. \n";
    std::cout << "Asset path is: " << AssetLoader::getAssetPath().string() << "\n";

    WindowContext windowContext{};

    float quadVertices[] = {
         -0.5f, -0.5f, 0.0f, // bottom left
          0.5f, -0.5f, 0.0f, // bottom right
          0.5f,  0.5f, 0.0f, // top right
         -0.5f,  0.5f, 0.0f  // top left
    };

    uint indices[] = {
         0, 3, 2,
         2, 1, 0
    };

    // Shaders
    std::string vertexShaderSourceStdStr = AssetLoader::readAssetToString("shaders/shader.vert");
    std::string fragmentShaderSourceStdStr = AssetLoader::readAssetToString("shaders/shader.frag");

    const char* vertexShaderSource = vertexShaderSourceStdStr.c_str();
    const char* fragmentShaderSource = fragmentShaderSourceStdStr.c_str();

    uint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    uint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    int vertexSuccess;
    int fragmentSuccess;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);

    if( vertexSuccess && fragmentSuccess ) {
         std::cout << "Compiled shaders succesfully. \n";
    }

    uint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int linkSuccess;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkSuccess);
    if (!linkSuccess) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader program linking failed:\n" << infoLog << "\n";
    } else {
        std::cout << "Shader program linked successfully.\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // buffers
    uint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    uint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    uint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    bool done = false;
    while( !done )
    {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        SDL_GL_SwapWindow(windowContext.window);
    };

//    mainLoop();

    return 0;
}
