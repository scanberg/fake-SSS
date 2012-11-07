#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <map>

#ifdef WIN32
#include <GL/glew.h>
#endif
#include <GL/glfw.h>

class Shader
{
protected:
	GLuint program;
	GLuint vertexShader, fragmentShader;

	bool compiled;

	bool createShader( const char *filename );

public:
	Shader();
	Shader( const std::string &filename );
	~Shader();

	GLint getAttributeLocation(const std::string &att);
	GLint getUniformLocation(const std::string &uni);

	void init(const std::string &filename);

	bool isCompiled() { return compiled; }

	void bind();
	void unbind();
};

#endif