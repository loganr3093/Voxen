// Full Screen Quad Shader

// Vertex
#type vertex
#version 460

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in int a_EntityID;

layout(location = 0) out vec2 v_TexCoords;
layout(location = 1) out flat int v_EntityID;

void main()
{
	v_TexCoords = a_TexCoords;
	v_EntityID = a_EntityID;

	gl_Position = vec4(a_Position, 1.0);
}

// Fragment
#type fragment
#version 460

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

layout(location = 0) in vec2 v_TexCoords;
layout(location = 1) in flat int v_EntityID;

layout(binding = 0) uniform sampler2D u_Texture;

void main()
{
	o_Color = texture(u_Texture, v_TexCoords);
	//o_Color = vec4(1.0, 1.0, 1.0, 1.0);
	o_EntityID = v_EntityID;
}