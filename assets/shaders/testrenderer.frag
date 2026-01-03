#version 330 core
uniform sampler2D texture1;
uniform sampler2D texture2;

in vec2 Uv;
out vec4 FragColor;

void main()
{
        FragColor = texture(texture1, Uv);
        //FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}