
// ---------------------------------------------------------------------
// PGround Implementation
// ---------------------------------------------------------------------

#include "PTile.h"

// Use Core Structures / std namespaces
using namespace CoreStructures;
using namespace std;

// ---------------------------------------------------------------------
// PRIVATE
// ---------------------------------------------------------------------

// Precondition: n/a
// Postcondition: Shader loaded from file
void PTile::loadShader()
{
	// Create Shader
	GLSL_ERROR glsl_err = GLSLFactory::createShaderProgram(string("Resources\\shaders\\basic_texture.vs"), string("Resources\\Shaders\\basic_texture.fs"), &shader);
}

// Precondition: n/a
// Postcondition: Texture loaded from file
void PTile::loadTexture()
{
	// Load Texture
	texture = CGTextureLoader::loadTexture(wstring(L"Resources\\Textures\\ant.png"), CGTextureGenProperties(GL_LINEAR, GL_LINEAR, 2.0f, GL_REPEAT, GL_REPEAT, true));
}

// Precondition: n/a
// Postcondition: Vertex/Texture coords and indices setup
void PTile::setupVertexData()
{
	// Setup of Vertex Data
	vertices[0] = GUVector4(0.0, 0.0, 0.0);
	vertices[1] = GUVector4(0.0, 0.0, 1.0);
	vertices[2] = GUVector4(1.0, 0.0, 0.0);
	vertices[3] = GUVector4(1.0, 0.0, 1.0);

	// Setup of Texture Coordinate Data
	texcoord[0] = GUVector2(0.0, 0.0);
	texcoord[1] = GUVector2(0.0, 1.0);
	texcoord[2] = GUVector2(1.0, 0.0);
	texcoord[3] = GUVector2(1.0, 1.0);

	// Setup of Index data
	indices[0] = 2;
	indices[1] = 0;
	indices[2] = 1;
	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;

	// Setup of Index data (for lines)
	lineIndices[0] = 0;
	lineIndices[1] = 1;
	lineIndices[2] = 3;
	lineIndices[3] = 2;
}

// Precondition: n/a
// Postcondition: VAO and VBOs setup 
void PTile::setupVertexArrayObject()
{
	// Setup VAO
	glGenVertexArrays(1, &VAO_tile);
	glBindVertexArray(VAO_tile);
	
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

	// Setup VBO - lineIndicies
	glGenBuffers(1, &VBO_lineIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_lineIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lineIndices), lineIndices, GL_STATIC_DRAW);
	
	// Unbind VAO
	glBindVertexArray(0);
}

// ---------------------------------------------------------------------
// PUBLIC
// ---------------------------------------------------------------------

// Constructor
PTile::PTile()
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
	VAO_tile		= 0;
	VBO_texture		= 0;
	VBO_position	= 0;

	// Texture
	texture			= 0;
}

PTile::PTile(GUVector4 pos)
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
	VAO_tile		= 0;
	VBO_texture		= 0;
	VBO_position	= 0;

	// Texture
	texture			= 0;

	// And set position
	position = pos;

	// Run init
	Initialise();
}

// Deconstructor
PTile::~PTile()
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
	glDeleteVertexArrays(1, &VAO_tile);

	// Delete shader
	glDeleteShader(shader);
}

// Initialise
// Precondition: n/a
// Postcondition: Vertex data setup, texture and shader loaded, VAO setup
void PTile::Initialise()
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
void PTile::Render(const GUMatrix4& T)
{
	static GLint mvpLocation = glGetUniformLocation(shader, "mvpMatrix");

	GUMatrix4 MVP = T * GUMatrix4::translationMatrix(position);

	// Use basic shader
	glUseProgram(shader);
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&(MVP));

	// Bind VAO & Texture
	glUniform1ui(glGetUniformLocation(shader, "texture"), 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO_tile);

	// Draw Ground
	if(mode == FILL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_index);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const GLvoid*)0);
	}
	else if(mode == LINE)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_lineIndex);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (const GLvoid*)0);
	}
	
	// Unbind VAO & Texture
	glBindVertexArray(0);
}

// Getters and Setters
void PTile::setTexture(const wstring filePath)
{
	// Load Texture
	texture = CGTextureLoader::loadTexture(filePath, CGTextureGenProperties(GL_LINEAR, GL_LINEAR, 2.0f, GL_REPEAT, GL_REPEAT, true));
}

void PTile::setPosition(GUVector4 pos)
{
	position = pos;
}

void PTile::setPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}