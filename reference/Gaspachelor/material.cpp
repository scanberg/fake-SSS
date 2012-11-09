#include "material.h"

Material Material::defaultMat;

Material::Material()
{
	shader = NULL;

	texture0	= UNUSED;
	texture1	= UNUSED;

	ambientColor	= glm::vec3(0.1f);
	diffuseColor	= glm::vec3(1.0f);
	specularColor	= glm::vec3(1.0f);
	specularFactor	= 10.0f;
	parallaxDepth 	= 16.0f;

	texture0Loc			= UNUSED;
	texture1Loc			= UNUSED;

	ambientColorLoc 	= UNUSED;
	diffuseColorLoc 	= UNUSED;
	specularColorLoc 	= UNUSED;
	specularFactorLoc	= UNUSED;
	parallaxDepthLoc	= UNUSED;
}

Material::~Material()
{
	if(shader)
		delete shader;
}

void Material::bind()
{
	if(shader)
	{
		shader->bind();
		
		//if(texture0 != UNUSED && texture0Loc != UNUSED)
		//{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture0);
			glUniform1i(texture0Loc,0);
		//}
		//if(texture1 != UNUSED && texture1Loc != UNUSED)
		//{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glUniform1i(texture1Loc,1);

			glUniform1f(parallaxDepthLoc,parallaxDepth);
		//}
	}
}

void Material::unbind()
{
	if(shader)
		shader->unbind();
}

Material *Material::getDefault()
{
	if(!defaultMat.shader)
		defaultMat.shader = new Shader("media/shaders/basic.glsl");

	return &defaultMat;
}