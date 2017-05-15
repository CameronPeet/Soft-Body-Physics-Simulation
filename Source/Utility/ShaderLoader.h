#pragma once

#include <iostream>

#include <glew\glew.h>
#include <freeglut\freeglut.h>

class ShaderLoader
{
private:

	std::string ReadShader(char *filename);
	GLuint CreateShader(GLenum shaderType,
		std::string source,
		char* shaderName);

public:

	ShaderLoader(void);
	~ShaderLoader(void);

	//Create vs, fs and gs shader program
	GLuint CreateProgram(char* VertexShaderFilename,
		char* FragmentShaderFilename,
		char* geometryShaderFilename = nullptr);
};
