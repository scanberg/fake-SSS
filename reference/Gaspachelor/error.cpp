#include <GL/glew.h>
#include <GL/glfw.h>

#include "error.h"

void printError(std::string error_type, std::string error_msg)
{
    std::cout << "Error: " << error_type << " : " << error_msg << std::endl;
}

void printError(std::string error_type, const char* error_msg)
{
    std::cout << "Error: " << error_type << " : " << error_msg << std::endl;
}

void printWarning(std::string warning_type, std::string warning_msg)
{
	std::cout << "Warning: " << warning_type << " : " << warning_msg << std::endl;
}

void printWarning(std::string warning_type, const char* warning_msg)
{
	std::cout << "Warning: " << warning_type << " : " << warning_msg << std::endl;
}

void printGLerrors()
{
	GLenum e;
	while ((e=glGetError()) != GL_NO_ERROR)
	{
		std::cout<<"GLError: ";
		switch (e)
		{
			case GL_INVALID_ENUM:
				std::cout << "Invalid enum." << std::endl;
				break;
 
			case GL_INVALID_VALUE:
				std::cout << "Invalid value." << std::endl;
				break;
 
			case GL_INVALID_OPERATION:
				std::cout << "Invalid Operation." << std::endl;
				break;
 
			case GL_OUT_OF_MEMORY:
				std::cout << "Out of memory." <<  std::endl;
				break;
 
			default:
				std::cout << "Unknown error! Enum code is: " << e << std::endl;
				break;
 
		}
	}
}