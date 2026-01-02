#pragma once

#include <GL/glew.h>
#include <string>

class Shader {
public:
     Shader(std::string vertexPath, std::string fragmentPath);
     void Use() const;

private:
     GLuint programId;
};

