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

// Load the obj-data and pack into a single Geometry
bool loadObj( Geometry &geom, const std::string &filename, float scale = 1.0f );

// Load the obj-data and fill a vector of Geometry
bool loadObj( std::vector<Geometry> &geomList, const std::string &filename, float scale = 1.0f );

#endif
