#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera
{
private:
    glm::vec2 nearfar;
    glm::ivec2 screensize;
    
    float fov;
    
    glm::vec3 position;
    glm::vec3 orientation;
    
    glm::vec3 *lookat;
    
public:
    void setup();
    void draw();
    Camera() : nearfar(glm::vec2(0.1f,100.0f)), screensize(glm::ivec2(640,480)), fov(60.0f), lookat(NULL) {}
    void setPosition(const glm::vec3 &pos) { position = pos; }
    void setPosition(float x, float y, float z) { position = glm::vec3(x,y,z);}
    void setOrientation(const glm::vec3 &ori) { orientation = ori; }
    void setOrientation(float x, float y, float z) { orientation = glm::vec3(x,y,z); }
    
    void lookAt(glm::vec3* pos){ lookat = pos; }
    
    void move(const glm::vec3 &vec);

    glm::vec3 getPosition() { return position; }
    glm::vec3 getOrientation() { return orientation; }

    glm::mat4 getProjMatrix();
    glm::mat4 getViewMatrix();
};

#endif
