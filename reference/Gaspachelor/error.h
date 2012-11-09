#ifndef ERROR_H
#define ERROR_H

#include <iostream>

void printError(std::string error_type, std::string error_msg);
void printError(std::string error_type, const char* error_msg);
void printWarning(std::string warning_type, std::string warning_msg);
void printWarning(std::string warning_type, const char* warning_msg);
void printGLerrors();

#endif
