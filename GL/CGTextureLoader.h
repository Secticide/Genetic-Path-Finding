
// CGTextureLoader.h

// Setup OpenGL textures from images using Windows Imaging Component (WIC).  This relies on services provided by the CGApp class


#pragma once

#include <GL\glew\glew.h>
#include <string>


class CGApp;


// structure to define properties for new textures
struct CGTextureGenProperties {

	GLint		minFilter;
	GLint		maxFilter;
	GLfloat		anisotropicLevel;
	GLint		wrap_s;
	GLint		wrap_t;
	bool		genMipMaps;

	CGTextureGenProperties(GLint _minf, GLint _maxf, GLfloat _af, GLint _ws, GLint _wt, bool _genmm) 
		:minFilter(_minf),
		maxFilter(_maxf),
		anisotropicLevel(_af),
		wrap_s(_ws),
		wrap_t(_wt),
		genMipMaps(_genmm) {}

};


enum CGMipmapGenMode {CG_NO_MIPMAP_GEN = 0, CG_CORE_MIPMAP_GEN, CG_EXT_MIPMAP_GEN};


class CGTextureLoader {

public:

	static void initialise(CGApp *hostApp); // initialise class - must be called before loadTexture methods otherwise these methods will return 0

	static GLuint loadTexture(const std::wstring& textureFilePath);
	static GLuint loadTexture(const std::wstring& textureFilePath, const CGTextureGenProperties& textureProperties);
};

