#ifndef GLEN_ENGINE_H
#define GLEN_ENGINE_H

#include "Types.h"
#include "Shader.h"

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_FULLSCREEN false

namespace glen
{
    class Engine
    {
    friend class Shader;
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

        Shader *getCurrentShader() { return currentShader; }

        static Engine *getInstance() { return instance; }
    private:
        void setCurrentShader( Shader* shad ) { currentShader = shad; }
        static Engine* instance;
        ivec2 windowSize;
        Shader *currentShader;
    };
}

#endif
