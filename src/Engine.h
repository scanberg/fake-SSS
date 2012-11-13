#ifndef GLEN_ENGINE_H
#define GLEN_ENGINE_H

#include "Types.h"

namespace glen
{
    class Engine
    {
    public:
        Engine();
        ~Engine();

        bool init(i32 width=640, i32 height=480, bool fullscreen=false, int samplesPerPixel=1);
        void update();
        void render();
        void swapBuffers();
    private:
        ivec2 windowSize;
    };
}

#endif
