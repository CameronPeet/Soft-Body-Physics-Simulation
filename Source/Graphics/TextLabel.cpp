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
// Author		: Cameron Peet
// Mail			: Cameron.Peet@mediadesign.school.nz
//

#include "TextLabel.h"

/**
*
* Constructor.
* (Task ID: Text Label)
*
* @author Cameron Peet
* @param text This is the text.
* @param font This is the font.
* @return void
*
*/
TextLabel::TextLabel(std::string text, std::string font) 
{

	this->text = text;
	this->color = glm::vec3(1.0, 1.0, 1.0);
	this->scale = 1.0;
	this->setPosition(position);
	height = 0;
	width = 0;

	ShaderLoader shaderLoader;
	program = 0;
	program = shaderLoader.CreateProgram("Assets/Shaders/text.vert", "Assets/Shaders/text.frag");
	if (program == 0)
	{
		assert(program);
	}

	width = 40 * (text.length() - 1);
	height = 48;
	MyFont* FontArray = new MyFont();
	{
		LoadFont(font);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/**
*
* Destructor.
* (Task ID: Text Label)
*
* @author Cameron Peet
* @param text void
* @return void
*
*/
TextLabel::~TextLabel() 
{
}

/**
*
* This function renders the text.
* (Task ID: Text Label)
*
* @author Cameron Peet
* @param camera This is the camera.
* @return void
*
*/
void TextLabel::Render(Camera& camera) 
{
	glm::vec3 textPos = this->position;

	//enable blending
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate corresponding render state	
	glUseProgram(program);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(1200.0f), 0.0f, static_cast<GLfloat>(800.0f));


	glm::mat4 model = glm::translate(model, position);
	//glm::mat4 view;
	//glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	if (m_bIsHighlighted)
	{
		glUniform3f(glGetUniformLocation(program, "textColor"), this->highlight.x, this->highlight.y, this->highlight.z);
	}
	else
	{
		glUniform3f(glGetUniformLocation(program, "textColor"), this->color.x, this->color.y, this->color.z);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	std::string::const_iterator c;
	width = 0;
	height = 0;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = textPos.x + ch.Bearing.x * this->scale;
		GLfloat ypos = textPos.y - (ch.Size.y - ch.Bearing.y) * this->scale;

		GLfloat w = ch.Size.x * this->scale;
		GLfloat h = ch.Size.y * this->scale;
		width += static_cast<int>(w);
		width += static_cast<int>(ch.Advance / 1000);
		if (height < h) height = static_cast<int>(h);
		// Update VBO for each character
		GLfloat vertices[6][4] = {
				{ xpos, ypos + h, 0.0, 0.0 },
				{ xpos, ypos, 0.0, 1.0 },
				{ xpos + w, ypos, 1.0, 1.0 },

				{ xpos, ypos + h, 0.0, 0.0 },
				{ xpos + w, ypos, 1.0, 1.0 },
				{ xpos + w, ypos + h, 1.0, 0.0 }
		};
		//width += w;
		//height += h;
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		// Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		textPos.x += (ch.Advance >> 6) * this->scale;
		// Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

}

/**
*
* This function sets the position.
* (Task ID: Text Label)
*
* @author Cameron Peet
* @param _position This is the position.
* @return void
*
*/
void TextLabel::setPosition(glm::vec3 _position) 
{

	this->position = _position;
}

/**
*
* This function sets the color.
* (Task ID: Text Label)
*
* @author Cameron Peet
* @param _color This is the color.
* @return void
*
*/
void TextLabel::setColor(glm::vec3 _color) 
{

	this->color = _color;
}

/**
*
* This function sets the scale.
* (Task ID: Text Label)
*
* @author Cameron Peet
* @param _scale This is the scale.
* @return void
*
*/
void TextLabel::setScale(GLfloat _scale) 
{

	this->scale = _scale;
}

/**
*
* This function sets the text.
* (Task ID: Text Label)
*
* @author Cameron Peet
* @param _text This is the text.
* @return void
*
*/
void TextLabel::setText(std::string _text) 
{

	this->text = _text;
	width = 40 * (text.length() - 1);
	height = 48;
}

/**
*
* This function adds the text.
* (Task ID: Text Label)
*
* @author Cameron Peet
* @param c This is the character.
* @return void
*
*/
void TextLabel::AddText(unsigned char c)
{
	if (c == '\b')
	{
		if (text.length() >= 1)
		{
			unsigned char old = text.at(text.length() - 1);
			text.erase(text.length() - 1);
			Character ch = Characters[old];
			GLfloat w = ch.Size.x / 2.0f;
			position += glm::vec3(w, 0, 0);
		}
	}
	else
	{
		text += c;
		width = 40 * (text.length() - 1);
		height = 48;
		Character ch = Characters[c];
		GLfloat w = 2 + ch.Size.x / 2.0f;
		position -= glm::vec3(w, 0, 0);

	}
}

/**
*
* This function loads the font.
* (Task ID: Text Label)
*
* @author Cameron Peet
* @param path This is the path.
* @return void
*
*/
void TextLabel::LoadFont(std::string path)
{
	// FreeType
	FT_Library ft;

	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	path.clear();
	if (path.size() == 0)
	{
		if (FT_New_Face(ft, "Assets/Fonts/autodestructbb_reg.ttf", 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}
	else
	{
		if (FT_New_Face(ft, path.c_str(), 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
			);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};

		Characters.insert(std::pair<GLchar, Character>(c, character));
	}

	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}