#version 460

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloomEnabled;
uniform float bloomStrength;

uniform float PickupPulseStrength;
uniform float WinOverlayAmount;

void main()
{
    vec3 hdrColor = texture(scene, TexCoord).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoord).rgb;

    vec3 result = hdrColor;

    if (bloomEnabled)
        result += bloomColor * bloomStrength;

    // Orb pickup pulse
    if (PickupPulseStrength > 0.0)
    {
        float distToCenter = distance(TexCoord, vec2(0.5, 0.5));
        float flash = smoothstep(0.95, 0.15, 1.0 - distToCenter);
        vec3 pulseColor = vec3(1.0, 0.78, 0.30);
        result += pulseColor * flash * PickupPulseStrength * 0.8;
    }

    // Win overlay darken
    result = mix(result, result * 0.2, WinOverlayAmount * 0.55);

    // Simple glowing center panel
    vec2 centered = abs(TexCoord - vec2(0.5, 0.5));

    float panelX = 1.0 - smoothstep(0.18, 0.22, centered.x);
    float panelY = 1.0 - smoothstep(0.06, 0.10, centered.y);
    float panel = panelX * panelY;

    vec3 panelColor = vec3(1.0, 0.88, 0.45);
    result = mix(result, panelColor, panel * WinOverlayAmount * 0.35);

    // Bright horizontal glow line through middle
    float lineY = 1.0 - smoothstep(0.0, 0.02, centered.y);
    float lineX = 1.0 - smoothstep(0.18, 0.24, centered.x);
    float lineMask = lineX * lineY;

    result += vec3(1.0, 0.97, 0.75) * lineMask * WinOverlayAmount * 0.9;

    FragColor = vec4(result, 1.0);
}