
#pragma once

#include <CoreStructures\GUObject.h>
#include <CoreStructures\GUMatrix4.h>
#include <GL\glew\glew.h>

class CGPrincipleAxesObject : public CoreStructures::GUObject {

private:

	GLuint					paVertexArrayObj;

	GLuint					paVertexBuffer;
	GLuint					paColourBuffer;
	
	GLuint					paIndexBuffer;

	GLuint					paShader;

public:

	CGPrincipleAxesObject();

	~CGPrincipleAxesObject();

	void render(const CoreStructures::GUMatrix4& T);
};
