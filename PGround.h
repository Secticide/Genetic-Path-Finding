#pragma once
#ifndef PGROUND
#define PGROUND

// Include
#include <GL\glew\glew.h>
#include <GL\GLSLFactory.h>
#include <GL\CGTextureLoader.h>
#include <CoreStructures\GUMatrix4.h>
#include <CoreStructures\GUVector2.h>
#include <CoreStructures\GUVector4.h>

// ---------------------------------------------------------------------
// PGround (Project Ground) - Used to render the ground surface
// ---------------------------------------------------------------------

class PGround
{
// ---------------------------------------------------------------------
private:

	// ATTRIBUTES

	// Vertex, Index and texture coordinate data
	CoreStructures::GUVector4		vertices[4];
	CoreStructures::GUVector2		texcoord[4];
	GLuint							indices[6];
	
	// Vertex Array Object & Vertex Buffer Objects
	GLuint							VAO_ground;
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
	PGround();
	~PGround();

	// Initialise
	void Initialise();

	// Render
	void Render(const CoreStructures::GUMatrix4& T);

};

#endif