
#pragma once
#ifndef PTEXTURES
#define PTEXTURES

// Include
#include <vector>
#include <GL\glew\glew.h>
#include <GL\GLSLFactory.h>
#include <GL\CGTextureLoader.h>

// ---------------------------------------------------------------------
// PTextures (Project Textures) - Used to store all of the texture resources
// ---------------------------------------------------------------------

class PTextures
{
// ---------------------------------------------------------------------
private:
	// Simple structure to hold texture data and file path
	// Placed privately since only this class needs to use it.
	struct PTexture
	{
		// Where the texture is
		std::string filePath;

		// A Gluint to store the 'link' to the texture
		GLuint texture;
	};

	// Vector of texture resources
	std::vector<PTexture> textures;


// ---------------------------------------------------------------------
public:

	// Constructor / Deconstructor
	PTextures();
	~PTextures();

	// Add node (to linkedNodes list)
	void addTexture(std::string filePath);
};

#endif