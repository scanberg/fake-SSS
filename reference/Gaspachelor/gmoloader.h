#ifndef GMOLOADER_H
#define GMOLOADER_H

class model;

#include <iostream>
#include "model.h"

Model* loadGmo(std::string filename);

#endif