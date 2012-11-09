#ifndef MESH_H
#define MESH_H

#include <vector>
#include <map>
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/glfw.h>

#include "material.h"

class MeshObject;

class Mesh
{
public:
	friend void loadObj( Mesh *mesh, const std::string &filename, float scale );
	friend int loadAse(std::string filename, MeshObject* m);

	std::vector<glm::vec3> vertex;
	std::vector<glm::vec3> normal;
	std::vector<glm::vec3> tangent;
	std::vector<glm::vec3> color;
	std::vector<glm::vec2> texCoord;

	std::vector<glm::uvec3> face;

	GLuint buffers[6];
	GLuint vao;

	bool usedVectors[6];

	Mesh(){}
	~Mesh();

	bool hasVertex() 	{return usedVectors[0];}
	bool hasNormal() 	{return usedVectors[1];}
	bool hasTangent() 	{return usedVectors[2];}
	bool hasColor() 	{return usedVectors[3];}
	bool hasTexCoord() 	{return usedVectors[4];}
	bool hasFace() 		{return usedVectors[5];}

	void prepare();
	void process();

	void loadObj(const std::string &s);

	void createBuffers(Shader *shader);
	void updateAttribPointers(Shader *shader);

	void draw();
};

#endif