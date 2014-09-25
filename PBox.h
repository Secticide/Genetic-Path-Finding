#pragma once
#ifndef PBOX
#define PBOX

// Include
#include <GL\glew\glew.h>
#include <GL\GLSLFactory.h>
#include <GL\CGTextureLoader.h>
#include <CoreStructures\GUMatrix4.h>
#include <CoreStructures\GUVector2.h>
#include <CoreStructures\GUVector4.h>

// ---------------------------------------------------------------------
// PBox (Project Box) - Used to render boxes (cubes) on the map
// ---------------------------------------------------------------------

class PBox
{
// ---------------------------------------------------------------------
private:

	// ATTRIBUTES

	// Position and scale vectors
	CoreStructures::GUVector4		pos;
	CoreStructures::GUVector4		scale;

	// Vertex, Index and texture coordinate data
	CoreStructures::GUVector4		vertices[8];
	CoreStructures::GUVector2		texcoord[8];
	GLuint							indices[30];
	
	// Vertex Array Object & Vertex Buffer Objects
	GLuint							VAO_box;
	GLuint							VBO_position;
	GLuint							VBO_index;
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

	// Constructor / Decontructor
	PBox();
	~PBox();

	// Overloaded Constructor to initilise Position
	PBox(CoreStructures::GUVector4 position);

	// Initialise
	void Initialise();

	// Render
	void Render(const CoreStructures::GUMatrix4& T);

	// Getters and Setters
	CoreStructures::GUVector4 getPosition();

	void setPosition(CoreStructures::GUVector4 position);
	void setPosition(float x, float y, float z);
	void setScale(CoreStructures::GUVector4 newScale);
	void setTexture(const std::wstring filePath);

};

#endif