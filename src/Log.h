#ifndef Log_H
#define Log_H

#ifdef WIN32
    #include <windows.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <cstdarg>

#define logError(m) Logger::_error(__FUNCTION__, __LINE__, m)
#define logWarning(m) Logger::_warning(__FUNCTION__, __LINE__, m)
#define logNote Logger::_note

class Logger
{
public:
    static void _warning(const char* func, int line, const char* msg);
    static void _error(const char* func, int line, const char* msg);
    static void _note(const char* msg,...);

private:
    static HANDLE hConsole;
};



#endif
