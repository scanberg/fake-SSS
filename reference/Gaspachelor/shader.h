#ifndef SHADER_H
#define SHADER

#include <iostream>
#include <map>

#include <GL/glew.h>
#include <GL/glfw.h>

class Shader
{
protected:
	GLuint program;
	GLuint vertexShader, fragmentShader;

	GLuint projMatrixLoc, viewMatrixLoc;

	bool compiled;

	int createShader( const char *filename );

public:
	Shader();
	Shader( const std::string &filename );
	~Shader();

	GLint getAttributeLocation(const std::string &att);
	GLint getUniformLocation(const std::string &uni);

	GLuint getProjMatrixLocation() { return projMatrixLoc; }
	GLuint getViewMatrixLocation() { return viewMatrixLoc; }

	void init(const std::string &filename);

	bool isCompiled() { return compiled; }

	void bind();
	void unbind();
};

#endif