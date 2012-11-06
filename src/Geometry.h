#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Types.h"
#include <vector>
#include <cassert>
#include <string>

class Geometry
{
public:
    /** Structure of Vertex used in Geometry **/
    typedef struct
    {
        vec3 position;
        vec3 normal;
        vec2 texCoord;
    }sVertex;

    /** GET functions **/
    u32 getVertexSize();
    u32 getTriangleSize();

    const vec3 &getVertexPosition(const u32 &vertexIndex) const;
    const vec3 &getVertexNormal(const u32 &vertexIndex) const;
    const vec2 &getVertexTextCoord(const u32 &vertexIndex) const;

    const uvec3 &getTriangleIndices(const u32 &triangleIndex) const;
    const vec3 &getTrianglePoint(const u32 &triangleIndex, const i32 &point) const;

    const f32 *getVertexData();
    const u32 *getTriangleData();

    /** Functions **/
    void addVertex(const sVertex &vertex);
    void addTriangle(const uvec3 &triangle);
    void addGeometry(const Geometry &geometry);

    void calculateNormals();

    void clear();

private:
    friend int loadObj( std::vector<Geometry> &geomList, const std::string &filename, float scale );

    std::vector<sVertex> vertices;
    std::vector<uvec3> triangles;
};

#endif
