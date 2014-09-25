#pragma once
#ifndef PTILE
#define PTILE

// Include
#include <GL\glew\glew.h>
#include <GL\GLSLFactory.h>
#include <GL\CGTextureLoader.h>
#include <CoreStructures\GUMatrix4.h>
#include <CoreStructures\GUVector2.h>
#include <CoreStructures\GUVector4.h>

// Draw mode
enum DrawMode {FILL, LINE};

// ---------------------------------------------------------------------
// PTile (Project Tile) - Used to render the path
// ---------------------------------------------------------------------

class PTile
{
// ---------------------------------------------------------------------
private:

	// ATTRIBUTES

	// Position
	CoreStructures::GUVector4		position;

	// Vertex, Index and texture coordinate data
	CoreStructures::GUVector4		vertices[4];
	CoreStructures::GUVector2		texcoord[4];
	GLuint							indices[6];
	GLuint							lineIndices[4];
	
	// Vertex Array Object & Vertex Buffer Objects
	GLuint							VAO_tile;
	GLuint							VBO_position;
	GLuint							VBO_index;
	GLuint							VBO_lineIndex;
	GLuint							VBO_texture;

	// Texture
	GLuint							texture;

	// Shader
	GLuint							shader;



	// METHODS
	void loadShader();
	void loadTexture();
	void setupVertexData();
	void setupVertexArrayObject();

// ---------------------------------------------------------------------
public:

	DrawMode mode;

	// Constructor / Decontructor
	PTile();
	~PTile();

	PTile(CoreStructures::GUVector4 pos);

	// Initialise
	void Initialise();

	// Render
	void Render(const CoreStructures::GUMatrix4& T);

	// Getters and Setters
	void setTexture(const std::wstring filePath);
	void setPosition(CoreStructures::GUVector4 pos);
	void setPosition(float x, float y, float z);
};

#endif