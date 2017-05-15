#pragma once

#include "CDebugDraw.h"



using namespace glm;


void CDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	// Changed these lines
	/*LINES.push_back(_LINE(from, to));
	COLORS.push_back(_COLOR(color));*/

	// to these
	LINES.push_back(_LINE(vec3(from.getX(), from.getY(), from.getZ()), vec3(to.getX(), to.getY(), to.getZ())));
	COLORS.push_back(_COLOR(vec3(color.getX(), color.getY(), color.getZ())));
}

void CDebugDraw::doDrawing()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, LINES.size() * sizeof(_LINE), &LINES[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, COLORS.size() * sizeof(_COLOR), &COLORS[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void CDebugDraw::cleanDrawing()
{
	// delete buffers
	glDeleteBuffers(2, vbo);
	glDeleteBuffers(1, &vao);
}