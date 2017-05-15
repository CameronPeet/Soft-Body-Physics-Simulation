#version 430 core

layout (location = 0) in vec3 position; 
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;


layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat4 normalMatrix;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);

		Normal = mat3(transpose(inverse(model * view))) * normal;

	FragPos = vec3(model * vec4(position, 1.0f));

	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}
