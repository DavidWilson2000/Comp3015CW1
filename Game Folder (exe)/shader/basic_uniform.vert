#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec3 VertexTangent;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat3 NormalMatrix;

out vec3 FragPos;
out vec2 TexCoord;
out mat3 TBN;

void main()
{
    vec4 worldPos = Model * vec4(VertexPosition, 1.0);
    FragPos = worldPos.xyz;
    TexCoord = VertexTexCoord;

    vec3 T = normalize(NormalMatrix * VertexTangent);
    vec3 N = normalize(NormalMatrix * VertexNormal);
    vec3 B = normalize(cross(N, T));

    TBN = mat3(T, B, N);

    gl_Position = Projection * View * worldPos;
}