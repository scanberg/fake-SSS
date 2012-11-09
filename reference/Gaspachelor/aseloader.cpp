#include <iostream>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include "error.h"
#include "txttools.h"
#include "mesh.h"
#include "aseloader.h"

//Temporary structs
typedef struct
{
	float x,y,z;
    float s,t;
}tVertex;

typedef struct
{
    float s,t;
}tTexCoord;

typedef struct
{
	unsigned int sg;
    unsigned int point[3];
}tFace;

void fixShadingGroups(std::vector<tVertex> &v, std::vector<tFace> &f);
bool existInVector(std::vector<int> &v, int value);
void duplicateVertex(unsigned int index, unsigned int sg, std::vector<tVertex> &v, std::vector<tFace> &f);
void copyGeometry(Mesh &g, std::vector<tVertex> &v, std::vector<tFace> &f);

Model* loadAse(std::string filename, float scale)
{
    std::cout<<"Reading ASE"<<std::endl;
    
    std::string param;
    std::string junk;
    
    std::ifstream file;
    
    file.open(filename.c_str(), std::ifstream::in);
    
    if ( file.fail() )
    {
        printError("AseReader", "Could not open file");
        return NULL;
    }

    Model *m = new Model();
    Mesh *g;
    
    int iparam;
    
    file >> param;
    
    while(!file.eof())
    {
        if (param == "*GEOMOBJECT")
        {
            std::string name;
            std::vector<tVertex> vertex;
            std::vector<tFace> face;
            std::vector<tTexCoord> tex;
            
            int numVertex=0;
            int numFace=0;
            int numTexCoord=0;
            
            tVertex tempVert;
            tFace tempFace;
            tTexCoord tempTex;
            
            file >> junk; file >> junk;
            file >> param;
            name = removeQuotes(param);
            
            skipTo(file, "*MESH_NUMVERTEX");
            file >> numVertex;
            
            skipTo(file, "*MESH_NUMFACES");
            file >> numFace;
            
            skipTo(file, "*MESH_VERTEX_LIST");
            file >> junk; //{
            
            for(int i=0; i<numVertex; i++)
            {
            	file >> junk; file >>junk;
                file >> tempVert.x;
                file >> tempVert.y;
                file >> tempVert.z;

                tempVert.x *= scale;
                tempVert.y *= scale;
                tempVert.z *= scale;
                
                vertex.push_back(tempVert);
            }
            
            skipTo(file, "*MESH_FACE_LIST");
            file >> junk; //{
            
            for(int i=0; i<numFace; i++)
            {
                file >> junk; file >> junk; file >> junk;
                file >> tempFace.point[0]; file >> junk;
                file >> tempFace.point[1]; file >> junk;
                file >> tempFace.point[2];
                file >> junk; file >> junk; file >> junk;
                file >> junk; file >> junk; file >> junk;
                file >> junk; //*MESH_SMOOTHING
                file >> tempFace.sg;
                file >> junk; file >> junk;
                
                face.push_back(tempFace);
            }
            
            fixShadingGroups(vertex,face);
            
            skipTo(file, "*MESH_NUMTVERTEX");
            file >> numTexCoord;
            
            skipTo(file, "*MESH_TVERTLIST");
            file >> junk; //{
            
            for(int i=0; i<numTexCoord; i++)
            {
                file >> junk; file >> junk;
                file >> tempTex.s;
                file >> tempTex.t;
                file >> junk;
                
                tex.push_back(tempTex);
            }
            
            skipTo(file, "*MESH_TFACELIST");
            file >> junk;//{
            
            for(int i=0; i<numFace; i++)
            {
                file >> junk; file >> junk;
                
                file >> iparam;
                vertex[face[i].point[0]].s = tex[iparam].s;
                vertex[face[i].point[0]].t = tex[iparam].t;

                file >> iparam;
                vertex[face[i].point[1]].s = tex[iparam].s;
                vertex[face[i].point[1]].t = tex[iparam].t;
                
                file >> iparam;
                vertex[face[i].point[2]].s = tex[iparam].s;
                vertex[face[i].point[2]].t = tex[iparam].t;
            }
            
            std::cout<<name<<": vertices="<<vertex.size()<<" faces="<<face.size()<<std::endl;
            g = new Mesh();
            copyGeometry(*g,vertex,face);
            g->process();
            m->addMesh(g,name);
        }
        file >> param;
    }
    
    std::cout<<"Done reading ASE"<<std::endl;
    return m;
}

void copyGeometry(Mesh &g, std::vector<tVertex> &v, std::vector<tFace> &f)
{
    std::vector<tVertex>::iterator vit = v.begin();
    
    while (vit != v.end())
    {
        g.vertex.push_back(glm::vec3(vit->x,vit->y,vit->z));
        g.texCoord.push_back(glm::vec2(vit->s,vit->t));
        vit++;
    }
    
    std::vector<tFace>::iterator fit = f.begin();
    
    while (fit != f.end())
    {
        g.face.push_back(glm::uvec3(fit->point[0],fit->point[1],fit->point[2]));
        fit++;
    }
}

void fixShadingGroups(std::vector<tVertex> &v, std::vector<tFace> &f)
{
    std::vector<int> groups;
    
    unsigned int vsize = v.size();
    for (unsigned int i = 0; i < vsize; i++)
    {
        groups.clear();
        for(unsigned int u=0; u<f.size(); u++)
        {
        	if(f[u].point[0] == i || f[u].point[1] == i || f[u].point[2] == i)
            {
                if(!existInVector(groups,f[u].sg))
                {
                    if(groups.size()>0)
                    	duplicateVertex(i,f[u].sg,v,f);
                    
                	groups.push_back(f[u].sg);	
                }
            }
    	}
    }
}

bool existInVector(std::vector<int> &v, int value)
{
    for(unsigned int i = 0; i < v.size(); i++)
    {
        if(v[i] == value)
        {
            return true;
        }
    }
    return false;
}

void duplicateVertex(unsigned int index, unsigned int sg, std::vector<tVertex> &v, std::vector<tFace> &f)
{
    tVertex vert;
    vert.x = v[index].x;
    vert.y = v[index].y;
    vert.z = v[index].z;
    
    v.push_back(vert);

    for(unsigned int u=0; u<f.size(); u++)
    {
        if(f[u].sg == sg)
        {
        	for(int p=0; p<3; p++)
        	{
            	if(f[u].point[p] == index)
                {
                    f[u].point[p] = v.size()-1;
                }
        	}
        }
    }
}

