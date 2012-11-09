#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLRenderer.h"

#define movespeed 0.3f
#define mousespeed 0.7f

void setupCamera(glm::mat4 &projMatrix, glm::mat4 &viewMatrix);
void steerCamera(Camera *cam);
void showFPS();

int main()
{
	GLRenderer *renderer = GLRenderer::getContext();
	renderer->init(640,480,false,0);

	glm::mat4 projMatrix, viewMatrix;
	Mesh m;
	Camera cam;

	cam.setPosition(-5,0,15);
	cam.setOrientation(0.0,-35.0,0.0);

	//Model *model = loadGmo("media/models/test.gmo");

	bool running=true;
	while(running)
	{
        renderer->clearBuffers();

		steerCamera(&cam);

		projMatrix = cam.getProjMatrix();
		viewMatrix = cam.getViewMatrix();

		//model->draw(projMatrix, viewMatrix);

        showFPS();
	
		renderer->swapBuffers();

		if(!glfwGetWindowParam(GLFW_OPENED) || glfwGetKey(GLFW_KEY_ESC))
			running = false;
	}

	//delete model;

	printGLerrors();

	renderer->terminate();

	return 0;
}

void steerCamera(Camera *cam)
{
	glm::ivec2 halfSize(320,240);
	glm::vec3 movevec;
	glm::vec3 camrot = cam->getOrientation();
        
    if(glfwGetKey('W'))
        movevec.z -= movespeed;
    if(glfwGetKey('A'))
        movevec.x -= movespeed;
    if(glfwGetKey('S'))
        movevec.z += movespeed;
    if(glfwGetKey('D'))
        movevec.x += movespeed;
    
    cam->move(movevec);
    
    int x,y;
    glfwGetMousePos(&x, &y);

    //std::cout<<"x: "<<x<<std::endl;
 
    camrot.x -= (float)(y-halfSize.y)*mousespeed;
    camrot.y -= (float)(x-halfSize.x)*mousespeed;

    //std::cout<<"camrot.x: "<<camrot.x<<std::endl;
    
    camrot.x = glm::clamp(camrot.x,-90.0f,90.0f);
    //if(camrot.x > 90.0f) camrot.x = 90.0f;
    //if(camrot.x < -90.0f) camrot.x = -90.0f;
    if(camrot.y > 360.0f) camrot.y -= 360.0f;
    if(camrot.y < -360.0f) camrot.y += 360.0f;
    
    cam->setOrientation(camrot);
    glfwSetMousePos(halfSize.x, halfSize.y);
}

void showFPS() {
    
    float t, fps;
    static float t0 = 0.0;
    static int frames = 0;
    static char titlestring[200];
    
    // Get current time
    t = glfwGetTime();  // Gets number of seconds since glfwInit()
    
    // If one second has passed, or if this is the very first frame
    if( (t-t0) > 1.0 || frames == 0 )
    {
        fps = (float)frames / (t-t0);
        sprintf(titlestring, "Verlet system (%.1f FPS)", fps);
        glfwSetWindowTitle(titlestring);
        t0 = t;
        frames = 0;
    }
    frames ++;
}