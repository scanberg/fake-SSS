#ifndef Log_H
#define Log_H

#include <cstdlib>
#include <cstdarg>

#define logError(...) printf("ERROR (%s:%i): ", __FUNCTION__, __LINE__);printf(__VA_ARGS__);printf("\n")
#define logWarning(...) printf("WARNING (%s:%i): ", __FUNCTION__, __LINE__);printf(__VA_ARGS__);printf("\n")
#define logNote(...) printf("NOTE: ");printf(__VA_ARGS__);printf("\n")

#endif
