#include "Utils.h"
#include "Camera.h"

class GeometryModel
{
public:
	GeometryModel() {}
	GeometryModel(GLuint program, Camera* camera);
	void Render();

public:

	GLuint program;
	Camera* camera;

	glm::quat m_Rotation;
	glm::vec3 m_Position;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	glm::vec3 m_Scale;
	glm::vec3 m_RotationPoint;

};