#version 460

in vec3 TexCoords;
out vec4 FragColor;

uniform sampler2D SkyTexture;

void main()
{
    vec2 uv = TexCoords.xy * 0.5 + 0.5;
    FragColor = texture(SkyTexture, uv);
}