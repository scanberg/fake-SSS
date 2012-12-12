#ifndef GLEN_ENGINE_H
#define GLEN_ENGINE_H

#include "Types.h"
#include "Shader.h"

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768
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

        // Set functions
        void setWindowSize(ivec2 size);

        // Get functions
        Shader *getCurrentShader() { return currentShader; }
        ivec2 getWindowSize() { return windowSize; }

        // Static functions
        static Engine *getInstance() { return instance; }
    private:
        void setCurrentShader( Shader* shad ) { currentShader = shad; }
        static Engine* instance;
        ivec2 windowSize;
        Shader *currentShader;

        u32 fbo;
        u32 colorMap, normalMap, depthMap;
    };
}

#endif
