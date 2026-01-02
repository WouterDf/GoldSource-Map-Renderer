#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;
layout (location = 2) in vec2 aUvCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 color;
out vec2 UvCoord;

void main()
{
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        color = aCol;
        UvCoord = aUvCoord;
}