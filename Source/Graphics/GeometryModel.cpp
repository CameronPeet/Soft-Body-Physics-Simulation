#include "GeometryModel.h"

GLfloat points[] = {
	-0.5f,  0.5f, -5.0f,  1.0f, 0.0f, 0.0f, // Top-left
	0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Top-right
	0.5f, -0.5f, 5.0f, 0.0f, 0.0f, 1.0f, // Bottom-right
	-0.5f, -0.5f, 7.0f, 1.0f, 1.0f, 0.0f  // Bottom-left
};
GeometryModel::GeometryModel(GLuint program, Camera* camera)
{
	this->program = program;
	this->camera = camera;



	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
}

void
GeometryModel::Render() 
{

	glUseProgram(program);
	glm::mat4 model;
	model = glm::translate(model, m_Position);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, 4);
	glBindVertexArray(0);
			

}