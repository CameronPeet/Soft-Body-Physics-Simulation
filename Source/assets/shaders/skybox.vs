#version 430 core
layout (location = 0) in vec3 position;
out vec3 TexCoords;


layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};



void main()
{
	mat4 viewSpace = mat4(mat3(view));
    vec4 pos = projection * viewSpace * vec4(position, 1.0);
    gl_Position = pos.xyww;
    TexCoords = position;
}  