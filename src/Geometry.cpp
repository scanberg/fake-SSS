#include "Geometry.h"

#include "Log.h"

namespace glen
{
    Geometry::Geometry()
    {
        vao = UNUSED_ADRESS;
        vbo_vertex = UNUSED_ADRESS;
        vbo_triangle = UNUSED_ADRESS;
    }

    Geometry::~Geometry()
    {
        destroyBuffers();
    }

    u32 Geometry::getVertexSize() { return vertices.size(); }
    u32 Geometry::getTriangleSize() { return triangles.size(); }

    const vec3 &Geometry::getVertexPosition(const u32 &vertexIndex) const
    {
        assert(vertexIndex < vertices.size());
        return vertices[vertexIndex].position;
    }

    const vec3 &Geometry::getVertexNormal(const u32 &vertexIndex) const
    {
        assert(vertexIndex < vertices.size());
        return vertices[vertexIndex].normal;
    }

    const vec2 &Geometry::getVertexTextCoord(const u32 &vertexIndex) const
    {
        assert(vertexIndex < vertices.size());
        return vertices[vertexIndex].texCoord;
    }

    const uvec3 &Geometry::getTriangleIndices(const u32 &triangleIndex) const
    {
        assert(triangleIndex < triangles.size());
        return triangles[triangleIndex];
    }

    const vec3 &Geometry::getTrianglePoint(const u32 &triangleIndex, const i32 &point) const
    {
        assert(triangleIndex < triangles.size());
        assert(0 < point && point < 3);
        return vertices[ triangles[triangleIndex][point] ].position;
    }

    const f32 *Geometry::getVertexData()
    {
        if(vertices.size() > 0)
            return &vertices[0].position[0];

        return NULL;
    }

    const u32 *Geometry::getTriangleData()
    {
        if(triangles.size() > 0)
            return &triangles[0][0];

        return NULL;
    }

    void Geometry::addVertex(const sVertex &vertex)
    {
        vertices.push_back(vertex);
    }

    void Geometry::addTriangle(const uvec3 &triangle)
    {
        triangles.push_back(triangle);
    }

    const Geometry& Geometry::addGeometry(const Geometry &geom)
    {
        u32 vertexOffset = vertices.size();
        uvec3 tri;

        for(u32 i=0; i<geom.vertices.size(); ++i)
        {
            vertices.push_back(geom.vertices[i]);
        }

        for(u32 i=0; i<geom.triangles.size(); ++i)
        {
            tri = geom.triangles[i];
            tri[0] += vertexOffset;
            tri[1] += vertexOffset;
            tri[2] += vertexOffset;

            triangles.push_back(tri);
        }

        return *this;
    }

    void Geometry::clear()
    {
        vertices.clear();
        triangles.clear();
    }

    void Geometry::calculateNormals()
    {
        glm::vec3 a,b,n;

        std::vector<vec3> tempNormal;
        tempNormal.resize(vertices.size(),vec3(0));

        assert(vertices.size() > 0);

        std::vector<i32> sharedFaces;
        sharedFaces.resize(vertices.size(), 0);

        for (u32 i=0; i<triangles.size(); ++i)
        {
            a = vertices[triangles[i][2]].position - vertices[triangles[i][0]].position;
            b = vertices[triangles[i][1]].position - vertices[triangles[i][0]].position;
            n = glm::normalize(glm::cross(b,a));

            for(u32 u=0; u<3; ++u)
            {
                tempNormal[triangles[i][u]] += n;
                sharedFaces[triangles[i][u]]++;
            }
        }
        for (u32 i=0; i<vertices.size(); ++i)
        {
            if(sharedFaces[i]>0)
            {
                tempNormal[i] /= (f32)sharedFaces[i];
                tempNormal[i] = glm::normalize(tempNormal[i]);
            }
            if(glm::dot(vertices[i].normal,vertices[i].normal) == 0.0f)
            {
                vertices[i].normal = tempNormal[i];
            }
        }
    }

    bool Geometry::createStaticBuffers(GLint posLoc, GLint normLoc, GLint texLoc)
    {
        destroyBuffers();

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo_vertex);
        glGenBuffers(1, &vbo_triangle);

        // bind buffer for vertices and copy data into buffer
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(sVertex), &vertices[0].position[0], GL_STATIC_DRAW);

        // Enable specific pointer for Vertex, for compability-mode and attributepointer for shader
        //glEnableClientState(GL_VERTEX_ARRAY);
        //glVertexPointer(3, GL_FLOAT, sizeof(sVertex), (char*)NULL);

        if(posLoc > -1)
        {
            glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (char*)NULL);
            glEnableVertexAttribArray(posLoc);
        }
        else
        {
            logWarning("posLoc in createStaticBuffer was undefined");
        }

        // Enable specific pointer for Normal, for compability-mode and attributepointer for shader
        //glEnableClientState(GL_NORMAL_ARRAY);
        //glNormalPointer(GL_FLOAT, sizeof(sVertex), (char*)NULL+3*sizeof(f32));

        if(normLoc > -1)
        {
            glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (char*)NULL+3*sizeof(f32));
            glEnableVertexAttribArray(normLoc);
        }
        else
        {
            logWarning("normLoc in createStaticBuffer was undefined");
        }

        // Enable specific pointer for TextureCoord, for compability-mode and attributepointer for shader
        //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        //glTexCoordPointer(2, GL_FLOAT, sizeof(sVertex), (char*)NULL+6*sizeof(f32));

        if(texLoc > -1)
        {
            glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(sVertex), (char*)NULL+6*sizeof(f32));
            glEnableVertexAttribArray(texLoc);
        }
        else
        {
            logWarning("texLoc in createStaticBuffer was undefined");
        }

        // Create and bind a BO for index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_triangle);
        // copy data into the buffer object
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size()*sizeof(uvec3), &triangles[0][0], GL_STATIC_DRAW);

        glBindVertexArray(0);

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

        logNote("createdStaticBuffers");
        logNote("vertices.size(): %i", vertices.size());
        logNote("triangles.size(): %i", triangles.size());
        logNote("sVertex size %i", sizeof(sVertex));

        return true;
    }

    void Geometry::destroyBuffers()
    {
        if(vbo_vertex != UNUSED_ADRESS)
            glDeleteBuffers(1, &vbo_vertex);

        if(vbo_triangle != UNUSED_ADRESS)
            glDeleteBuffers(1, &vbo_triangle);

        if(vao != UNUSED_ADRESS)
            glDeleteVertexArrays(1, &vao);

        vao=UNUSED_ADRESS;
        vbo_vertex=UNUSED_ADRESS;
        vbo_triangle=UNUSED_ADRESS;
    }

    void Geometry::draw()
    {
        if(vao != UNUSED_ADRESS)
        {
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 3 * triangles.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

}
