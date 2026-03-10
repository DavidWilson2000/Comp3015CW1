#version 460 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D ScreenTexture;
uniform bool RelicActivated;

void main()
{
    vec3 color = texture(ScreenTexture, vTexCoord).rgb;

    // base subtle lift
    color *= 1.05;

    if (RelicActivated)
    {
        color *= vec3(1.10, 1.10, 1.18);
    }

    FragColor = vec4(color, 1.0);
}