#pragma once

#include <GL/glew.h>
#include <string>

class Shader {
public:
     Shader(std::string vertexPath, std::string fragmentPath);
     void Use() const;
     void BindUniform(std::string name, int value) const;

private:
     GLuint programId;
};

