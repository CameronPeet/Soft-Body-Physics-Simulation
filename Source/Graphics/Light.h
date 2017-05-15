#pragma once
#include "glm\glm.hpp"
class Light 
{
public:
	Light() {
		Position = glm::vec3(0.f, 0.f, 0.f);
		Color = glm::vec3(1.0f, 1.0f, 1.0f);
		float Intensity = 0.2f;
	}
	Light(glm::vec3 Position, glm::vec3 Color, float Intensity) {
		this->Position = Position;
		this->Color = Color;
		this->Intensity = Intensity;
	}

	glm::vec3 Position;
	glm::vec3 Color;
	float Intensity;

};