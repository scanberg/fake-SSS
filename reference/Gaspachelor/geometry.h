#ifndef GEOMETRY_H
#define GEOMETRY_H

class Geometry
{
protected:
	std::vector<glm::vec3> vertex;
	std::vector<glm::vec3> normal;
	std::vector<glm::vec3> tangent;
	std::vector<glm::vec3> color;
	std::vector<glm::vec2> texCoord;

	std::vector<glm::uvec3> face;
public:
};

#endif