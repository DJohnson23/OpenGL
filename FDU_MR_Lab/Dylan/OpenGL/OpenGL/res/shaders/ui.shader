#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 v_TexCoord;
out vec3 FragPos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
	gl_Position = Projection * View * Model * vec4(position, 1.0);
	FragPos = vec3(Model * vec4(position, 1.0));
	v_TexCoord = texCoord;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 FragPos;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform vec3 viewPos;

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord);
	color = texColor * u_Color;
};