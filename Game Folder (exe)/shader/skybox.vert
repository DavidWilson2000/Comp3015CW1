#version 460

layout(location = 0) in vec3 VertexPosition;

out vec3 TexCoords;

uniform mat4 View;
uniform mat4 Projection;

void main()
{
    TexCoords = VertexPosition;
    vec4 pos = Projection * View * vec4(VertexPosition, 1.0);
    gl_Position = pos.xyww;
}