#version 460 core
layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0) uniform sampler2D u_InputTexture;
layout(r32f, binding = 0) writeonly uniform image2D u_OutputTexture;

uniform vec2 u_ScreenSize;
// 0 = horizontal, 1 = vertical
uniform int u_Direction;

const float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    if (coord.x >= int(u_ScreenSize.x) || coord.y >= int(u_ScreenSize.y))
        return;

    vec2 texCoord = vec2(coord) / u_ScreenSize;

    float result = texture(u_InputTexture, texCoord).r * weights[0];

    if (u_Direction == 0)
    {
        // Horizontal blur
        for (int i = 1; i < 5; ++i)
        {
            result += texture(u_InputTexture, texCoord + vec2(i / u_ScreenSize.x, 0.0)).r * weights[i];
            result += texture(u_InputTexture, texCoord - vec2(i / u_ScreenSize.x, 0.0)).r * weights[i];
        }
    }
    else
    {
        // Vertical blur
        for (int i = 1; i < 5; ++i)
        {
            result += texture(u_InputTexture, texCoord + vec2(0.0, i / u_ScreenSize.y)).r * weights[i];
            result += texture(u_InputTexture, texCoord - vec2(0.0, i / u_ScreenSize.y)).r * weights[i];
        }
    }

    imageStore(u_OutputTexture, coord, vec4(result, 0.0, 0.0, 0.0));
}