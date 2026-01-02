#pragma once

#include <GL/glew.h>
#include <string>

class Shader {
public:
     Shader(std::string vertexPath, std::string fragmentPath);
     void Use() const;
     void BindUniform(std::string name, int value) const;
     void BindUniform4f(std::string name, const GLfloat* value) const;

private:
     GLuint programId;
};

