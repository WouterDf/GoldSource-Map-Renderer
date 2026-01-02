#version 330 core
in vec4 color;
in vec2 UvCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

out vec4 FragColor;

void main()
{
        FragColor = texture(texture1, UvCoord);

}