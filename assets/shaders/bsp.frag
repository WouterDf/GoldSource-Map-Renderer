#version 330 core
out vec4 FragColor;

in vec2 Uv;
in vec2 LightMapCoords;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
        vec3 albedo = texture(texture1, Uv).rgb;
        vec3 light  = texture(texture2, LightMapCoords).rgb;

        // Boost light intensity (adjust 2.0 to taste)
        light *= 2.0;

        FragColor = vec4(albedo * light, 1.0);
        //FragColor = texture(texture1, Uv);
        //FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}