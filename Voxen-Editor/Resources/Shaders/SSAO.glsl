#version 460 core
layout(local_size_x = 16, local_size_y = 16) in;

layout(r32f, binding = 0) writeonly uniform image2D u_AOTexture;

layout(binding = 0) uniform sampler2D u_DepthTexture;
layout(binding = 1) uniform sampler2D u_NormalTexture;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_InverseViewProjectionMatrix;
uniform vec2 u_ScreenSize;
uniform float u_Radius = 0.5;
uniform float u_Bias = 0.025;
uniform float u_AOStrength;

const vec3 samples[16] = vec3[](
    vec3(0.04977, 0.04235, 0.04996), vec3(0.01445, 0.07621, 0.02011),
    vec3(-0.14387, 0.07520, 0.04634), vec3(-0.12405, 0.10884, 0.02062),
    vec3(0.12507, 0.13995, 0.10652), vec3(-0.14394, 0.04524, 0.12348),
    vec3(0.09129, 0.05783, 0.14267), vec3(0.12603, 0.02535, 0.04589),
    vec3(-0.05483, 0.09888, 0.06190), vec3(0.14782, 0.10193, 0.13105),
    vec3(-0.21412, 0.10490, 0.14219), vec3(-0.00543, 0.15388, 0.19570),
    vec3(-0.07424, 0.22972, 0.22716), vec3(-0.03662, 0.33388, 0.15370),
    vec3(-0.26342, 0.17966, 0.23320), vec3(-0.21603, 0.30869, 0.19086)
    );

void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    if (coord.x >= int(u_ScreenSize.x) || coord.y >= int(u_ScreenSize.y)) return;

    vec2 texCoord = vec2(coord) / u_ScreenSize;

    // Reconstruct world position
    float depth = texture(u_DepthTexture, texCoord).r;
    vec4 clipPos = vec4(texCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 worldPos = u_InverseViewProjectionMatrix * clipPos;
    worldPos /= worldPos.w;

    // Get normal and generate random vector
    vec3 normal = normalize(texture(u_NormalTexture, texCoord).rgb);
    vec3 randomVec = normalize(vec3(
        fract(sin(dot(texCoord, vec2(12.9898, 78.233))) * 43758.5453),
        fract(sin(dot(texCoord, vec2(12.9898, 78.233) + vec2(42.123, 13.456))) * 43758.5453),
        fract(sin(dot(texCoord, vec2(12.9898, 78.233) + vec2(87.654, 32.109))) * 43758.5453)
    ));

    // Create TBN matrix
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // Calculate occlusion
    float occlusion = 0.0;
    for (int i = 0; i < 8; i++)
    {
        vec3 samplePos = TBN * samples[i];
        samplePos = worldPos.xyz + samplePos * u_Radius;

        vec4 offset = u_ViewProjectionMatrix * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;
        vec2 sampleUV = offset.xy * 0.5 + 0.5;

        if (sampleUV.x < 0 || sampleUV.x>1 || sampleUV.y < 0 || sampleUV.y>1) continue;

        float sampleDepth = texture(u_DepthTexture, sampleUV).r * 2.0 - 1.0;
        float rangeCheck = smoothstep(0.0, 1.0, u_Radius / abs(worldPos.z - samplePos.z));
        if (offset.z > sampleDepth + u_Bias)
            occlusion += rangeCheck;
    }

    occlusion = 1.0 - occlusion / 16.0;
    float finalAO = mix(1.0, occlusion, u_AOStrength);
    imageStore(u_AOTexture, coord, vec4(finalAO, 0.0, 0.0, 1.0));
}