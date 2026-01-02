#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;
layout (location = 2) in vec2 aUvCoord;

out vec4 color;
out vec2 UvCoord;

void main()
{
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        color = aCol;
        UvCoord = aUvCoord;
}