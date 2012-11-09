//
//  Camera.cpp
//  Gestapo3
//
//  Created by Robin Skånberg on 2012-01-06.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

void Camera::move(const glm::vec3 &vec)
{
    glm::mat4 rotmat = glm::yawPitchRoll(glm::radians(orientation.y), glm::radians(orientation.x), glm::radians(orientation.z));
    //glm::mat4 m;
    //m = glm::rotate(m,orientation.x,glm::vec3(1.0,0.0,0.0));
    //m = glm::rotate(m,orientation.y,glm::vec3(0.0,1.0,0.0));
    //m = glm::rotate(m,orientation.z,glm::vec3(0.0,0.0,1.0));

    position += glm::vec3( rotmat * glm::vec4(vec,0.0) );
}

void Camera::draw()
{
    //frustum.drawLines();
    //frustum.drawNormals();
}

void Camera::setup()
{
    // Get window size. It may start out different from the requested
    // size, and will change if the user resizes the window.
    glfwGetWindowSize( &screensize.x, &screensize.y );
    if(screensize.y<=0) screensize.y=1; // Safeguard against iconified/closed window
    
    // Set viewport. This is the pixel rectangle we want to draw into.
    glViewport( 0, 0, screensize.x, screensize.y ); // The entire window
    
    // Select and setup the projection matrix.
    glMatrixMode(GL_PROJECTION); // "We want to edit the projection matrix"
    glLoadIdentity(); // Reset the matrix to identity
    
    GLfloat ratio = (GLfloat)screensize.x/(GLfloat)screensize.y;
    
    gluPerspective( fov, ratio, nearfar.x, nearfar.y );
    
    if (lookat)
    {
		gluLookAt(position.x, position.y, position.z, lookat->x, lookat->y, lookat->z, 0.0, 1.0, 0.0);
    }
    else
    {
        // Select and setup the modelview matrix.
        glMatrixMode( GL_MODELVIEW ); // "We want to edit the modelview matrix"
        glLoadIdentity(); // Reset the matrix to identity
        
        glRotatef(-orientation.x,1.0,0.0,0.0);
        glRotatef(-orientation.y,0.0,1.0,0.0);
        glRotatef(-orientation.z,0.0,0.0,1.0);
        
        glTranslatef(-position.x,-position.y,-position.z); //translate the screen
    }
}

glm::mat4 Camera::getViewMatrix()
{
    glm::mat4 m;
    if(lookat)
    {
        m = glm::lookAt(position,*lookat,glm::vec3(0.0,1.0,0.0));
    }
    else
    {
        m = glm::rotate(m,-orientation.x,glm::vec3(1.0,0.0,0.0));
        m = glm::rotate(m,-orientation.y,glm::vec3(0.0,1.0,0.0));
        m = glm::rotate(m,-orientation.z,glm::vec3(0.0,0.0,1.0));
        m = glm::translate(m,-position);
    }

    return m;
}

glm::mat4 Camera::getProjMatrix()
{
    // Get window size. It may start out different from the requested
    // size, and will change if the user resizes the window.
    glfwGetWindowSize( &screensize.x, &screensize.y );
    if(screensize.y<=0) screensize.y=1; // Safeguard against iconified/closed window
    
    // Set viewport. This is the pixel rectangle we want to draw into.
    glViewport( 0, 0, screensize.x, screensize.y ); // The entire window

    GLfloat ratio = (GLfloat)screensize.x/(GLfloat)screensize.y;

    return glm::perspective( fov, ratio, nearfar.x, nearfar.y );
}