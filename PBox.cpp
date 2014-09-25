
// ---------------------------------------------------------------------
// PBox Implementation
// ---------------------------------------------------------------------

#include "PBox.h"

// Use Core Structures / std namespaces
using namespace CoreStructures;
using namespace std;

// ---------------------------------------------------------------------
// PRIVATE
// ---------------------------------------------------------------------

// Setup functions
// Precondition: n/a
// Postcondition: Shader loaded from file
void PBox::loadShader()
{
	// Create Shader
	GLSL_ERROR glsl_err = GLSLFactory::createShaderProgram(string("Resources\\shaders\\basic_texture.vs"), string("Resources\\Shaders\\basic_texture.fs"), &shader);
}

// Precondition: n/a
// Postcondition: Texture loaded from file
void PBox::loadTexture()
{
	// Load Texture
	texture = CGTextureLoader::loadTexture(wstring(L"Resources\\Textures\\box.png"), CGTextureGenProperties(GL_LINEAR, GL_LINEAR, 2.0f, GL_REPEAT, GL_REPEAT, true));
}

// Precondition: n/a
// Postcondition: Vertex/Texture coords and indices setup
void PBox::setupVertexData()
{
	// Setup of Vertex Data
	vertices[0] = GUVector4(0.0, 0.0, 0.0);
	vertices[1] = GUVector4(0.0, 0.0, 1.0);
	vertices[2] = GUVector4(1.0, 0.0, 0.0);
	vertices[3] = GUVector4(1.0, 0.0, 1.0);

	vertices[4] = GUVector4(0.0, 1.0, 0.0);
	vertices[5] = GUVector4(0.0, 1.0, 1.0);
	vertices[6] = GUVector4(1.0, 1.0, 0.0);
	vertices[7] = GUVector4(1.0, 1.0, 1.0);

	// Setup of Texture Coordinate Data
	texcoord[0] = GUVector2(0.0, 0.0);
	texcoord[1] = GUVector2(0.0, 1.0);
	texcoord[2] = GUVector2(1.0, 0.0);
	texcoord[3] = GUVector2(1.0, 1.0);

	texcoord[4] = GUVector2(0.0, 0.0);
	texcoord[5] = GUVector2(0.0, 1.0);
	texcoord[6] = GUVector2(1.0, 0.0);
	texcoord[7] = GUVector2(1.0, 1.0);

	// Setup of Index data
	// Top
	indices[0] = 6;
	indices[1] = 4;
	indices[2] = 5;
	indices[3] = 6;
	indices[4] = 5;
	indices[5] = 7;

	// Right
	indices[6] = 6;
	indices[7] = 7;
	indices[8] = 3;
	indices[9] = 6;
	indices[10] = 3;
	indices[11] = 2;

	// Front
	indices[12] = 7;
	indices[13] = 5;
	indices[14] = 1;
	indices[15] = 7;
	indices[16] = 1;
	indices[17] = 3;

	// Left
	indices[18] = 5;
	indices[19] = 4;
	indices[20] = 0;
	indices[21] = 5;
	indices[22] = 0;
	indices[23] = 1;

	// Back
	indices[24] = 4;
	indices[25] = 6;
	indices[26] = 2;
	indices[27] = 4;
	indices[28] = 2;
	indices[29] = 0;
}

// Precondition: n/a
// Postcondition: VAO and VBOs setup 
void PBox::setupVertexArrayObject()
{
	// Setup VAO
	glGenVertexArrays(1, &VAO_box);
	glBindVertexArray(VAO_box);
	
	// Setup VBO - Position
	glGenBuffers(1, &VBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	// Setup VBO - Texture Coordinates
	glGenBuffers(1, &VBO_texture);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);

	// Enable VBOs
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(3);

	// Setup VBO - indicies
	glGenBuffers(1, &VBO_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	// Unbind VAO
	glBindVertexArray(0);
}

// ---------------------------------------------------------------------
// PUBLIC
// ---------------------------------------------------------------------

// Constructor
PBox::PBox()
{
	// Set initial values
	for(int i = 0; i < 4; i += 1)
	{
		// Vertices
		vertices[i].x	= 0.0f;
		vertices[i].y	= 0.0f;
		vertices[i].z	= 0.0f;
		vertices[i].w	= 0.0f;

		// Texcoord
		texcoord[i].x	= 0.0f;
		texcoord[i].y	= 0.0f;

	}

	// VBOs & VAO
	VAO_box		= 0;
	VBO_texture		= 0;
	VBO_position	= 0;

	// Texture
	texture			= 0;

	// Set initial position to 0, 0, 0
	pos = GUVector4(0.0, 0.0, 0.0);

	// Set initial scale to 1, 1, 1
	scale = GUVector4(1.0, 1.0, 1.0);
}

// Deconstructor
PBox::~PBox()
{
	// Release memory
	// Unbind VAO
	glBindVertexArray(0);

	// Unbind the VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Delete VBOs
	glDeleteBuffers(1, &VBO_position);
	glDeleteBuffers(1, &VBO_index);
	glDeleteBuffers(1, &VBO_texture);

	// Delete VAO
	glDeleteVertexArrays(1, &VAO_box);

	// Delete shader
	glDeleteShader(shader);
}

// Overloaded Constructor
PBox::PBox(CoreStructures::GUVector4 position)
{
	// Set initial position
	pos = position;

	// Set initial values
	for(int i = 0; i < 4; i += 1)
	{
		// Vertices
		vertices[i].x	= 0.0f;
		vertices[i].y	= 0.0f;
		vertices[i].z	= 0.0f;
		vertices[i].w	= 0.0f;

		// Texcoord
		texcoord[i].x	= 0.0f;
		texcoord[i].y	= 0.0f;

	}

	// VBOs & VAO
	VAO_box		= 0;
	VBO_texture		= 0;
	VBO_position	= 0;

	// Texture
	texture			= 0;

}

// Initialise
// Precondition: n/a
// Postcondition: Vertex data setup, texture and shader loaded, VAO setup
void PBox::Initialise()
{
	// Setup vertex data
	setupVertexData();

	// Load resources
	loadTexture();
	loadShader();

	// Setup VAO
	setupVertexArrayObject();
}

// Render
// Precondition: Camera matrix
// Postcondition: Object rendered
void PBox::Render(const GUMatrix4& T)
{
	static GLint mvpLocation = glGetUniformLocation(shader, "mvpMatrix");

	// Create matrix for translation
	GUMatrix4 MVP = T * GUMatrix4::translationMatrix(pos) * GUMatrix4::scaleMatrix(scale.x, scale.y, scale.z);

	// Use basic shader
	glUseProgram(shader);
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&(MVP));

	// Bind VAO & Texture
	glUniform1ui(glGetUniformLocation(shader, "texture"), 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO_box);

	// Draw
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_index);
	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, (const GLvoid*)0);
	
	// Unbind VAO & Texture
	glBindVertexArray(0);
}

// ---------------------------------------------------------------------
// Getters and Setters
// ---------------------------------------------------------------------

GUVector4 PBox::getPosition()
{
	return pos;
}

void PBox::setPosition(GUVector4 position)
{
	pos = position;
}

void PBox::setPosition(float x, float y, float z)
{
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

void PBox::setTexture(const wstring filePath)
{
	// Load Texture
	texture = CGTextureLoader::loadTexture(filePath, CGTextureGenProperties(GL_LINEAR, GL_LINEAR, 2.0f, GL_REPEAT, GL_REPEAT, true));
}

void PBox::setScale(GUVector4 newScale)
{
	scale = newScale;
}