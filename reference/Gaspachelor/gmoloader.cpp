#include <iostream>
#include <fstream>
#include <vector>
#include "gmoloader.h"
#include "error.h"
#include "txttools.h"
#include "material.h"
#include "aseloader.h"
#include "matloader.h"
#include "objloader.h"

Model* loadMeshdata( const std::string &filename , float scale=1.0f);

Model* loadGmo(std::string filename)
{
    std::string param;
    std::string junk;
    
    std::ifstream file;
    
    file.open(filename.c_str(), std::ifstream::in);
    
    if ( file.fail() )
    {
        printError("readGMO", "Could not open file");
        file.close();
        return NULL;
    }
    
    std::cout<<"Reading GMO"<<std::endl;

    std::vector<Material*> matList;
    
    Model* model = NULL;
    
    file >> param;
    
    while ( !file.eof() )
    {
        if(param == "*Material")
        {            
            file >> junk;
            file >> param;

			Material* mat = loadMat(removeQuotes(param));
            std::cout<<"found material "<<param<<std::endl;
                
            matList.push_back(mat);
        }
        if(param == "*Mesh")
        {
            float scale;

            file >> param;
            file >> scale;

            model = loadMeshdata(removeQuotes(param), scale);
            std::cout<<"Found mesh: "<<model<<std::endl;

            if(!model)
                return NULL;
        }
        if(param == "*GroupMaterial")
        {
            int matIndex=0;
            file >> param;
            file >> matIndex;
            std::cout<<"Assigning material "<<matList[matIndex]<<" to group "<<param<<std::endl;
            model->assignMaterial(removeQuotes(param),matList[matIndex]);
        }
        file >> param;
    }

    file.close();
    
    std::cout<<"Done reading GMO"<<std::endl;

    model->prepare();

    return model;
}

Model* loadMeshdata( const std::string &filename , float scale)
{
    std::size_t found;
    std::string ending;

    found=filename.rfind(".");
    if (found!=std::string::npos)
    {
        ending = filename.substr(found+1);
        if(ending == "ase" || ending == "ASE")
        {
            std::cout<<"found ase"<<std::endl;
            return loadAse(filename, scale);
        }
        if(ending == "obj" || ending == "OBJ")
        {
            std::cout<<"found obj"<<std::endl;
            return loadObj(filename, scale);
        }
    }

    return NULL;
}