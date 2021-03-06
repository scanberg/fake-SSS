/**
Copyright (C) 2012-2014 Robin Skånberg

Permission is hereby granted, free of charge,
to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CAMERA_H
#define CAMERA_H

#include "Types.h"
#include "Engine.h"

#define CAMERA_PLANE_LEFT   0
#define CAMERA_PLANE_RIGHT  1
#define CAMERA_PLANE_BOTTOM 2
#define CAMERA_PLANE_TOP    3
#define CAMERA_PLANE_NEAR   4
#define CAMERA_PLANE_FAR    5

class Camera
{
private:
	ivec2 screensize;
    vec2 nearfar;
    f32 fov;

    vec3 position;
    vec3 orientation;
    mat4 viewMat, projMat;
    mat4 invViewMat, invProjMat;

    vec4 plane[6];

    vec3 *lookat;

public:
    void setup();
    void draw();
    Camera(const ivec2& wh = ivec2(1280, 720), const vec2& nf = vec2(0.1f, 100.f), float f = glm::radians(60.f)) :
		screensize(wh), nearfar(nf), fov(f), lookat(NULL) {}

    void setPosition(const vec3 &pos) { position = pos; }
    void setPosition(f32 x, f32 y, f32 z) { position = vec3(x,y,z);}
    void setOrientation(const vec3 &ori) { orientation = ori; }
    void setOrientation(f32 x, f32 y, f32 z) { orientation = vec3(x,y,z); }

    void lookAt(vec3 *pos){ lookat = pos; }
    void move(const vec3 &vec);
    void translate(const vec3 &vec) { position+=vec; }

    const vec3 &getPosition() const { return position; }
    const vec3 &getOrientation() const { return orientation; }

    float getFov() const { return fov; }
	float getRatio() const { return (float)screensize.x / (float)screensize.y; }

    const mat4 &getProjMatrix() const { return projMat; }
    const mat4 &getViewMatrix() const { return viewMat; }
    const mat4 &getInverseViewMatrix() const { return invViewMat; }
    const mat4 &getInverseProjMatrix() const { return invProjMat; }
};

#endif
