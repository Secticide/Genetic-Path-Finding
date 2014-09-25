
//  GLSLFactory.h - this version uses the new OpenGL 2.0 entry points rather than the older ARB extension entry points

#pragma once

#include <CoreStructures\GUObject.h>
#include <GL\glew\glew.h>
#include <GL\glew\wglew.h>
#include <string>

// declare GLSL setup return / error codes
typedef enum GLSL_ERROR_CODES {
	
	GLSL_OK = 0,
	
	GLSL_VERTEX_SHADER_REQUIRED_ERROR, // returned if a geometry shader but no vertex shader is defined
	
	GLSL_SHADER_SOURCE_NOT_FOUND, // shader source string not found
	GLSL_SHADER_OBJECT_CREATION_ERROR, // shader object cannot be created by OpenGL
	GLSL_SHADER_COMPILE_ERROR, // shader object could not be compiled

	GLSL_VERTEX_SHADER_SOURCE_NOT_FOUND,
	GLSL_GEOMETRY_SHADER_SOURCE_NOT_FOUND,
	GLSL_FRAGMENT_SHADER_SOURCE_NOT_FOUND,
	
	GLSL_VERTEX_SHADER_OBJECT_CREATION_ERROR,
	GLSL_GEOMETRY_SHADER_OBJECT_CREATION_ERROR,
	GLSL_FRAGMENT_SHADER_OBJECT_CREATION_ERROR,
	
	GLSL_VERTEX_SHADER_COMPILE_ERROR,
	GLSL_GEOMETRY_SHADER_COMPILE_ERROR,
	GLSL_FRAGMENT_SHADER_COMPILE_ERROR,
	
	GLSL_PROGRAM_OBJECT_CREATION_ERROR,
	GLSL_PROGRAM_OBJECT_LINK_ERROR
	
} GLSL_ERROR;


class GLSLFactory : public CoreStructures::GUObject {

public:

	static GLSL_ERROR createShaderProgram(const std::string& vsPath, const std::string& fsPath, GLuint *result); // basic shader object creation function takes a path to a vertex shader file and fragment shader file and returns a bound and linked shader program object in *result.  No attribute bindings are specified in this function so it is assumed 'in' variable declarations have associated location declarations in the shader source file
	
	static GLSL_ERROR createShaderProgram(const std::string& vsPath, const std::string& fsPath, GLuint *result, int numAttributes, ...); // overload of createShaderProgram that allows vertex attribute locations to be declared in C/C++ and bound during shader creation.  This avoids the need to specify the layout in the vertex shader 'in' variable declarations, but this is not as convinient!

	static GLSL_ERROR createShaderProgram(const std::string& vsPath, const std::string& gsPath, const std::string& fsPath, GLuint *result); // overloaded version of createShaderProgram that takes a geometry shader filename as well as a vertex and fragment shader filename

};



// define constants that represent vertex attribute index bindings (this isn't part of using OpenGL or GLSL, but it's useful for us to adopt a standard to use throughout the application so we know which vertex packet locations different variables will be assigned to)

enum cg_va_binding {
	
	va_position_index = 0,
	va_colour_index = 1,
	va_normal_index = 2,
	va_texcoord_index = 3
};

