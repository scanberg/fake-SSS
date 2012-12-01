#include "Log.h"
#include "Types.h"

void printErrorsGL(const char *func, int line)
{
//
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
    	printf("%s:%i :\n",func,line);
    	switch(glErr)
    	{
    	
			case GL_INVALID_ENUM:
				printf("glError: Invalid enum \n");
				break;

			case GL_INVALID_VALUE:
				printf("glError: Invalid value\n");
				break;

			case GL_INVALID_OPERATION:
				printf("glError: Invalid operation \n");
				break;

			case GL_INVALID_FRAMEBUFFER_OPERATION:
				printf("glError: Invalid framebuffer operation \n");
				break;

			case GL_OUT_OF_MEMORY:
				printf("glError: Out of memory \n");
				break;

			#ifdef GL_STACK_UNDERFLOW
			case GL_STACK_UNDERFLOW:
				printf("glError: Stack underflow \n");
				break;
			#endif

			#ifdef GL_STACK_OVERFLOW
			case GL_STACK_OVERFLOW:
				printf("glError: Stack underflow \n");
				break;
			#endif

			default:
				printf("glError: unrecognized error \n");
    	}
        glErr = glGetError();
    }
}