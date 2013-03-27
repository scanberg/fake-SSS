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
