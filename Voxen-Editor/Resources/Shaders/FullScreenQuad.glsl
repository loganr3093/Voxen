// Full Screen Quad Shader

// Vertex
#type vertex
#version 460

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in int a_EntityID;
layout(location = 3) in vec4 a_AOEnabled;

layout(location = 0) out vec2 v_TexCoords;

void main()
{
	v_TexCoords = a_TexCoords;

	gl_Position = vec4(a_Position, 1.0);
}

// Fragment
#type fragment
#version 460

layout(location = 0) in vec2 v_TexCoords;

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;
layout(location = 2) out vec4 o_Normal;
layout(location = 3) out float o_Depth;

layout(binding = 0) uniform sampler2D u_ColorTexture;
layout(binding = 1) uniform isampler2D u_EntityTexture;
layout(binding = 2) uniform sampler2D u_NormalTexture;
layout(binding = 3) uniform sampler2D u_DepthTexture;
layout(binding = 4) uniform sampler2D u_AOTexture;

struct PointLight
{
    vec3 position;
    float intensity;
    vec3 color;
    float radius;
};
layout(std430, binding = 4) buffer LightBuffer
{
    PointLight u_PointLights[];
};
uniform int u_NumPointLights;

uniform int u_AOEnabled;
uniform int u_LightingEnabled;
uniform int u_ShowNormalsEnabled;

uniform vec3 u_SunLightDirection = vec3(0.2, 0.65, 0.4);
uniform mat4 u_InverseViewProjectionMatrix;
uniform float u_AmbientStrength = 0.2;
uniform float u_DiffuseStrength = 0.8;
uniform vec3 u_SunLightColor = vec3(1.0);

float CalculateAttenuation(float distance, float radius)
{
    float d = clamp(distance / radius, 0.0, 1.0);
    float attenuation = 1.0 - d * d;  // Quadratic falloff
    attenuation *= attenuation;

    // Add inverse square falloff
    float invsq = 1.0 / (1.0 + 15.0 * distance * distance);
    return (attenuation * invsq) * 4.0;
}

void main()
{
	// Reads
    vec4 color = texture(u_ColorTexture, v_TexCoords);
    int entityID = texture(u_EntityTexture, v_TexCoords).r;
    vec3 normal = texture(u_NormalTexture, v_TexCoords).rgb;
    float depth = texture(u_DepthTexture, v_TexCoords).r;
    float ao = u_AOEnabled != 0 ? texture(u_AOTexture, v_TexCoords).r : 1.0;

    if (entityID != -1 && u_LightingEnabled != 0)
    {
        // World position from depth
        vec4 clipPos = vec4(v_TexCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
        vec4 worldPos = u_InverseViewProjectionMatrix * clipPos;
        worldPos /= worldPos.w;

        vec3 normalizedNormal = normalize(normal);

        // Sun lighting
        vec3 lightDir = normalize(u_SunLightDirection);
        float sunDiffuseFactor = max(dot(normalizedNormal, lightDir), 0.0);
        vec3 sunDiffuse = color.rgb * u_DiffuseStrength * sunDiffuseFactor * u_SunLightColor;
        vec3 ambient = color.rgb * u_AmbientStrength * u_SunLightColor;

        // Point lights
        vec3 pointDiffuse = vec3(0.0);
        for (int i = 0; i < u_NumPointLights; ++i)
        {
            PointLight light = u_PointLights[i];
            vec3 toLight = light.position - worldPos.xyz;
            float distance = length(toLight);

            // Calculate attenuation based on modified function
            float attenuation = CalculateAttenuation(distance, light.radius);
            attenuation *= light.intensity;

            vec3 lightDir = normalize(toLight);
            float diffuseFactor = max(dot(normalizedNormal, lightDir), 0.0);
            pointDiffuse += color.rgb * diffuseFactor * light.color * attenuation;
        }

        // Combine all lighting and apply AO
        vec3 totalDiffuse = sunDiffuse + pointDiffuse;
        vec3 litColor = (ambient + totalDiffuse) * ao;
        color.rgb = litColor;
    }


    // Outputs
    o_Color = u_ShowNormalsEnabled == 1 ? vec4(normal, 1.0) : color;
    o_EntityID = entityID;
    o_Normal = vec4(normal, 1.0);
    o_Depth = depth;
}