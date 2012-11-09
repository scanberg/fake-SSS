#include <set>

#include "error.h"
#include "model.h"

Model::Model()
{

}

Model::~Model()
{
	unsigned int i = 0;

	for(i=0; i<meshList.size(); i++)
	{
		delete meshList[i];
	}

	std::set<Material*> uniqueMat;

	for(i=0; i<matList.size(); i++)
	{
		uniqueMat.insert(matList[i]);
	}

	uniqueMat.erase(Material::getDefault());
	uniqueMat.erase(NULL);

	std::set<Material*>::iterator it = uniqueMat.begin();

	while(it != uniqueMat.end())
	{
		delete *it;
		it++;
	}
}

void Model::addMesh(Mesh* m, const std::string &name)
{
	if(!m)
	{
		printError("addMesh","Handle m is null");
		return;
	}

	meshMap.insert(std::pair<std::string, unsigned int>(name,meshList.size()));
	meshList.push_back(m);
	matList.push_back(Material::getDefault());
}

void Model::assignMaterial(const std::string &name, Material *material)
{
	std::map< std::string, unsigned int >::const_iterator it = meshMap.find(name);

	if(it != meshMap.end())
	{
		unsigned int index = it->second;

		matList[index] = material;
	}
}

void Model::draw(const glm::mat4 &projMat, const glm::mat4 &viewMat)
{
	unsigned int i = 0;

	for(i=0; i<meshList.size(); i++)
	{
		matList[i]->bind();

		glUniformMatrix4fv((GLuint)matList[i]->getShader()->getProjMatrixLocation(), 1, false, &projMat[0][0]);
		glUniformMatrix4fv((GLuint)matList[i]->getShader()->getViewMatrixLocation(), 1, false, &viewMat[0][0]);

		if(meshList[i])
			meshList[i]->draw();

		matList[i]->unbind();
	}
}

void Model::prepare()
{
	std::cout<<"Model::prepare()"<<std::endl;
	unsigned int i = 0;

	for(i=0; i<meshList.size(); i++)
	{
		std::cout<<"preparing "<<meshList[i]<<std::endl;
		meshList[i]->createBuffers(matList[i]->shader);
	}
}