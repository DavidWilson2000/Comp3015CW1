#version 460 core

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 TexCoord;

out vec2 vTexCoord;

void main()
{
    vTexCoord = TexCoord;
    gl_Position = vec4(Position, 0.0, 1.0);
}