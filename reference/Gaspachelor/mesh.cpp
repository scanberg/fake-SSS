#include "mesh.h"
#include "error.h"

void calculateTangent(glm::vec3 &tangent, glm::vec3 v1, glm::vec3 v2, glm::vec2 st1, glm::vec2 st2);
void orthogonalizeTN(glm::vec3 &tangent, glm::vec3 &normal);

Mesh::~Mesh()
{

}

void Mesh::prepare()
{
	//for(unsigned int i=0; i<vertex.size(); i++)
		//color.push_back(glm::vec3(1.0,0.0,0.0));

	//usedVectors[3] = true;

	process();
}

void Mesh::updateAttribPointers(Shader *shader)
{
	GLint vl = shader->getAttributeLocation("in_position");
	GLint cl = shader->getAttributeLocation("in_color");
	GLint nl = shader->getAttributeLocation("in_normal");

	GLuint vertexLoc = (GLuint)vl;
	GLuint colorLoc = (GLuint)cl;
	GLuint normalLoc = (GLuint)nl;

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexLoc);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(colorLoc);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glVertexAttribPointer(normalLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(normalLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::createBuffers(Shader *shader)
{
	GLint vl = shader->getAttributeLocation("in_position");
	GLint cl = shader->getAttributeLocation("in_color");
	GLint nl = shader->getAttributeLocation("in_normal");
    GLint tl = shader->getAttributeLocation("in_texCoord");
    GLint tal = shader->getAttributeLocation("in_tangent");

	GLuint vertexLoc = (GLuint)vl;
	GLuint colorLoc = (GLuint)cl;
	GLuint normalLoc = (GLuint)nl;
    GLuint texCoordLoc = (GLuint)tl;
    GLuint tangentLoc = (GLuint)tal;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(6, buffers);

	// bind buffer for vertices and copy data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 3*vertex.size()*sizeof(float), &vertex[0][0], GL_STATIC_DRAW);
    if(vl > -1)
    {
        glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(vertexLoc);
    }
    else
        printWarning("createBuffers","vertex location not found");

	// bind buffer for color and copy data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, 3*color.size()*sizeof(float), &color[0][0], GL_STATIC_DRAW);
    if(cl > -1)
    {
        glVertexAttribPointer(colorLoc, 3, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(colorLoc);
    }
    else
        printWarning("createBuffers","color location not found");

	// bind buffer for normal and copy data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, 3*normal.size()*sizeof(float), &normal[0][0], GL_STATIC_DRAW);
    if(nl > -1)
    {
        glVertexAttribPointer(normalLoc, 3, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(normalLoc);
    }
    else
        printWarning("createBuffers","normal location not found");

    if(texCoord.size() == 0)
        printWarning("createBuffers","texCoord.size() is 0");

    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ARRAY_BUFFER, 2*texCoord.size()*sizeof(float), &texCoord[0][0], GL_STATIC_DRAW);
    if(tl > -1)
    {
        glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(texCoordLoc);
    }
    else
        printWarning("createBuffers","texture location not found");

    // bind buffer for tangent and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
    glBufferData(GL_ARRAY_BUFFER, 3*tangent.size()*sizeof(float), &tangent[0][0], GL_STATIC_DRAW);
    if(tal > -1)
    {
        glVertexAttribPointer(tangentLoc, 3, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(tangentLoc);
    }
    else
        printWarning("createBuffers","tangent location not found");

	// Create and bind a BO for index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[5]);
	// copy data into the buffer object
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * face.size() * sizeof(GLuint), &face[0][0], GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER,0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	glBindVertexArray(0);
}

void Mesh::draw()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 3 * face.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::process()
{
    int sharedFaces[vertex.size()];
    unsigned int i;
    float numFaces;
    
    glm::vec3 n,a,b,t;
    glm::vec2 st1, st2;
    
    for (i=0; i<(unsigned int)vertex.size(); i++)
    {
        normal.push_back(glm::vec3(0));
        tangent.push_back(glm::vec3(0));
        sharedFaces[i] = 0;
    }
    
    for (i=0; i<(unsigned int)face.size(); i++)
    {
        a = vertex[face[i].z] - vertex[face[i].x];
        b = vertex[face[i].y] - vertex[face[i].x];
        
        n = glm::cross(b,a);//b.cross(a);
        n = glm::normalize(n);

        if(texCoord.size()>0)
        {
        	st1 = texCoord[face[i][2]] - texCoord[face[i][0]];
        	st2 = texCoord[face[i][1]] - texCoord[face[i][0]];

        	calculateTangent(t,a,b,st1,st2);

        	t = glm::normalize(t);

        	tangent[face[i].x] += t;
        	tangent[face[i].y] += t;
        	tangent[face[i].z] += t;
        }
        
        normal[face[i].x] += n;
        normal[face[i].y] += n;
        normal[face[i].z] += n;
        
        sharedFaces[face[i].x]++;
        sharedFaces[face[i].y]++;
        sharedFaces[face[i].z]++;
    }
    for (i=0; i<(unsigned int)vertex.size(); i++)
    {
        if(sharedFaces[i]>0)
        {
            numFaces = (float)sharedFaces[i];
            normal[i] /= numFaces;

            normal[i] = glm::normalize(normal[i]);

            //if(i<tangent.size())
            //{
            	tangent[i] /= numFaces;
            	tangent[i] = glm::normalize(tangent[i]);
            //}

                //std::cout<<"TANGENT "<<tangent[i].x<<" "<<tangent[i].y<<" "<<tangent[i].z<<std::endl;

            //orthogonalizeTN(tangent[i],normal[i]);


        }
    }
}

void calculateTangent(glm::vec3 &tangent, glm::vec3 v1, glm::vec3 v2, glm::vec2 st1, glm::vec2 st2)
{
    float coef = 1.0 / (st1.x * st2.y - st2.x * st1.y);
    
    tangent.x = coef * ((v1.x * st2.y) + (v2.x * -st1.y));
    tangent.y = coef * ((v1.y * st2.y) + (v2.y * -st1.y));
    tangent.z = coef * ((v1.z * st2.y) + (v2.z * -st1.y));
}

void orthogonalizeTN(glm::vec3 &tangent, glm::vec3 &normal)
{
    tangent = tangent - glm::normalize(normal * glm::dot(normal,tangent));
}