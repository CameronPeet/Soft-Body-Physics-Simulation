#include "Terrain.h"

Terrain::Terrain(std::wstring heightmapFilename, std::string texFileName, std::string texFileName1, std::string texFileName2, GLuint program, Camera* camera, Light* light) {

	this->heightmapFilename = heightmapFilename;
	this->program = program;
	this->camera = camera;
	this->light = light;
	this->texFileName = texFileName;
	this->texFileName1 = texFileName1;
	this->texFileName2 = texFileName2;

	mNumVertices = numRows*numCols;
	mNumFaces = (numRows - 1)*(numCols - 1) * 2;

	loadHeightMap();
	smooth();
	createVerticesAndIndices();
	buildBuffers();
	texture = setTexture(texFileName);
	texture1 = setTexture(texFileName1);
	texture2 = setTexture(texFileName2);

	m_Position = glm::vec3(0.f, 0.f, 0.f);
	m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

Terrain::~Terrain() {
}

void Terrain::loadHeightMap() {

	// A height for each vertex
	std::vector<unsigned char> in(numRows * numCols);

	// Open the file.
	std::ifstream inFile;
	inFile.open(heightmapFilename.c_str(), std::ios_base::binary);

	if (inFile) {
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done with file.
		inFile.close();
	}

	// Copy the array data into a float array, and scale and offset the heights.
	heightmap.resize(numRows * numCols, 0);

	for (UINT i = 0; i < numRows * numCols; ++i) {

		heightmap[i] = (float)in[i] * heightScale + (float)heightOffset;
		//printf("heightmap data: %f, %d \n ", heightmap[i], i);
	}

	printf("heightmap size: %zd ", heightmap.size());

}

void Terrain::smooth() {

	std::vector<float> dest(heightmap.size() + 1);

	printf("heightmap size: %f ", heightmap.size());

	for (UINT i = 0; i < numRows - 2; ++i) {
		for (UINT j = 0; j < numCols - 2; ++j) {
			dest[i*numCols + j] = average(i, j);
		}
	}

	// Replace the old heightmap with the filtered one.
	heightmap = dest;

}

float Terrain::average(UINT i, UINT j) {
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	for (UINT m = i - 1; m <= i + 1; ++m) {
		for (UINT n = j - 1; n <= j + 1; ++n) {
			if (m >= 0 && i < numRows && n >= 0 && j < numCols) { // check if in bounds

				avg += heightmap[m*numCols + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

void Terrain::createVerticesAndIndices() {

	std::vector<VertexFormat> mvertices(mNumVertices);

	float halfWidth = (numCols - 1)* cellSpacing * 0.5f;
	float halfDepth = (numRows - 1)* cellSpacing * 0.5f;

	float du = 1.0f / (numCols - 1);
	float dv = 1.0f / (numRows - 1);
	for (UINT i = 0; i < numRows; ++i)
	{
		float z = halfDepth - i*cellSpacing;
		for (UINT j = 0; j < numCols; ++j)
		{
			float x = -halfWidth + j*cellSpacing;

			float y = heightmap[i*numCols + j];
			mvertices[i*numCols + j].VertexPosition = Position(x, y, z);
			mvertices[i*numCols + j].Normal = Normal(0.0f, 1.0f, 0.0f);
			mvertices[i*numCols + j].TexturePosition = TexCoord(j*du, i*dv);

			// Stretch texture over grid.
			//mvertices[i*numCols + j].texCoord.u = float(j*du);
			//mvertices[i*numCols + j].texCoord.v = float(i*dv);
		}
	}


	// Estimate normals for interior nodes using central difference.
	float invTwoDX = 1.0f / (2.0f*cellSpacing);
	float invTwoDZ = 1.0f / (2.0f*cellSpacing);
	for (UINT i = 2; i < numRows - 1; ++i)
	{
		for (UINT j = 2; j < numCols - 1; ++j)
		{
			float t = heightmap[(i - 1)*numCols + j];
			float b = heightmap[(i + 1)*numCols + j];
			float l = heightmap[i*numCols + j - 1];
			float r = heightmap[i*numCols + j + 1];

			glm::vec3 tanZ(0.0f, (t - b)*invTwoDZ, 1.0f);
			glm::vec3 tanX(1.0f, (r - l)*invTwoDX, 0.0f);

			glm::vec3 normal = glm::cross(tanZ, tanX);
			normal = glm::normalize(normal);


			Normal N = Normal(normal.x, normal.y, normal.z);

			//N = glm::normalize(N);
			//D3DXVec3Cross(&N, &tanZ, &tanX);
			//D3DXVec3Normalize(&N, &N);

			mvertices[i*numCols + j].Normal = N;
		}
	}


	// Iterate over each quad and compute indices.
	std::vector<GLuint> mindices(mNumFaces * 3); // 3 indices per face
	int k = 0;
	for (GLuint i = 0; i < numRows - 1; ++i) {
		for (GLuint j = 0; j < numCols - 1; ++j) {
			mindices[k] = i*numCols + j;
			mindices[k + 1] = i*numCols + j + 1;
			mindices[k + 2] = (i + 1)*numCols + j;

			mindices[k + 3] = (i + 1)*numCols + j;
			mindices[k + 4] = i*numCols + j + 1;
			mindices[k + 5] = (i + 1)*numCols + j + 1;

			k += 6; // next quad
		}
	}

	vertices = mvertices;
	indices = mindices;

	printf("vertices size: %zd", vertices.size());
}

void Terrain::buildBuffers() {

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	//Attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(offsetof(VertexFormat, VertexFormat::TexturePosition)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(offsetof(VertexFormat, VertexFormat::Normal)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


}

GLuint Terrain::setTexture(std::string  texFileName) {

	GLuint mtexture;

	//** load texture
	glGenTextures(1, &mtexture);
	glBindTexture(GL_TEXTURE_2D, mtexture);

	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//** loadImage and create texture
	// Load image, create texture and generate mipmaps
	int width, height;
	unsigned char* image = SOIL_load_image(texFileName.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);

	printf("fileName %s \n", texFileName.c_str());

	return mtexture;


	//bIsTextureSet = true;
}

void Terrain::Render() {
	glUseProgram(this->program);

	//if(bIsTextureSet)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(program, "texture"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glUniform1i(glGetUniformLocation(program, "texture1"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(program, "texture2"), 2);

	glm::mat4 model;
	//Translate the model according to its position and apply the rotation
	model = glm::translate(model, m_Position) * glm::mat4(m_Rotation);

	//Rotation points
	if (m_RotationPoint != glm::vec3(0, 0, 0))
		model = glm::translate(model, m_Position - m_RotationPoint);

	//Scale the model according to private member m_Scale
	model = glm::scale(model, m_Scale);
	glm::mat4 normalMatrix;
	normalMatrix = glm::transpose(glm::inverse(model * camera->GetViewMatrix()));

	glm::mat4 vp = camera->GetProjectionMatrix() * camera->GetViewMatrix();
	GLint vpLoc = glGetUniformLocation(program, "vp");
	glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(vp));

	GLint modelLoc = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//lighting Calculation
	GLint colorLoc = glGetUniformLocation(program, "objectColor");
	glUniform3f(colorLoc, ObjectColor.r, ObjectColor.g, ObjectColor.b);

	GLuint cameraPosLoc = glGetUniformLocation(program, "cameraPos");
	glUniform3f(cameraPosLoc, camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z);

	GLuint lightPosLoc = glGetUniformLocation(program, "lightPos");
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(light->Position));

	GLuint lightColorLoc = glGetUniformLocation(program, "lightColor");
	glUniform3f(lightColorLoc, this->light->Color.x, this->light->Color.y, this->light->Color.z);

	GLuint specularStrengthLoc = glGetUniformLocation(program, "specularStrength");
	glUniform1f(specularStrengthLoc, 0.1f);

	GLuint ambientStrengthLoc = glGetUniformLocation(program, "ambientStrength");
	glUniform1f(ambientStrengthLoc, 0.5f);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}