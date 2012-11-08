#include "Geometry.h"
#include "Log.h"
#include "Tokenizer.h"
#include "Shader.h"

int main()
{
    Geometry g;
    logNote("Nice");

    glen::Shader shader("nice","shader");

    logError("Nice %i",2);

    Tokenizer tok("v  1 2 3   ");

    std::string str;

    while(tok.hasMore())
    {
        logNote(tok.getToken().c_str());
    }


    return 0;
}
