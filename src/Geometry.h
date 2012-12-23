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

    Geometry();
    ~Geometry();

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
    const Geometry& addGeometry(const Geometry &geometry);

    void translate(const vec3 &translation);
    void rotate(const vec3 &rotation);
    void scale(float scale);

    bool createStaticBuffers(GLint posLoc=0, GLint normLoc=1, GLint texLoc=2);
    bool createDynamicBuffers();
    inline bool existOnGpu() { return glIsVertexArray(vao); }
    bool updateBuffers();

    void destroyBuffers();

    void calculateNormals();
    void clear();

    void draw();

private:
    friend bool loadObj( std::vector<Geometry> &geomList, const std::string &filename, float scale );

    u32 vao;
    u32 vbo_vertex;
    u32 vbo_triangle;

    std::vector<sVertex> vertices;
    std::vector<uvec3> triangles;
};
    
#endif
