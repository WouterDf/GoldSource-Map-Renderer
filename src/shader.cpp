#include <iostream>
#include <GL/glew.h>
#include "shader.h"
#include "assetloader.h"


Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
         std::string vertexShaderSourceStdStr = AssetLoader::ReadAssetToString(vertexPath);
    std::string fragmentShaderSourceStdStr = AssetLoader::ReadAssetToString(fragmentPath);

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
    } else {
        char vertInfoLog[512];
        char fragInfoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, vertInfoLog);
        glGetShaderInfoLog(fragmentShader, 512, NULL, fragInfoLog);

        std::cout << vertInfoLog << "\n" << fragInfoLog << "\n";
    }

    this->programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);

    int linkSuccess;
    char infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &linkSuccess);
    if (!linkSuccess) {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::cout << "Shader program linking failed:\n" << infoLog << "\n";
    } else {
        std::cout << "Shader program linked successfully.\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Use() const
{
     glUseProgram(programId);  
}

void Shader::BindUniform(std::string name, int value) const {
     glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
}


void Shader::BindUniform4f(std::string name, const GLfloat* value) const
{
     unsigned int transformLoc = glGetUniformLocation(programId, name.c_str());
     glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value);
};
