#include <fstream>
#include <IL/il.h>
#include <IL/ilu.h>

#include <GL/glew.h>
#include <GL/glfw.h>

#include "txttools.h"
#include "error.h"
#include "matloader.h"


ILuint loadImage(const std::string &filename);
ILuint mergeRGB_A(ILuint rgb, ILuint alpha);
int loadTexture(const std::string &filename);
GLuint makeTexture(ILuint img);

Material *loadMat(const std::string &filename)
{
    std::string param;
    std::string junk;
    
    std::string texture0Uniform = "texture0";
    std::string texture1Uniform = "texture1";
    
    std::ifstream file;
    
    file.open(filename.c_str(), std::ifstream::in);
    
    if ( file.fail() )
    {
        file.close();
        printError("readMAT", "Could not open file");
        printError("file",filename);
        return NULL;
    }

    ILuint tex[2];
    ILuint map[4];

    std::string diffuseMap, normalMap;

    Material *m = new Material();
    
    std::cout<<"Reading MAT"<<std::endl;
    
    while ( !file.eof() )
    {
        if( param == "*DiffuseMap" )
        {
            file>>param;
            diffuseMap = removeQuotes(param);
            map[0] = loadImage(removeQuotes(param));
        }
        if( param == "*SpecularMap" )
        {
            file>>param;
            map[1] = loadImage(removeQuotes(param));
        }
        if( param == "*NormalMap" )
        {
            file>>param;
            normalMap = removeQuotes(param);
            map[2] = loadImage(removeQuotes(param));
        }
        if( param == "*HeightMap" )
        {
            file>>param;
            map[3] = loadImage(removeQuotes(param));
        }
        if( param == "*DiffuseColor" )
        {
            file >> m->diffuseColor.x;
            file >> m->diffuseColor.y;
            file >> m->diffuseColor.z;
        }
        if( param == "*SpecularColor" )
        {
            file >> m->specularColor.x;
            file >> m->specularColor.y;
            file >> m->specularColor.z;
        }
        if( param == "*SpecularFactor" )
        {
            file >> m->specularFactor;
        }
        if( param == "*ParallaxDepth" )
        {
            file >> m->parallaxDepth;
        }
        if( param == "*Shader" )
        {
            file >> param;
            m->shader = new Shader(removeQuotes(param));
        }
        if( param == "*Texture0Uniform")
        {
            file >> param;
            texture0Uniform = removeQuotes(param);
        }
        if( param == "*Texture1Uniform")
        {
            file >> param;
            texture1Uniform = removeQuotes(param);
        }

        file >> param;
    }

    file.close();

    tex[0] = mergeRGB_A(map[0],map[1]);
    tex[1] = mergeRGB_A(map[2],map[3]);

    // glGenTextures(1, &m->texture0);
    // glBindTexture(GL_TEXTURE_2D, m->texture0);
    // if(glfwLoadTexture2D(diffuseMap.c_str(),GLFW_BUILD_MIPMAPS_BIT) == GL_FALSE)
    // {
    //     printError("Matloader","could not load "+diffuseMap);
    // }


    // glGenTextures(1, &m->texture1);
    // glBindTexture(GL_TEXTURE_2D, m->texture1);
    // if(glfwLoadTexture2D(normalMap.c_str(),GLFW_BUILD_MIPMAPS_BIT) == GL_FALSE)
    // {
    //     printError("Matloader","could not load "+normalMap);
    // }

    m->texture0 = makeTexture(tex[0]);
    m->texture1 = makeTexture(tex[1]);

    if(m->shader->isCompiled())
    {
        int result;

        result = m->shader->getUniformLocation(texture0Uniform);
        if(result>-1)
            m->texture0Loc = (GLuint)result;
        else
            printWarning("matloader","could not find uniform "+texture0Uniform);

        result = m->shader->getUniformLocation(texture1Uniform);
        if(result>-1)
            m->texture1Loc = (GLuint)result;
        else
            printWarning("matloader","could not find uniform "+texture1Uniform);

        result = m->shader->getUniformLocation(std::string("ambientColor"));
        if(result>-1)
            m->ambientColorLoc = (GLuint)result;
        else
            printWarning("matloader","could not find uniform ambientColor");

        result = m->shader->getUniformLocation("diffuseColor");
        if(result>-1)
            m->diffuseColorLoc = (GLuint)result;
        else
            printWarning("matloader","could not find uniform diffuseColor");

        result = m->shader->getUniformLocation("specularColor");
        if(result>-1)
            m->specularColorLoc = (GLuint)result;
        else
            printWarning("matloader","could not find uniform specularColor");

        result = m->shader->getUniformLocation("specularFactor");
        if(result>-1)
            m->specularFactorLoc = (GLuint)result;
        else
            printWarning("matloader","could not find uniform specularFactor");

        result = m->shader->getUniformLocation("parallaxDepth");
        if(result>-1)
            m->parallaxDepthLoc = (GLuint)result;
        else
            printWarning("matloader","could not find uniform parallaxDepth");
    }

    ilDeleteImages(2,tex);
    ilDeleteImages(4,map);

    glBindTexture(GL_TEXTURE_2D,0);

    return m;
}

ILuint loadImage(const std::string &filename)
{

    ILuint texid;
    ILboolean success;

    ilGenImages(1, &texid); /* Generation of one image name */
    ilBindImage(texid); /* Binding of image name */
    success = ilLoadImage((const ILstring)filename.c_str()); /* Loading of image "image.jpg" */

    if (success) /* If no error occured: */
    {
        if(ilGetInteger(IL_IMAGE_BPP) > 1)
            ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        else
            ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
    }
    else
    {
        printError("loadImage","could not find "+filename);
        ilDefaultImage();
    }

    return texid;
}

ILuint mergeRGB_A(ILuint rgb, ILuint alpha)
{
    ILuint width, height;
    ILubyte *alphadata, *imgdata;
    ILuint img;

    ilBindImage(rgb);

    img = ilCloneCurImage();

    width = ilGetInteger(IL_IMAGE_WIDTH);
    height = ilGetInteger(IL_IMAGE_HEIGHT);

    ilBindImage(alpha);

    iluImageParameter( ILU_FILTER, ILU_SCALE_BSPLINE );
    iluScale(width,height,1);

    alphadata = ilGetData();

    //ilGenImages(1,&img);
    ilBindImage(img);

    ilRegisterFormat(IL_RGBA);
    ilRegisterType(IL_UNSIGNED_BYTE);

    imgdata = ilGetData();

    for(unsigned int i=0; i<width*height; i++)
        *(imgdata + i*4 + 3) = *(alphadata + i);
    
    return img;
}

GLuint makeTexture(ILuint img)
{
    ilBindImage(img); /* Binding of image name */

    int format = GL_RGBA8;

    GLuint texture;

    glGenTextures(1, &texture); /* Texture name generation */
    
    glBindTexture(GL_TEXTURE_2D, texture); /* Binding of texture name */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, ilGetInteger(IL_IMAGE_WIDTH),
    ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
    ilGetData()); /* Texture specification */

    glGenerateMipmap(GL_TEXTURE_2D);

    return texture;
}

int loadTexture(const std::string &filename)
{

    ILuint texid;
    ILboolean success;
    unsigned int texture;

    ilGenImages(1, &texid); /* Generation of one image name */
    ilBindImage(texid); /* Binding of image name */
    success = ilLoadImage((const ILstring)filename.c_str()); /* Loading of image "image.xxx" */

    if (success) /* If no error occured: */
    {
        ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
    }
    else
    {
        ilDefaultImage();
    }

    int format = GL_R8;

    if(ilGetInteger(IL_IMAGE_BPP) > 1)
        format = GL_RGBA8;

    glGenTextures(1, &texture); /* Texture name generation */
    
    glBindTexture(GL_TEXTURE_2D, texture); /* Binding of texture name */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, ilGetInteger(IL_IMAGE_WIDTH),
    ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
    ilGetData()); /* Texture specification */

    ilDeleteImages(1, &texid); /* Because we have already copied image data into texture data
    we can release memory used by image. */

    return (int)texture;
}