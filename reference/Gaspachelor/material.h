#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>

#include "shader.h"

#define UNUSED 9999

class Material
{
protected:
	static Material defaultMat;
	friend Material *loadMat(const std::string &filename);

	GLuint 		texture0; // rgb: diffuse, a: specular
	GLuint 		texture1; // rgb: normal, a: height

	glm::vec3	ambientColor;
	glm::vec3 	diffuseColor;
	glm::vec3 	specularColor;

	float		specularFactor;
	float 		parallaxDepth;

	GLuint 		texture0Loc;
	GLuint 		texture1Loc;

	GLuint 		ambientColorLoc;
	GLuint 		diffuseColorLoc;
	GLuint 		specularColorLoc;
	GLuint		specularFactorLoc;
	GLuint		parallaxDepthLoc;

public:

	Shader *shader;

	static Material* getDefault();
	Material();
	~Material();

	bool hasTexture0() { return texture0 != UNUSED; }
	bool hasTexture1() { return texture1 != UNUSED; }

	GLuint getTexture0Loc() { return texture0Loc; }
	GLuint getTexture1Loc() { return texture1Loc; }

	Shader *getShader() { return shader; }

	void bind();
	void unbind();
};

#endif