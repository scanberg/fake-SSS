#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <glm/glm.hpp>

class BoundingBox
{
private:
	glm::vec3 min, max;
public:
	BoundingBox(){}
	~BoundingBox(){}

	bool pointInside(const glm::vec3 &point)
	{
		if(point.x < min.x || max.x < point.x) return false;
		if(point.y < min.y || max.y < point.y) return false;
		if(point.z < min.z || max.z < point.z) return false;
		return true;
	}
};

#endif