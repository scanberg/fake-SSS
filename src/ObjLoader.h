/**
* ObjLoader.h
* Functions to parse .obj files and store inside Geometry
*
* Author: Robin Skånberg
*/

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include "Geometry.h"
#include "Tokenizer.h"

#define LOADOBJ_IGNORE_NORMALS 		0x01
#define LOADOBJ_IGNORE_TEXCOORDS	0x02

// Load the obj-data and pack into a single Geometry
bool loadObj( Geometry &geom, const std::string &filename, float scale = 1.0f , int flags = 0);

// Load the obj-data and fill a vector of Geometry
bool loadObj( std::vector<Geometry> &geomList, const std::string &filename, float scale = 1.0f, int flags = 0);

#endif
