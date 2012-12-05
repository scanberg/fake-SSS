#ifndef GLEN_CAMERA_H
#define GLEN_CAMERA_H

#include "Types.h"
#include "Engine.h"

#define CAMERA_PLANE_LEFT   0
#define CAMERA_PLANE_RIGHT  1
#define CAMERA_PLANE_BOTTOM 2
#define CAMERA_PLANE_TOP    3
#define CAMERA_PLANE_NEAR   4
#define CAMERA_PLANE_FAR    5

namespace glen
{

    class Camera
    {
    private:
        vec2 nearfar;
        ivec2 screensize;
        f32 fov;

        vec3 position;
        vec3 orientation;
        mat4 viewMat, projMat;

        vec4 plane[6];

        vec3 *lookat;

    public:
        void setup();
        void draw();
        Camera() : nearfar(vec2(0.1f,50.0f)), screensize(ivec2(640,480)), fov(60.0f), lookat(NULL) {}
        void setPosition(const vec3 &pos) { position = pos; }
        void setPosition(f32 x, f32 y, f32 z) { position = vec3(x,y,z);}
        void setOrientation(const vec3 &ori) { orientation = ori; }
        void setOrientation(f32 x, f32 y, f32 z) { orientation = vec3(x,y,z); }

        void lookAt(vec3 *pos){ lookat = pos; }
        void move(const vec3 &vec);
        void translate(const vec3 &vec) { position+=vec; }

        vec3 getPosition() { return position; }
        vec3 getOrientation() { return orientation; }

        const mat4 &getProjMatrix() { return projMat; }
        const mat4 &getViewMatrix() { return viewMat; }
    };

}

#endif
