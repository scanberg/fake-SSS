#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "mesh.h"
#include "model.h"
#include <vector>

Model* loadObj( const std::string &filename, float scale=1.0f );

#endif