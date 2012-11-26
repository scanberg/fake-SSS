#ifndef Log_H
#define Log_H

#include <cstdlib>
#include <cstdarg>

void printErrorsGL(const char *func, int line);

#define logError(...) printf("ERROR (%s:%i): ", __FUNCTION__, __LINE__);printf(__VA_ARGS__);printf("\n")
#define logWarning(...) printf("WARNING (%s:%i): ", __FUNCTION__, __LINE__);printf(__VA_ARGS__);printf("\n")
#define logNote(...) printf("NOTE: ");printf(__VA_ARGS__);printf("\n")
#define logErrorsGL() printErrorsGL(__FUNCTION__, __LINE__)

#endif
