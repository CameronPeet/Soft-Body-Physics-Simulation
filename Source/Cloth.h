#pragma once
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2016 Media Design School
//
// File Name	: Model.h
// Description	: Encapsulate required functionality and components to define and render a predefined shape.
// Author		: Cameron Peet
// Mail			: Cameron.Peet@mediadesign.school.nz

//Local Includes
#include "Utils.h"
#include "Camera.h"
#include "Light.h"

//Library Includes
#include <vector>

struct Vertex2 { glm::vec3 pos; glm::vec2 uv; glm::vec3 n; };


/*
@Author : Cameron Peet
@Class : Model
@Purpose: Model Class - Encapsulate initialisation, rendering and positional control over an indiviudual model.
Allow for inidividual textures, and can be modified to have an individual shader program per requirements.
*/
class Cloth
{

public:
	Cloth();
	Cloth(std::vector<Vertex2> &points, std::vector<GLuint> &indices, char* filepath = nullptr);
	~Cloth();

	//GLuint program;

	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint texture;
	GLuint texture2;

	ModelType Shape;

	//Containers for model vertices and indices, allowing the model class to to define any shape constructible through primitives
	std::vector<Vertex2>		vertices;
	std::vector<GLuint>			indices;

	//Scale, Position and Rotation Components
	glm::quat m_Rotation;
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::vec3 m_RotationPoint;



	//Stored by the constructor for use in the initialiser.
	char* texturePath;

	//Render the model using an externaly provided camera for projection and view matrix, and an externally supplied program.
	virtual void Render(GLuint program, Camera& camera, Light* lightSource = nullptr);
	virtual void Initialise();


public:


	//Attributes
	float Specular;
	float Ambient;
	glm::vec3 ObjectColor;

};

class PhysicsBody
{

public:
	PhysicsBody();
	PhysicsBody(std::vector<Vertex2> &points, std::vector<GLuint> &indices, char* filepath = nullptr, bool DynamicDraw = false);
	~PhysicsBody();

	bool DynamicDraw = false;

	//GLuint program;

	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint texture;
	GLuint texture2;

	ModelType Shape;

	//Containers for model vertices and indices, allowing the model class to to define any shape constructible through primitives
	std::vector<Vertex2>		vertices;
	std::vector<GLuint>			indices;

	//Scale, Position and Rotation Components
	glm::quat m_Rotation;
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::vec3 m_RotationPoint;



	//Stored by the constructor for use in the initialiser.
	char* texturePath;

	//Render the model using an externaly provided camera for projection and view matrix, and an externally supplied program.
	virtual void Render(GLuint program, Camera& camera, Light* lightSource = nullptr);
	virtual void Initialise();


public:


	//Attributes
	float Specular;
	float Ambient;
	glm::vec3 ObjectColor;

};

