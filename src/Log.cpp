#include "Log.h"

#define COLOR_DEFAULT 15
#define COLOR_WARNING 14
#define COLOR_ERROR 12
#define COLOR_NOTE 10

HANDLE Logger::hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void Logger::_error(const char* func, int line, const char* msg)
{
    #ifdef WIN32
    SetConsoleTextAttribute(hConsole, COLOR_ERROR);
    #endif

    printf("ERROR");

    #ifdef WIN32
    SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
    #endif

    printf(" in function '%s' at line %i: %s \n", func, line, msg);
}

void Logger::_warning(const char* func, int line, const char* msg)
{
    #ifdef WIN32
    SetConsoleTextAttribute(hConsole, COLOR_WARNING);
    #endif

    printf("WARNING");

    #ifdef WIN32
    SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
    #endif

    printf(" in function '%s' at line %i: %s \n", func, line, msg);
}

void Logger::_note(const char* msg)
{
    #ifdef WIN32
    SetConsoleTextAttribute(hConsole, COLOR_NOTE);
    #endif

    printf("NOTE");

    #ifdef WIN32
    SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
    #endif

    printf(" %s \n", msg);
}
