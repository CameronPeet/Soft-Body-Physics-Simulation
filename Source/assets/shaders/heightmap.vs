#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out vec2 TexCoord;
out vec3 worldPos;
out vec3 FragPos;
out vec3 Normal;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	TexCoord = texCoord;
	
	FragPos = vec3(model* vec4(position, 1.0f));
	Normal = mat3(transpose(inverse(model))) * normal;
	
	vec4 tWorldPos =  model *vec4(position, 1.0);
	worldPos = tWorldPos.xyz;
}