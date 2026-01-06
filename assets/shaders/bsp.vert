#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUv;
layout (location = 2) in vec2 aLightMapCoords;

out vec2 Uv;
out vec2 LightMapCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        Uv = aUv;
        LightMapCoords = aLightMapCoords;
}