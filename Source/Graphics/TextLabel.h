//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2016 Media Design School
//
// File Name	: TextLabel.cpp
// Description	: Define, Update and Draw Text on the screen
// Author		: Cameron Peet and Charmaine Lim
// Mail			: Cameron.Peet@mediadesign.school.nz
// Mail			: Charmaine.Lim@mediadesign.school.nz
//


#pragma once


//Library Includes
#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <soil\SOIL.h>


//GL Math Includes
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>


#include "ShaderLoader.h"
#include "Camera.h"
#include "Utils.h"

#include "..\..\Dependencies\include\freetype\ft2build.h"
#include FT_FREETYPE_H  


#include <map>
#include <string>
#include <iostream>

struct Character
{
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

struct MyFont
{
	Character characters[128];
};


class TextLabel
{
public:
	TextLabel(std::string text, std::string font = 0);
	~TextLabel();

	void Render(Camera& camera);
	void setPosition(glm::vec3 _position);
	void setColor(glm::vec3 _color);
	void setHighlight(glm::vec3 _color) { highlight = _color; };
	void setScale(GLfloat _scale);
	
	void setText(std::string _text);
	void AddText(unsigned char c);
	void LoadFont(std::string path);
	
	void setActive(bool b){ m_bIsActive = b; };
	void setButton(bool b){	m_bIsButton = b; };
	void setHighlighted(bool b){ m_bIsHighlighted = b; };

	bool isActive() { return m_bIsActive; };
	bool isButton() { return m_bIsButton; };
	bool isHighlighted() { return m_bIsHighlighted; };

	std::string getText(){ return text; };
	glm::vec3 position;
	
	int width, height;
	
	bool m_bIsButton = true;
	bool m_bIsActive = true;
	bool m_bIsHighlighted = false;


private:
	std::string text;
	GLfloat scale;
	glm::vec3 color;
	glm::vec3 highlight;

	GLuint VAO, VBO, program;
	std::map<GLchar, Character> Characters;

	//For the sake of this smaller game, an unchanging shader can be static.
	//static GLuing program;
};

