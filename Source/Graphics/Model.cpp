// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2016 Media Design School
//
// File Name	: Model.cpp
// Description	: Encapsulate required functionality and components to define and render a predefined shape.
// Author		: Cameron Peet
// Mail			: Cameron.Peet@mediadesign.school.nz

#include "Model.h"
#include "Light.h"
#include "Camera.h"

Model::Model()
{

}

Model::~Model()
{

}

/*@Author : Cameron Peet
@Purpose : Choose a static function to define the vertices for the models shape, according to the enum provided
@param ModelType : Determine the shape of the model
@param Filename : Store the filepath to the desired texture (Only 1 for now ).
*/

Model::Model(ModelType Shape, char* Filename, bool usingBulletPhysics)
{
	switch (Shape)
	{
	case QUAD:
		Utils::SetQuad(*this);
		break;

	case CUBE:
		Utils::SetCube(*this);
		break;

	case TRIANGLE:
		Utils::SetTriangle(*this);
		break;

	case PYRAMID:
		Utils::SetPyramid(*this);
		break;

	case CIRCLE:
		Utils::SetCircle(*this);
		break;
	case SPHERE:
		Utils::SetSphere(*this);
		break;
	}

	m_Scale = glm::vec3(1, 1, 1);
	texturePath = Filename;

}

/*Author : Cameron Peet
Purpose :  Initialise the vao, vbo and ebo, and load any textures
*/
void Model::Initialise()
{
	//Buffer Creation
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	//Attribute Setting
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Texture Position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Vertex Normals
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set texutre wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}


/* 
@Author : Cameron Peet
@Purpose : Render the model in a 3D manner using the Camera, consistent across all models 
@param program : The coupled vertex and fragment shader desired for the rendering of the calling instance 
@param camera :  Address to the main camera, providing the view and projection matrices for rendering the view of the model.
*/
void Model::Render(GLuint program, Camera& camera, Light* lightSource)
{
	glUseProgram(program);

	GLfloat currentTime = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	currentTime = currentTime / 1000;//convert millisecond to seconds

	//get uniform location from program and set currentTime value
	GLint currentTimeLocation = glGetUniformLocation(program, "currentTime"); 
	glUniform1f(currentTimeLocation, currentTime); 

	GLint modelLoc = glGetUniformLocation(program, "model");

	GLint lightColorLoc = glGetUniformLocation(program, "lightColor");
	GLint lightPosLoc = glGetUniformLocation(program, "lightPos");
	GLint specloc = glGetUniformLocation(program, "specularStrength");
	GLint ambLoc = glGetUniformLocation(program, "ambientStrength");
	GLint objectColorLoc = glGetUniformLocation(program, "objectColor");
	GLint cameraPosLoc = glGetUniformLocation(program, "cameraPos");
	GLint normalMatrixLoc = glGetUniformLocation(program, "normalMatrix");

	glm::mat4 model;

	//Translate the model according to its position and apply the rotation
	model = glm::translate(model, m_Position) * glm::mat4(m_Rotation);

	//Rotation points
	if (m_RotationPoint != glm::vec3(0, 0, 0))
		model = glm::translate(model, m_Position - m_RotationPoint);

	//Scale the model according to private member m_Scale
	model = glm::scale(model, m_Scale);

	glm::mat4 normalMatrix;
	normalMatrix = glm::transpose(glm::inverse(model * camera.GetViewMatrix()));
	//Send the values to the uniform variables in the program supplied in the function call
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glm::mat4 vp = camera.GetProjectionMatrix() * camera.GetViewMatrix();
	GLint vpLoc = glGetUniformLocation(program, "vp");
	glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(vp));
	glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	if (lightSource != nullptr)
	{
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightSource->Color));
		glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightSource->Position));
		glUniform1f(specloc, 0.5f);
		glUniform1f(ambLoc, 0.5f);
		glUniform3fv(objectColorLoc, 1, glm::value_ptr(ObjectColor));
		glUniform3fv(cameraPosLoc, 1, glm::value_ptr(camera.GetPosition()));
	}
	else
	{
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, 1.0f, 1.0f, 1.0f);
		glUniform1f(specloc, 0.1f);
		glUniform1f(ambLoc, 0.8f);
		glUniform3fv(objectColorLoc, 1, glm::value_ptr(glm::vec3(1, 1, 1)));
		glUniform3fv(cameraPosLoc, 1, glm::value_ptr(camera.GetPosition()));
	}

	//Bind the texture to the uniform variable 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(program, "Texture"), 0);
	
	//Bind, draw and unbind the vertex array 
	glBindVertexArray(vao);	
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()) , GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}
