// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2016 Media Design School
//
// File Name	: Camera.cpp
// Description	: Provide control over the view of rendered components
// Author		: Cameron Peet
// Mail			: Cameron.Peet@mediadesign.school.nz

#include "Camera.h"
/*
@Author : Cameron Peet
@Purpose : Default Constructor without initialising view componenets. Camera not yet "Active"
*/
Camera::Camera() 
	: m_Viewport(0), 
	m_Position(0),
	m_Rotation(), 
	m_ProjectionMatrix(1),
	m_ViewMatrix(1)
	{ }

/*
@Author : Cameron Peet
@Purpose : Construct the camera with the neccessary parameters to initialise an active camera
*/
Camera::Camera(int _iWindowWidth, int _iWindowHeight)
	: m_Viewport(0),
	m_Position(0),
	m_Rotation(),
	m_ProjectionMatrix(1),
	m_ViewMatrix(1)
{ 
	m_Viewport = glm::vec4(0, 0, _iWindowWidth, _iWindowHeight);
	glViewport(0, 0, _iWindowWidth, _iWindowHeight);
}


void Camera::SetViewPort(int x, int y, int width, int height)
{
	m_Viewport = glm::vec4(x, y, width, height);
	glViewport(x, y, width, height);
}

void Camera::SetPosition(const glm::vec3& pos)
{
	m_Position = pos;
	internalUpdateRequired = true;
}

void Camera::SetProjection(float fov, float aspectRatio, float zNear, float zFar)
{
	m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);
}

glm::vec3 Camera::GetPosition() const
{
	return m_Position;
}



void Camera::Translate(const glm::vec3&delta, bool local)
{
	if (local)
	{
		m_Position += m_Rotation * delta;
	}
	else
	{
		m_Position += delta;
	}
	internalUpdateRequired = true;
}

void Camera::SetRotation(const glm::quat & rot)
{
	m_Rotation = rot;
	internalUpdateRequired = true;
}

glm::quat Camera::GetRotation() const
{
	return m_Rotation;
}

void Camera::SetEulerAngles(const glm::vec3& eulerAngles) 
{
	m_Rotation = glm::quat(glm::radians(eulerAngles));
}

glm::vec3 Camera::GetEulerAngles() const
{
	return glm::degrees(glm::eulerAngles(m_Rotation));
}

void Camera::Rotate(const glm::quat& rot)
{
	m_Rotation = m_Rotation * rot;
	internalUpdateRequired = true;
}


glm::mat4 Camera::GetProjectionMatrix()
{
	return m_ProjectionMatrix;
}

glm::mat4 Camera::GetViewMatrix()
{
	UpdateViewMatrix();
	return m_ViewMatrix;
}

glm::vec4 Camera::GetViewport() const
{
	return m_Viewport;
}

void Camera::UpdateViewMatrix()
{
	if (internalUpdateRequired)
	{
		glm::mat4 translate = glm::translate(-m_Position);
		glm::mat4 rotate = glm::transpose(glm::mat4(m_Rotation));

		m_ViewMatrix = rotate * translate;

		internalUpdateRequired = false;
	}
}

glm::vec3 Camera::GetRayTo(int x, int y, float rayLength)
{
	int MAX_RAY_LENGTH = rayLength;

	//Calculate normal device coordinates of x and y
	float ndcX = (2.0f * x) / 1200.0f - 1.0f;
	float ndcY = 1.0f - (2.0f * y) / 800.0f;

	//Screen Positon in terms of OpenGL NDC coordinates
	glm::vec2 normalisedScreenCoord = glm::vec2(ndcX, ndcY);

	//Screen Position to projection space
	glm::vec4 clipCoord = glm::vec4(normalisedScreenCoord, -1.0f, 1.0f);

	//Projection Space to Eye Space
	glm::mat4 invProjMat = glm::inverse(GetProjectionMatrix());
	glm::vec4 eyeCoord = invProjMat * clipCoord;
	eyeCoord = glm::vec4(eyeCoord.x, eyeCoord.y, -1.0f, 0.0f);

	//Eye space to World space
	glm::mat4 invViewMat = glm::inverse(GetViewMatrix());
	glm::vec4 rayWorld = invViewMat * eyeCoord;
	glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

	return rayDirection *= MAX_RAY_LENGTH;
}
