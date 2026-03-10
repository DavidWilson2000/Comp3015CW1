#version 460

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube SkyTexture;

void main()
{
    FragColor = texture(SkyTexture, TexCoords);
}