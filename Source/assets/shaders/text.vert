#version 430 core


layout(location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
	//	gl_Position = model * view * projection * vec4(vertex.xy, 0.0, 1.0);
	TexCoords = vertex.zw;
}