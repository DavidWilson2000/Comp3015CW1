#version 460

in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

uniform float TileFactor;
uniform vec3 ViewPos;
uniform vec3 AmbientColor;
uniform float Shininess;
uniform bool GammaEnabled;
uniform float Gamma;
uniform float Alpha;
uniform bool UseAlpha;

uniform sampler2D DiffuseTex;
uniform sampler2D MossTex;
uniform sampler2D NormalMap;
uniform sampler2D TreeTex;
uniform sampler2D KeyTex;

uniform bool NormalMapEnabled;
uniform bool EmissiveEnabled;
uniform bool UseTreeTexture;
uniform bool UseFlatColor;
uniform bool UseKeyTexture;

uniform vec3 EmissiveColor;
uniform vec3 FlatColor;

// Directional light
uniform vec3 DirLightDirection;
uniform vec3 DirLightColor;

// Point light
uniform vec3 PointLightPos;
uniform vec3 PointLightColor;

// Relic spotlight
uniform vec3 SpotLightPos;
uniform vec3 SpotLightDirection;
uniform vec3 SpotLightColor;
uniform float SpotLightCutOff;
uniform float SpotLightOuterCutOff;
uniform bool SpotLightEnabled;

// Fog
uniform vec3 FogColor;
uniform float FogDensity;

vec3 calculateDirectionalLight(vec3 norm, vec3 viewDir, vec3 baseColor)
{
    vec3 lightDir = normalize(-DirLightDirection);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * DirLightColor * baseColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), Shininess);
    vec3 specular = spec * DirLightColor * 0.4;

    return diffuse + specular;
}

vec3 calculatePointLight(vec3 norm, vec3 fragPos, vec3 viewDir, vec3 baseColor)
{
    vec3 lightDir = normalize(PointLightPos - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * PointLightColor * baseColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), Shininess);
    vec3 specular = spec * PointLightColor * 0.6;

    float distance = length(PointLightPos - fragPos);
    float attenuation = 1.0 / (1.0 + 0.12 * distance + 0.08 * distance * distance);

    diffuse *= attenuation;
    specular *= attenuation;

    return diffuse + specular;
}

vec3 calculateSpotLight(vec3 norm, vec3 fragPos, vec3 viewDir, vec3 baseColor)
{
    if (!SpotLightEnabled)
        return vec3(0.0);

    vec3 lightDir = normalize(SpotLightPos - fragPos);

    float theta = dot(lightDir, normalize(-SpotLightDirection));
    float epsilon = SpotLightCutOff - SpotLightOuterCutOff;
    float intensity = clamp((theta - SpotLightOuterCutOff) / epsilon, 0.0, 1.0);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * SpotLightColor * baseColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), Shininess);
    vec3 specular = spec * SpotLightColor * 0.8;

    float distance = length(SpotLightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return diffuse + specular;
}

void main()
{
    vec3 norm;

    if (NormalMapEnabled)
    {
        vec3 normalSample = texture(NormalMap, TexCoord * TileFactor).rgb;
        normalSample = normalSample * 2.0 - 1.0;
        norm = normalize(TBN * normalSample);
    }
    else
    {
        norm = normalize(TBN[2]);
    }

    vec3 viewDir = normalize(ViewPos - FragPos);

    vec3 baseColor;

  if (UseFlatColor)
{
    baseColor = FlatColor;
}
else if (UseTreeTexture)
{
    baseColor = texture(TreeTex, TexCoord).rgb;
}
else if (UseKeyTexture)
{
    baseColor = texture(KeyTex, TexCoord).rgb;
}
else
{
    vec3 stone = texture(DiffuseTex, TexCoord * TileFactor).rgb;
    vec3 moss = texture(MossTex, TexCoord * TileFactor * 1.5).rgb;

    float mossAmount = clamp((norm.y - 0.3) * 2.0, 0.0, 1.0);
    baseColor = mix(stone, moss, mossAmount);
}

    vec3 ambient = AmbientColor * baseColor;
    vec3 dirLight = calculateDirectionalLight(norm, viewDir, baseColor);
    vec3 pointLight = calculatePointLight(norm, FragPos, viewDir, baseColor);
    vec3 spotLight = calculateSpotLight(norm, FragPos, viewDir, baseColor);

    vec3 result = ambient + dirLight + pointLight + spotLight;

    if (EmissiveEnabled)
        result += EmissiveColor;

    float dist = length(ViewPos - FragPos);
    float fogFactor = exp(-FogDensity * dist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(FogColor, result, fogFactor);

    if (GammaEnabled)
        finalColor = pow(finalColor, vec3(1.0 / Gamma));

float finalAlpha = UseAlpha ? Alpha : 1.0;

FragColor = vec4(finalColor, finalAlpha);

float brightness = dot(finalColor, vec3(0.2126, 0.7152, 0.0722));

if (brightness > 1.0)
    BrightColor = vec4(finalColor, finalAlpha);
else
    BrightColor = vec4(0.0, 0.0, 0.0, finalAlpha);
    }