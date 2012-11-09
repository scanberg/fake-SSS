#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/glfw.h>
#include "objloader.h"

Model* loadObj( const std::string &filename, float scale )
{

    std::string line,param;
    
    std::ifstream file;
    
    file.open(filename.c_str(), std::ios::in);
    
    std::cout<<"loading .obj file"<<std::endl;
    
    if(file.fail())
    {
        std::cout<<"loadObj failed, could not read "<<filename<<std::endl;
        return NULL;
    }

    Model *model = new Model();
    Mesh *g;

    while( !file.eof() && file.good() )
    {
        std::getline(file,line);
        std::istringstream ss(line, std::istringstream::in);
        
        //std::cout<<"line "<<line<<std::endl;

        ss >> param;

        if(param == "g")
        {

        }
        if(param == "v")
        {
            glm::vec3 v;
            ss >> v.x;
            ss >> v.y;
            ss >> v.z;
            
            //std::cout<<"v "<<v.x<<" "<<v.y<<" "<<v.z<<std::endl;
            v *= scale;
            g->usedVectors[0]=true;
            g->vertex.push_back(v);
        }
        
        if(param == "f")
        {
            glm::uvec3 f;
            unsigned int index;
            
            //std::cout<<"f ";
            
            unsigned long pos = line.find('/');
            //std::cout<<(pos == std::string::npos);
            if(pos == std::string::npos)
            {
                ss >> index;
                f[0] = index - 1;
                //std::cout<<index<<" ";
                
                ss >> index;
                f[1] = index - 1;
                //std::cout<<index<<" ";
                
                ss >> index;
                f[2] = index - 1;
                //std::cout<<index;
                
            }
            else
            {
                unsigned long begin;
                param = line;
                
                for (int i=0; i<3; ++i)
                {
                    begin = param.find(' ') + 1;
                    pos = param.find('/',begin);
                    std::istringstream iss(param.substr(begin,pos),std::istringstream::in);
                    
                    param = param.substr(begin,param.size()-1);
                    iss >> index;
                    f[i] = index - 1;
                }
            }
            g->usedVectors[5]=true;
            g->face.push_back(f);

        }
        //std::cout<<"loop";
        if(file.eof())
            break;
    }
    file.close();

    return model;
}