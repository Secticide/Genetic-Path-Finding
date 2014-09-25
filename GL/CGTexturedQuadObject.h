
#pragma once

#include <CoreStructures\GUObject.h>
#include <CoreStructures\GUMatrix4.h>
#include <GL\glew\glew.h>


// model a simple textured quad oriented to face along the +z axis (so the textured quad faces the viewer in (right-handed) eye coordinate space.  The quad is modelled using VBOs and VAOs and rendered using the basic texture shader in Resources\Shaders\basic_texture.vs and Resources\Shaders\basic_texture.fs

class CGTexturedQuadObject : public CoreStructures::GUObject {

private:

	GLuint					quadVertexArrayObj;

	GLuint					quadVertexBuffer;
	GLuint					quadTextureCoordBuffer;

	GLuint					quadShader;

	GLuint					texture;

	//
	// Private API
	//

	void loadShader();
	void setupVAO();


	//
	// Public API
	//

public:

	CGTexturedQuadObject(const std::wstring& texturePath);
	CGTexturedQuadObject(GLuint initTexture);

	~CGTexturedQuadObject();

	void render(const CoreStructures::GUMatrix4& T);
};
