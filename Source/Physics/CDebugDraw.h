#pragma once
#include <LinearMath/btIDebugDraw.h>
#include <vector>
#include <glew\glew.h>

//GL Math Includes
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using namespace glm;


class CDebugDraw :
	public btIDebugDraw
{
private:
	int m_debugMode;
public:
	CDebugDraw(void);
	virtual ~CDebugDraw(void);

	struct _LINE {
		vec3 from;
		vec3 to;

		_LINE(vec3 f, vec3 t) {
			from = f;
			to = t;
		}
	};

	std::vector<_LINE> LINES;

	struct _COLOR {
		vec3 col;

		_COLOR(vec3 c) {
			col = c;
		}
	};

	std::vector<_COLOR> COLORS;

	GLuint vao, vbo[2];

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

	virtual void reportErrorWarning(const char* warningString);

	virtual void draw3dText(const btVector3& location, const char* textString);

	virtual void setDebugMode(int m_debugMode);

	virtual int getDebugMode() const;

	void doDrawing();
	void cleanDrawing();
};
#pragma once
