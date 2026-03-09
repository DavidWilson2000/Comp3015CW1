#version 460

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloomEnabled;
uniform float bloomStrength;

void main()
{
    vec3 hdrColor = texture(scene, TexCoord).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoord).rgb;

    vec3 result = hdrColor;

    if (bloomEnabled)
        result += bloomColor * bloomStrength;

    FragColor = vec4(result, 1.0);
}