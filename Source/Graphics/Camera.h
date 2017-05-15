// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2016 Media Design School
//
// File Name	: Camera.h
// Description	: Create and manage a "View" that is applied to rendered components.
// Author		: Cameron Peet
// Mail			: Cameron.Peet@mediadesign.school.nz
#pragma once

#include <glew\glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:
	Camera();
	Camera(int _iWindowWidth, int _iWindowHeight);

public:

	void SetViewPort(int x, int y, int width, int height);
	void SetProjection(float fov, float aspectRatio, float zNear, float zFar);
	void SetPosition(const glm::vec3& pos);
	void Translate(const glm::vec3& delta, bool local = true);
	void SetRotation(const glm::quat& rot);
	void SetEulerAngles(const glm::vec3& eulerAngles);
	void Rotate(const glm::quat& rot);

	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewMatrix();
	glm::vec4 GetViewport() const;
	glm::quat GetRotation() const;
	glm::vec3 GetEulerAngles() const;
	glm::vec3 GetPosition() const;


protected:

	void UpdateViewMatrix();
	glm::vec4 m_Viewport;
	glm::vec3 m_Position;
	glm::quat m_Rotation;
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;

private:
	bool internalUpdateRequired;

};