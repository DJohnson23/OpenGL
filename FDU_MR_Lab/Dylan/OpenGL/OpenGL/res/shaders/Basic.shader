#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 v_normal;

out vec2 v_TexCoord;
out vec3 normal;
out vec3 FragPos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
   gl_Position = Projection * View * Model * vec4(position, 1.0);
   FragPos = vec3(Model * vec4(position, 1.0));
   v_TexCoord = texCoord;
   normal = mat3(transpose(inverse(Model))) * v_normal;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 normal;
in vec3 FragPos;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform vec3 viewPos;

void main()
{
	//vec3 lightPos = vec3(1, 2, 3);
	//vec3 lightDir = normalize(lightPos - FragPos);
	vec3 lightDir = normalize(vec3(1, 2, 3));
	vec3 lightColor = vec3(1, 0.9, 0.8);
	vec3 norm = normalize(normal);

	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	/*
	vec4 texColor = texture(u_Texture, v_TexCoord);
	color = texColor;
	color = u_Color;
	*/

	float specularStrength = 1;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * u_Color.rgb;
	color = vec4(result,1);
};