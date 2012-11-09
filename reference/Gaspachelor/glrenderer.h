#ifndef GLRENDERER_H
#define GLRENDERER_H

#include <GL/glew.h>
#include <GL/glfw.h>

#include <glm/glm.hpp>
#include <IL/il.h>
#include <IL/ilu.h>

#include "camera.h"
#include "error.h"
#include "objloader.h"
#include "matloader.h"
#include "gmoloader.h"
#include "aseloader.h"
#include "light.h"

class GLRenderer
{
	private:
		class DeferredFBO
		{
		public:
			GLuint fbo;
			GLuint colorMap, normalMap, depthMap;

			void init(const glm::ivec2 &screenSize);
			void destroy();
		};
		static GLRenderer* context;

		glm::ivec2 screenSize;
 
		GLRenderer();
		~GLRenderer();
	public:
		static GLRenderer *getContext();

		void init(int width=640, int height=480, bool fullscreen=false, int fsaa=0);
		void terminate();

		void clearBuffers();
		void swapBuffers();
};

#endif