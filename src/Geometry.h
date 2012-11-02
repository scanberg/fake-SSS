#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Types.h"
#include <vector>

class Geometry
{


private:
    typedef struct
    {
        vec3 position;
        vec3 normal;
        vec2 texCoord;
    }s_vertex;

    std::vector<sVertex> v_vertices;
};

#endif
