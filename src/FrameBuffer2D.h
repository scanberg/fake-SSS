#ifndef FRAMEBUFFER2D_H
#define FRAMEBUFFER2D_H

#include <GL/glew.h>
#include <GL/glfw.h>

class FrameBuffer2D
{
public:
	FrameBuffer2D(int numAuxBuffers = 1);
	~FrameBuffer2D();

	void assignAuxBuffer(	int auxIndex, int width, int height,
							int internalFormat, int format, int type,
							int textureMinFilter, int textureMaxFilter, int textureWrap);

	void assignDepthBuffer(	int width, int height,
							int internalFormat = 	GL_DEPTH_COMPONENT,
							int format = 			GL_DEPTH_COMPONENT,
							int type = 				GL_UNSIGNED_BYTE,
							int textureMinFilter = 	GL_NEAREST,
							int textureMaxFilter = 	GL_NEAREST,
							int textureWrapS = 		GL_CLAMP_TO_EDGE,
							int textureWrapT = 		GL_CLAMP_TO_EDGE);

	bool createBuffers();
	void destroyBuffers();

	void resizeBuffers(int bufferBit, int width, int height);

	void clearBuffers();

	void bind();
	void unbind();

private:
	int *auxWidth;
	int *auxHeight;
	int depthWidth;
	int depthHeight;

	int auxBuffersSize;
	unsigned int *auxHandle;
	unsigned int depthHandle;
};

#define FBO_AUX_BUFFER0 	0x00000000
#define FBO_AUX_BUFFER1 	0x00000001
#define FBO_AUX_BUFFER2 	0x00000002
#define FBO_AUX_BUFFER3 	0x00000004

#define FBO_DEPTH_BUFFER	0x00000008


#endif