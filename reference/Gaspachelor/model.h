#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "mesh.h"

class Model
{
protected:
	std::vector<Mesh*> meshList;
	std::vector<Material*> matList;

	std::map<std::string, unsigned int> meshMap;
public:
	Model();
	~Model();

	void prepare();

	void draw(const glm::mat4 &projMat, const glm::mat4 &viewMat);
	void addMesh(Mesh* m, const std::string &name);

	void assignMaterial(const std::string &name, Material *material);
};

#endif