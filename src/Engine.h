#ifndef GLEN_ENGINE_H
#define GLEN_ENGINE_H

#include "Types.h"

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_FULLSCREEN false

namespace glen
{
    class Engine
    {
    public:
        Engine();
        ~Engine();

        bool init(  i32 width=DEFAULT_WIDTH,
                    i32 height=DEFAULT_HEIGHT,
                    bool fullscreen=DEFAULT_FULLSCREEN);

        void update();
        void render();
        void clearBuffers();
        void swapBuffers();

        static Engine *getInstance() { return instance; }
    private:
        ivec2 windowSize;
        static Engine* instance;
    };
}

#endif
