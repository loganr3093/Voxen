// Full Screen Quad Shader

// Vertex
#type vertex
#version 460

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in int a_EntityID;

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

layout(binding = 0) uniform sampler2D u_ColorTexture;
layout(binding = 1) uniform isampler2D u_EntityTexture;

void main()
{
	o_Color = texture(u_ColorTexture, v_TexCoords);
	o_EntityID = texture(u_EntityTexture, v_TexCoords).r;
}